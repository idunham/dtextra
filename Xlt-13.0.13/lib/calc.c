/* $Id: calc.c,v 1.18 2001/06/20 19:43:23 amai Exp $ */

/* calc.c - Floating-point calculator V1.00 (C) Richard K. Lloyd 1992 */

/* amai: Everything not related to usage within libXlt has been removed.
         A version closer to the original will remain somewhere deep
         inside CVS.
       
         Finally do not try to learn programming from this hack -
         it's ugly and error-prone ...
         Check the trigonometric functions WRT degree vs. radiant!
 */

#include "../config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "calc.h"


#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif


#define MAXFACTOR  148  /* Get that lipstick out ! */
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


static double
calc_drand(void)
{
#ifdef HAVE_DRAND48
	return drand48();
#else
	return((double)rand()/RAND_MAX);
#endif
}


static void
calc_srand(long int seedval)
{
#ifdef HAVE_SRAND48
	srand48(seedval);
#else
	srand((unsigned int)seedval);
#endif
}


static void level_0(void);

static char *expr_line;
static char tempstr[255];
static char err_string[255];
static char expression[255], answer_str[255];
static int exp_len, hex_format;
static int exp_error;
static int scan, equals;
static int interact=FALSE;
static double facts[MAXFACTOR+1];
static double operand;
static char *envstr;


static char
*substr(char *mainstr, int startpos, int length)
{
   int mainsize=strlen(mainstr);
   int newpos=0;
   int oldpos=startpos-1;
   if (startpos<1 || startpos>mainsize || length<1) return("");
   while (oldpos<mainsize && newpos<length)
   {
      tempstr[newpos++]=mainstr[oldpos++];
   }
   tempstr[newpos]='\0';
   return(tempstr);
}


static int
instr(char *mainstr, char slicechar)
{
   int mainlen=strlen(mainstr);
   int slicepos=0;
   int found=FALSE;
   while (slicepos<mainlen && !found)
   {
      found=(mainstr[slicepos++]==slicechar);
   }
   if (found) return(slicepos); else return(0);
}


static void
randomize(long newseed)
{
   calc_srand(newseed);
}


static double
randval(long upper)
{
   if (upper<0) { randomize(-upper); return(upper); }
   if (upper<=1) return(calc_drand());
   return((long)(calc_drand()*upper+1));
}


static void
strip_expression(void)
{
   int copy1=0;
   int copy2=0;
   exp_len=strlen(expr_line);
   while (copy2<exp_len)
   {
      if (expr_line[copy2]==' ')
         copy2++;
      else
         expr_line[copy1++]=toupper(expr_line[copy2++]);
   }
   exp_len=copy1;
   expr_line[exp_len]='\0';
}


static void
crash(const char *error_message)
{
   if (!exp_error)
   {
      strcpy(err_string,error_message);
      exp_error=TRUE;
   }
}


static int
search(char *operator)
{
   int op_len=strlen(operator);
   int found_it=FALSE;
   if ((scan+op_len>exp_len) || exp_error) return(FALSE);
   if ((found_it=(strcmp(substr(expression,scan+1,op_len),operator)==0)))
      scan+=op_len;
   return(found_it);
}


static char
next(void)
{
   if (scan>=exp_len || exp_error) return('\0');
   return(expression[scan++]);
}


static double
get_number(void)
{
   char the_number[255],the_var[255],conv_str[63];
   int num_len=0;
   int OK=TRUE;
   int got_exp= -1;
   int got_hex=FALSE;
   int var_len=0;
   double answer;
   int answer_int;
   int status;
   char *envptr;
   if (search("PI")) return(M_PI);
   while (scan<exp_len && OK && !exp_error)
   {
      switch (expression[scan])
      {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
         if (var_len)
            the_var[var_len++]=next();
         else
            the_number[num_len++]=next();
         break;
      case '.':
         if (got_hex || var_len) OK=FALSE; else the_number[num_len++]=next();
         break;
      case '$':
      case '&':
         if (got_hex || num_len || var_len)
            OK=FALSE;
         else
         {
            scan++; got_hex=TRUE;
            the_number[0]='0'; the_number[1]='x';
            num_len+=2;
         }   
         break;
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'F':
         if (got_hex) the_number[num_len++]=next();
         else
            if (num_len) OK=FALSE; else the_var[var_len++]=next();
         break;
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case '_':
         if (num_len) OK=FALSE; else the_var[var_len++]=next();
         break;
      case 'E':
         if (var_len) the_var[var_len++]=next();
         else
            if (num_len)
            {
               the_number[num_len++]=next();
               if (!got_hex) got_exp=scan;
            }
            else OK=FALSE;
         break;
      case '+':
      case '-':
         if (scan==got_exp) the_number[num_len++]=next(); else OK=FALSE;
         break;
      default:
         OK=FALSE;
      }
   }
   the_number[num_len]='\0'; the_var[var_len]='\0';
   if (got_hex)
   {
      strcpy(conv_str,"Bad hexadecimal constant");
      status=(sscanf(the_number,"%x",&answer_int)<0);
      answer=(double)answer_int;
#ifdef DEBUG
      printf("String = %s, Value = %f\n",the_number,answer);
#endif
   }
   else
   {
      status=FALSE;
      if (var_len)
      {
         sprintf(conv_str,"Bad symbol (%s)",the_var);
#ifdef DEBUG
         printf("Reading %d char var (%s)\n",strlen(the_var),the_var);
         printf("Printf claims A=%s\n",getenv(the_var));
#endif
         envptr=getenv(the_var);
         if (envptr==NULL)
            status=TRUE;
         else
         {
            strcpy(the_number,envptr);
#ifdef DEBUG
            printf("Translation of %s = %s\n",the_var,the_number);
#endif
         }
      }
      else strcpy(conv_str,"Bad decimal constant");
      if (!status) status=(sscanf(the_number,"%lf",&answer)<0);
   }
   if (status) crash(conv_str);
   return(answer);
}


static void
bracket(void)
{
   level_0();
   if (next()!=')') crash("Missing bracket");
}


static double
compute_sin(double angle)
{
   return(sin(M_PI*angle/180));
}


static double 
compute_cos(double angle)
{
   return(cos(M_PI*angle/180));
}


static double 
compute_tan(double angle)
{
   return(tan(M_PI*angle/180));
}


static void
eval_operand(void)
{
   if (search("INT"))
   {
      eval_operand();
      if (operand>INT_MAX || operand<-INT_MAX)
         crash("Floating-point number too large to be truncated");
      else
         operand=(long)operand;
   }
   else
   if (search("SQRT"))
   {
      eval_operand();
      if (operand<0) crash("Negative square root"); else operand=sqrt(operand);
   }
   else
   if (search("SIN"))
   {
      eval_operand(); operand=compute_sin(operand);
   }
   else
   if (search("COS"))
   {
      eval_operand(); operand=compute_cos(operand);
   }
   else
   if (search("TAN"))
   {
      eval_operand(); operand=compute_tan(operand);
   }
   else
   if (search("ASN"))
   {
      eval_operand();
      if (abs((int)operand)>1) crash("Bad arcsine");
         else operand=asin(operand);
   }
   else
   if (search("ACS"))
   {
      eval_operand();
      if (abs((int)operand)>1) crash("Bad arccosine");
         else operand=acos(operand);
   }
   else
   if (search("EXP"))
   {
      eval_operand(); operand=exp(operand);
   }
   else
   if (search("LN"))
   {
      eval_operand();
      if (operand<=0) crash("Natural log error");
         else operand=log(operand);
   }
   else
   if (search("LOG"))
   {
      eval_operand();
      if (operand<=0) crash("Base 10 log error");
         else operand=log10(operand);
   }
   else
   if (search("ATN"))
   {
      eval_operand(); operand=atan(operand);
   }
   else
   if (search("RND"))
   {
      eval_operand();
      operand=randval((long)operand);
   }
   else
   if (search("TIME")) operand=time(0);
   else
   if (search("FACT"))
   {
      eval_operand();
      if (operand<0 || operand>MAXFACTOR)
         crash("Out of bounds factorial value");
      else
         operand=facts[(long)operand];
   }
   else
   if (search("-"))
   {
      eval_operand(); operand= -operand;
   }
   else
   if (search("+")) eval_operand();
   else
   if (search("(")) bracket(); else operand=get_number();
}


static void
level_4(void)
{
   double temp_operand;
   eval_operand();
   while (search("^"))
   {
      if (operand<=0.)
         crash("Cannot raise zero or less to the power of a value");
      else
      {
         temp_operand=operand;
         eval_operand();
         if (!exp_error) operand=pow(temp_operand,operand);
      }
   }
}


static void
level_3(void)
{
   double temp_operand;
   level_4();
   while (search("/"))
   {
      temp_operand=operand;
      level_4();
      if (operand==0.) crash("Division by zero");
      else
      {
         if (search("%")) operand=operand/100;
         operand=temp_operand/operand;
      }
   }
}


static void
level_2(void)
{
   double temp_operand;
   level_3();
   while (search("*"))
   {
      temp_operand=operand;
      level_2();
      if (search("%")) operand=operand/100;
      operand=temp_operand*operand;
   }
}


static void
level_1(void)
{
   double temp_operand;
   level_2();
   while (search("-"))
   {
      temp_operand=operand;
      level_2();
      if (search("%")) operand=temp_operand*operand/100;
      operand=temp_operand-operand;
   }
}


static void
level_0(void)
{
    double temp_operand;
    level_1();
    if (search("+"))
    {
       temp_operand=operand;
       level_0();
       if (search("%")) operand=temp_operand*operand/100;
       operand+=temp_operand;
    }
}


static void
eval_statement(void)
{
   char signstr[10];
   int ans_len;
   scan=0; exp_error=FALSE; level_0();
   if (scan<=exp_len-1) crash("Missing operand");
   if (equals==0 && interact && !exp_error) printf("%s",expression);
   if (exp_error)
      fprintf(stderr,"%s contains an error : %s\n",expression,err_string);
   else
   {
      if (hex_format)
      {
         if (operand<0) strcpy(signstr,"-$"); else strcpy(signstr,"$");
         sprintf(answer_str,"%s%lx",signstr,(long)operand);
      }
      else
      {
         sprintf(answer_str,"%.14f",operand);
         ans_len=strlen(answer_str)-1;
         while (answer_str[ans_len]=='0')
         {
            ans_len--;
         }
         if (answer_str[ans_len]=='.') ans_len--;
         answer_str[ans_len+1]='\0';
      }
      if (equals==0 && !exp_error)
      {
         if (interact) printf(" = ");
#if 0
         printf("%s\n",answer_str);
#endif
      }
   }
}


static void
show_help(void)
{
   printf("This is a BASIC-style expression evaluator which understands the following\n");
   printf("operators (all trigonometrical functions use degrees):\n");
   printf("+,-,*,/,^   Addition, subtraction, multiplication, division and power.\n");
   printf("()          Brackets (overrides operator precedence).\n");
   if (!interact)
   printf("            Be careful with * on the command line - use \\* to avoid expansion.\n");
   printf("%%           Percentage (works in combination with +,-,* and /).\n");
   printf("~           Print result as a hexadecimal number.\n");
   printf("$ or &      Hexadecimal constant follows.\n");
   if (interact)
   printf("!command    Issue a shell command.\n");
   printf("ACS expr    Arccosine.\n");
   printf("ASN expr    Arcsine.\n");
   printf("ATN expr    Arctangent.\n");
   printf("COS expr    Cosine.\n");
   printf("EXP expr    Exponential power.\n");
   printf("FACT expr   Factorial (expr must be between 0 and %d).\n",MAXFACTOR);
   printf("LN expr     Natural log (expr must be positive).\n");
   printf("LOG expr    Log to base 10 (expr must be positive).\n");
   printf("RND expr    Random number generator.\n");
   printf("            If expr<0, then expr is used to seed the random number generator.\n");
   printf("            If 0<=expr<=1, then returns a real number between 0 and 1.\n");
   printf("            If expr>1, then returns an integer between 1 and expr.\n");
   printf("SIN expr    Sine.\n");
   printf("SQRT expr   Square root.\n");
   printf("TAN expr    Tangent.\n");
   printf("TIME        Returns the number of seconds since 1st January 1970.\n");
}


static void
split_line(void)
{
   char lhs[255];
   int comma;
   int line_len=strlen(expr_line);
   while (line_len)
   {
      exp_len=instr(expr_line,',')-1;
      if (exp_len<0) exp_len=line_len;
      comma=exp_len;
      hex_format=(expr_line[0]=='~');
      if (hex_format)
      {
         exp_len-=1;
         strcpy(expression,substr(expr_line,2,exp_len));
      }
      else strcpy(expression,substr(expr_line,1,exp_len));
      equals=instr(expression,'=');
      if (equals)
      {
         if (equals==1)
         {
            printf("Warning ! Discarded result ! "); exp_len-=1;
            strcpy(expression,substr(expression,2,exp_len));
            equals=0;
         }
         else
         {
            exp_len-=equals;
            strcpy(lhs,substr(expression,1,equals-1));
            strcpy(expression,substr(expression,equals+1,exp_len));
         }
      }
      if (interact &&
         (strcmp(expression,"QUIT")==0 || strcmp(expression,"EXIT")==0))
      {
         line_len=0;
      }
      else
      if (interact && strcmp(expression,"HELP")==0)
      {
         line_len=0; 
         show_help();
      }
      else
      {
         eval_statement();
         if (equals)
         {
#ifdef VMS
/* Put something in here to assign to a DCL symbol */
#else
/* This next b*****d bit took me AGES to work out ! If you do multiple putenv
   stuff, you have to MALLOC the strings, otherwise they get overwritten. I
   think the putenv man page should make this very clear indeed (instead of
   some vague waffle about "automatic variables") */
            envstr=malloc(255);
            sprintf(envstr,"%s=%s",lhs,answer_str);
            putenv(envstr);
#ifdef DEBUG
            printf("Env str = %s, New value of %s = %s\n",envstr,lhs,getenv(lhs));
#endif
#endif
         }
         strcpy(expr_line,substr(expr_line,comma+2,line_len-comma-1));
         line_len=strlen(expr_line);
      }
   }
}


static void
init(void)
{
   int i;

   facts[0]=1.;
   for (i=1; i<=MAXFACTOR; i++)
   {
      facts[i]=facts[i-1]*i;
   }
   
   randomize(time(0)); /* Equivalent to RND(-TIME) */
}


extern double 
XltCalc(const char *cmd)
{

	static int IsInit = FALSE;
	int cmdlen;

	if (!IsInit) {
		init();
		IsInit=TRUE;
	}
	answer_str[0] = '\0';
	cmdlen=strlen(cmd);
	expr_line = (char *)malloc(cmdlen+1);
	strcpy(expr_line, cmd);
	strip_expression();
	split_line();
	free(expr_line);
	return(operand);
}
