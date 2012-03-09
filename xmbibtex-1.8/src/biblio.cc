/* .................................... */
/* ...    biblio.cc                 ... */
/* ...    last change 08.05.1997    ... */
/* .................................... */

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <strstream>


#include "biblio.h"

/* ...   private functions   ... */
  char *BIBLIO::exright(int i,char *s) {
    char *p;
    char h[MAX_FIELD_LENGTH];
    int  hp;

    p=s;
    hp=0;
    while (*s!='\n' && *s!=0x0D && *s && hp<MAX_FIELD_LENGTH-5) {
      switch(*s) {
      case '\t':
        break;
      case '\"':
	h[hp]='{';    h[hp+1]='\"';
        h[hp+2]='}';
        hp+=3;
        break;
      case 'á':
	h[hp]='{';   h[hp+1]='\\';
	h[hp+2]='s'; h[hp+3]='s'; h[hp+4]='}';
	hp+=5;
        break;
      case 'š':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\"';
	h[hp+3]='U'; h[hp+4]='}';
	hp+=5;
        break;
      case 'Ž':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\"';
	h[hp+3]='A'; h[hp+4]='}';
	hp+=5;
        break;
      case '„':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\"';
	h[hp+3]='a'; h[hp+4]='}';
	hp+=5;
        break;
      case '”':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\"';
	h[hp+3]='o'; h[hp+4]='}';
	hp+=5;
        break;
      case '':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\"';
	h[hp+3]='u'; h[hp+4]='}';
	hp+=5;
        break;
      case '‚':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='\'';
	h[hp+3]='e'; h[hp+4]='}';
	hp+=5;
        break;
      case 'Š':
	h[hp]='{'; h[hp+1]='\\'; h[hp+2]='`';
	h[hp+3]='e'; h[hp+4]='}';
	hp+=5;
        break;
      case '%':
	h[hp]='\\'; h[hp+1]='%';
	hp+=2;
        break;
      default: h[hp]=s[0]; hp++;
      }
      s++;
    }
    h[hp]='\0';
    strcpy(p,h);
    s=p;
    return(p+i);
  }

void BIBLIO::put_id(unsigned long n, unsigned long refid)
{
  n--;
  c[n]->id=refid;
}

void BIBLIO::put_doctype(unsigned long n, char *s)
{
  n--;
  c[n]->doctype=new char[strlen(s)+1];
  strcpy(c[n]->doctype,s);
}

void BIBLIO::add_author(unsigned long n, char *s)
{
  int  size, ntok,i;
  char *tok;
  char last[100]="", first[100]="", von[100]="", jr[100]="";

  tok=strtok(s," ,."); ntok=1;
  while (tok) {
    if(!strcasecmp(tok,"de")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"von")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"van")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"du")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"la")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"jr")) {
      strcat(jr,tok);  strcat(jr, " "); }
    else if(!strcmp(tok,"Th")) {
      strcat(first,tok);  strcat(first, ". "); }
    else if(islower(tok[0]))    {
      strcat(von,tok); strcat(von," "); }
    else if(isupper(tok[0]) && ntok==1) {
      strcat(last,tok); strcat(last," "); ntok++;}
    else if(isupper(tok[0]) && ntok>1 && strlen(tok)==1) {
      strcat(first,tok); strcat(first,". "); }
    else if(isupper(tok[0]) && ntok>1 && strlen(tok)>1) {
      if(islower(tok[1])) {
	strcat(last,tok); strcat(last," ");}
      if(isupper(tok[1])) 
	for(i=0;i<strlen(tok);i++) {
	  strncat(first,tok+i,1);
	  strcat(first,". ");}
    }
    tok=strtok(NULL," ,.");
  }

  strcpy(s,"");
  if(strlen(von)!=0) strcat(s,von);
  if(strlen(last)!=0) {
    strcat(s,last);
    if(strlen(jr)!=0 || strlen(first)!=0) {
      s[strlen(s)-1]=',';
      strcat(s," ");
    }
  }
  if(strlen(jr)!=0)
    if(strlen(first)!=0) strcat(s,"Jr., ");
    else strcat(s,"Jr.");
  if(strlen(first)!=0) strncat(s,first,strlen(first)-1);

  n--;
  if (!c[n]->author) {
    c[n]->author=new char[strlen(s)+1];
    strcpy(c[n]->author,s);
  }
  else {
    size=strlen(c[n]->author);
    c[n]->author=(char *)realloc(c[n]->author,size+strlen(s)+5+1);
    strcat(c[n]->author," and ");
    strcat(c[n]->author,s);
  }
}
void BIBLIO::add_editor(unsigned long n, char *s)
{
  int  size, ntok,i;
  char *tok;
  char last[100]="", first[100]="", von[100]="", jr[100]="";

  tok=strtok(s," ,."); ntok=1;
  while (tok) {
    if(!strcasecmp(tok,"de")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"von")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"van")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"du")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"la")) {
      strcat(von,tok); strcat(von," "); }
    else if(!strcasecmp(tok,"jr")) {
      strcat(jr,tok);  strcat(jr, " "); }
    else if(!strcmp(tok,"Th")) {
      strcat(first,tok);  strcat(first, ". "); }
    else if(islower(tok[0]))    {
      strcat(von,tok); strcat(von," "); }
    else if(isupper(tok[0]) && ntok==1) {
      strcat(last,tok); strcat(last," "); ntok++;}
    else if(isupper(tok[0]) && ntok>1 && strlen(tok)==1) {
      strcat(first,tok); strcat(first,". "); }
    else if(isupper(tok[0]) && ntok>1 && strlen(tok)>1) {
      if(islower(tok[1])) {
	strcat(last,tok); strcat(last," ");}
      if(isupper(tok[1])) 
	for(i=0;i<strlen(tok);i++) {
	  strncat(first,tok+i,1);
	  strcat(first,". ");}
    }
    tok=strtok(NULL," ,.");
  }

  strcpy(s,"");
  if(strlen(von)!=0) strcat(s,von);
  if(strlen(last)!=0) {
    strcat(s,last);
    if(strlen(jr)!=0 || strlen(first)!=0) {
      s[strlen(s)-1]=',';
      strcat(s," ");
    }
  }
  if(strlen(jr)!=0)
    if(strlen(first)!=0) strcat(s,"Jr., ");
    else strcat(s,"Jr.");
  if(strlen(first)!=0) strncat(s,first,strlen(first)-1);

  n--;
  if (!c[n]->editor) {
    c[n]->editor=new char[strlen(s)+1];
    strcpy(c[n]->editor,s);
  }
  else {
    size=strlen(c[n]->editor);
    c[n]->editor=(char *)realloc(c[n]->editor,size+strlen(s)+5+1);
    strcat(c[n]->editor," and ");
    strcat(c[n]->editor,s);
  }
}

void BIBLIO::put_title(unsigned long n, char *s)
{
  n--;
  c[n]->title=new char[strlen(s)+1];
  strcpy(c[n]->title,s);
}

void BIBLIO::put_booktitle(unsigned long n, char *s)
{
  n--;
  c[n]->booktitle=new char[strlen(s)+1];
  strcpy(c[n]->booktitle,s);
}

void BIBLIO::put_institution(unsigned long n, char *s)
{
  n--;
  c[n]->institution=new char[strlen(s)+1];
  strcpy(c[n]->institution,s);
}

void BIBLIO::put_abstract(unsigned long n, char *s)
{
  n--;
  c[n]->abstract=new char[strlen(s)+1];
  strcpy(c[n]->abstract,s);
}

void BIBLIO::put_journal(unsigned long n, char *s, unsigned long i)
{
  n--;
  c[n]->journal=new char[i+1];
  strncpy(c[n]->journal,s,i);
  c[n]->journal[i]='\0';
}

void BIBLIO::put_publisher(unsigned long n, char *s, unsigned long i)
{
  n--;
  c[n]->publisher=new char[i+1];
  strncpy(c[n]->publisher,s,i);
  c[n]->publisher[i]='\0';
}

void BIBLIO::put_year(unsigned long n, char *s)
{
  std::ostrstream year;
  int  y;

  n--;
  y=atoi(s);
  year << y << '\0';
  c[n]->year=year.str();
}

void BIBLIO::put_volume(unsigned long n, char *s)
{
  int  i;

  n--;
  i=strcspn(s,":");
  c[n]->volume=new char[i+1];
  strncpy(c[n]->volume,s,i); c[n]->volume[i]='\0';
}

void BIBLIO::put_pages(unsigned long n, char *s)
{
  n--;
  c[n]->pages=new char[strlen(s)+1];
  strcpy(c[n]->pages,s);
}

void BIBLIO::add_keyword(unsigned long n, char *s)
{
  int size;
  n--;
  if (!c[n]->keyword) {
    c[n]->keyword=new char[strlen(s)+1];
    strcpy(c[n]->keyword,s);
  }
  else {
    size=strlen(c[n]->keyword);
    c[n]->keyword=(char *)realloc(c[n]->keyword,size+strlen(s)+1+1);
    strcat(c[n]->keyword,"\n");
    strcat(c[n]->keyword,s);
  }
}

void BIBLIO::put_note(unsigned long n, char *s)
{
  n--;
  c[n]->note=new char[strlen(s)+1];
  strcpy(c[n]->note,s);
}

void BIBLIO::put_comment(unsigned long n, char *s)
{
  n--;
  c[n]->comment=new char[strlen(s)+1];
  strcpy(c[n]->comment,s);
}

void BIBLIO::put_number(unsigned long n, char *s)
{
  std::ostrstream number;
  int  y;

  n--;
  y=atoi(s);
  number << y << '\0';
  c[n]->number=number.str();
}

void BIBLIO::put_month(unsigned long n, char *s)
{
  n--;
  c[n]->month=new char[strlen(s)+1];
  strcpy(c[n]->month,s);
}



void BIBLIO::add_field(char *t, unsigned long n, char *s)
{
  int ablen;
  n--;

  if(t=="abstract") {
    if(!c[n]->abstract) {
      c[n]->abstract=new char[strlen(s)+1];
      strcpy(c[n]->abstract,s);
    }
    else {
      ablen=strlen(c[n]->abstract)+strlen(s)+2;
      c[n]->abstract=(char *)realloc(c[n]->abstract,ablen);
      strcat(c[n]->abstract,"\n");
      strcat(c[n]->abstract,s);
    }
  }
  else if(t=="address") {
    if(!c[n]->address) {
      c[n]->address=new char[strlen(s)+1];
      strcpy(c[n]->address,s);
    }
    else {
      ablen=strlen(c[n]->address);
      c[n]->address=(char *)realloc(c[n]->address,ablen+strlen(s)+2);
      strcat(c[n]->address,"\n");
      strcat(c[n]->address,s);
    }
  }
  else if(t=="annote") {
    if(!c[n]->annote) {
      c[n]->annote=new char[strlen(s)+1];
      strcpy(c[n]->annote,s);
    }
    else {
      ablen=strlen(c[n]->annote);
      c[n]->annote=(char *)realloc(c[n]->annote,ablen+strlen(s)+2);
      strcat(c[n]->annote,"\n");
      strcat(c[n]->annote,s);
    }
  }
  else if(t=="author") {
    if(!c[n]->author) {
      c[n]->author=new char[strlen(s)+1];
      strcpy(c[n]->author,s);
    }
    else {
      ablen=strlen(c[n]->author);
      c[n]->author=(char *)realloc(c[n]->author,ablen+strlen(s)+2);
      strcat(c[n]->author,"\n");
      strcat(c[n]->author,s);
    }
  }
  else if(t=="booktitle") {
    if(!c[n]->booktitle) {
      c[n]->booktitle=new char[strlen(s)+1];
      strcpy(c[n]->booktitle,s);
    }
    else {
      ablen=strlen(c[n]->booktitle);
      c[n]->booktitle=(char *)realloc(c[n]->booktitle,ablen+strlen(s)+2);
      strcat(c[n]->booktitle,"\n");
      strcat(c[n]->booktitle,s);
    }
  }
  else if(t=="chapter") {
    if(!c[n]->chapter) {
      c[n]->chapter=new char[strlen(s)+1];
      strcpy(c[n]->chapter,s);
    }
    else {
      ablen=strlen(c[n]->chapter);
      c[n]->chapter=(char *)realloc(c[n]->chapter,ablen+strlen(s)+2);
      strcat(c[n]->chapter,"\n");
      strcat(c[n]->chapter,s);
    }
  }
  else if(t=="comment") {
    if(!c[n]->comment) {
      c[n]->comment=new char[strlen(s)+1];
      strcpy(c[n]->comment,s);
    }
    else {
      ablen=strlen(c[n]->comment);
      c[n]->comment=(char *)realloc(c[n]->comment,ablen+strlen(s)+2);
      strcat(c[n]->comment,"\n");
      strcat(c[n]->comment,s);
    }
  }
  else if(t=="crossref") {
    if(!c[n]->crossref) {
      c[n]->crossref=new char[strlen(s)+1];
      strcpy(c[n]->crossref,s);
    }
    else {
      ablen=strlen(c[n]->crossref);
      c[n]->crossref=(char *)realloc(c[n]->crossref,ablen+strlen(s)+2);
      strcat(c[n]->crossref,"\n");
      strcat(c[n]->crossref,s);
    }
  }
  else if(t=="edition") {
    if(!c[n]->edition) {
      c[n]->edition=new char[strlen(s)+1];
      strcpy(c[n]->edition,s);
    }
    else {
      ablen=strlen(c[n]->edition);
      c[n]->edition=(char *)realloc(c[n]->edition,ablen+strlen(s)+2);
      strcat(c[n]->edition,"\n");
      strcat(c[n]->edition,s);
    }
  }
  else if(t=="editor") {
    if(!c[n]->editor) {
      c[n]->editor=new char[strlen(s)+1];
      strcpy(c[n]->editor,s);
    }
    else {
      ablen=strlen(c[n]->editor);
      c[n]->editor=(char *)realloc(c[n]->editor,ablen+strlen(s)+2);
      strcat(c[n]->editor,"\n");
      strcat(c[n]->editor,s);
    }
  }
  else if(t=="howpublished") {
    if(!c[n]->howpublished) {
      c[n]->howpublished=new char[strlen(s)+1];
      strcpy(c[n]->howpublished,s);
    }
    else {
      ablen=strlen(c[n]->howpublished);
      c[n]->howpublished=(char *)realloc(c[n]->howpublished,ablen+strlen(s)+2);
      strcat(c[n]->howpublished,"\n");
      strcat(c[n]->howpublished,s);
    }
  }
  else if(t=="institution") {
    if(!c[n]->institution) {
      c[n]->institution=new char[strlen(s)+1];
      strcpy(c[n]->institution,s);
    }
    else {
      ablen=strlen(c[n]->institution);
      c[n]->institution=(char *)realloc(c[n]->institution,ablen+strlen(s)+2);
      strcat(c[n]->institution,"\n");
      strcat(c[n]->institution,s);
    }
  }
  else if(t=="journal") {
    if(!c[n]->journal) {
      c[n]->journal=new char[strlen(s)+1];
      strcpy(c[n]->journal,s);
    }
    else {
      ablen=strlen(c[n]->journal);
      c[n]->journal=(char *)realloc(c[n]->journal,ablen+strlen(s)+2);
      strcat(c[n]->journal,"\n");
      strcat(c[n]->journal,s);
    }
  }
  else if(t=="key") {
    if(!c[n]->key) {
      c[n]->key=new char[strlen(s)+1];
      strcpy(c[n]->key,s);
    }
    else {
      ablen=strlen(c[n]->key);
      c[n]->key=(char *)realloc(c[n]->key,ablen+strlen(s)+2);
      strcat(c[n]->key,"\n");
      strcat(c[n]->key,s);
    }
  }
  else if(t=="keyword") {
    if(!c[n]->keyword) {
      c[n]->keyword=new char[strlen(s)+1];
      strcpy(c[n]->keyword,s);
    }
    else {
      ablen=strlen(c[n]->keyword);
      c[n]->keyword=(char *)realloc(c[n]->keyword,ablen+strlen(s)+2);
      strcat(c[n]->keyword,"\n");
      strcat(c[n]->keyword,s);
    }
  }
  else if(t=="month") {
    if(!c[n]->month) {
      c[n]->month=new char[strlen(s)+1];
      strcpy(c[n]->month,s);
    }
    else {
      ablen=strlen(c[n]->month);
      c[n]->month=(char *)realloc(c[n]->month,ablen+strlen(s)+2);
      strcat(c[n]->month,"\n");
      strcat(c[n]->month,s);
    }
  }
  else if(t=="note") {
    if(!c[n]->note) {
      c[n]->note=new char[strlen(s)+1];
      strcpy(c[n]->note,s);
    }
    else {
      ablen=strlen(c[n]->note);
      c[n]->note=(char *)realloc(c[n]->note,ablen+strlen(s)+2);
      strcat(c[n]->note,"\n");
      strcat(c[n]->note,s);
    }
  }
  else if(t=="number") {
    if(!c[n]->number) {
      c[n]->number=new char[strlen(s)+1];
      strcpy(c[n]->number,s);
    }
    else {
      ablen=strlen(c[n]->number);
      c[n]->number=(char *)realloc(c[n]->number,ablen+strlen(s)+2);
      strcat(c[n]->number,"\n");
      strcat(c[n]->number,s);
    }
  }
  else if(t=="organization") {
    if(!c[n]->organization) {
      c[n]->organization=new char[strlen(s)+1];
      strcpy(c[n]->organization,s);
    }
    else {
      ablen=strlen(c[n]->organization);
      c[n]->organization=(char *)realloc(c[n]->organization,ablen+strlen(s)+2);
      strcat(c[n]->organization,"\n");
      strcat(c[n]->organization,s);
    }
  }
  else if(t=="pages") {
    if(!c[n]->pages) {
      c[n]->pages=new char[strlen(s)+1];
      strcpy(c[n]->pages,s);
    }
    else {
      ablen=strlen(c[n]->pages);
      c[n]->pages=(char *)realloc(c[n]->pages,ablen+strlen(s)+2);
      strcat(c[n]->pages,"\n");
      strcat(c[n]->pages,s);
    }
  }
  else if(t=="publisher") {
    if(!c[n]->publisher) {
      c[n]->publisher=new char[strlen(s)+1];
      strcpy(c[n]->publisher,s);
    }
    else {
      ablen=strlen(c[n]->publisher);
      c[n]->publisher=(char *)realloc(c[n]->publisher,ablen+strlen(s)+2);
      strcat(c[n]->publisher,"\n");
      strcat(c[n]->publisher,s);
    }
  }
  else if(t=="reprint") {
    if(!c[n]->reprint) {
      c[n]->reprint=new char[strlen(s)+1];
      strcpy(c[n]->reprint,s);
    }
    else {
      ablen=strlen(c[n]->reprint);
      c[n]->reprint=(char *)realloc(c[n]->reprint,ablen+strlen(s)+2);
      strcat(c[n]->reprint,"\n");
      strcat(c[n]->reprint,s);
    }
  }
  else if(t=="school") {
    if(!c[n]->school) {
      c[n]->school=new char[strlen(s)+1];
      strcpy(c[n]->school,s);
    }
    else {
      ablen=strlen(c[n]->school);
      c[n]->school=(char *)realloc(c[n]->school,ablen+strlen(s)+2);
      strcat(c[n]->school,"\n");
      strcat(c[n]->school,s);
    }
  }
  else if(t=="series") {
    if(!c[n]->series) {
      c[n]->series=new char[strlen(s)+1];
      strcpy(c[n]->series,s);
    }
    else {
      ablen=strlen(c[n]->series);
      c[n]->series=(char *)realloc(c[n]->series,ablen+strlen(s)+2);
      strcat(c[n]->series,"\n");
      strcat(c[n]->series,s);
    }
  }
  else if (t=="title") {
    if(!c[n]->title) {
      c[n]->title=new char[strlen(s)+1];
      strcpy(c[n]->title,s);
    }
    else {
      ablen=strlen(c[n]->title);
      c[n]->title=(char *)realloc(c[n]->title,ablen+strlen(s)+2);
      strcat(c[n]->title,"\n");
      strcat(c[n]->title,s);
    }
  }
  else if (t=="type") {
    if(!c[n]->type) {
      c[n]->type=new char[strlen(s)+1];
      strcpy(c[n]->type,s);
    }
    else {
      ablen=strlen(c[n]->type);
      c[n]->type=(char *)realloc(c[n]->type,ablen+strlen(s)+2);
      strcat(c[n]->type,"\n");
      strcat(c[n]->type,s);
    }
  }
  else if (t=="volume") {
    if(!c[n]->volume) {
      c[n]->volume=new char[strlen(s)+1];
      strcpy(c[n]->volume,s);
    }
    else {
      ablen=strlen(c[n]->volume);
      c[n]->volume=(char *)realloc(c[n]->volume,ablen+strlen(s)+2);
      strcat(c[n]->volume,"\n");
      strcat(c[n]->volume,s);
    }
  }
  else if (t=="year") {
    if(!c[n]->year) {
      c[n]->year=new char[strlen(s)+1];
      strcpy(c[n]->year,s);
    }
    else {
      ablen=strlen(c[n]->year);
      c[n]->year=(char *)realloc(c[n]->year,ablen+strlen(s)+2);
      strcat(c[n]->year,"\n");
      strcat(c[n]->year,s);
    }
  }
}


/* ............................ */
/* ...   public functions   ... */
/* ............................ */
int BIBLIO::Read_BibTeX(char *fname) {
  std::ifstream   fin;
  int        rc=0, end_entry=0, pos, enclev;
  char       *s, *sp, *hs, *sanc, encstart, encend;

  s=new char[MAX_FIELD_LENGTH];
  sanc=s;
  hs=new char[MAX_FIELD_LENGTH];

  fin.open(fname);
  if (!fin) {
    std::cout << "Cannot open file: " << fname <<"\n";
    rc=-1;
    goto ende;
  }
  /* Hauptschleife */
  fin.getline(s,MAX_FIELD_LENGTH);
  while(!fin.eof()) {
    // ******** get entry type **********;
    end_entry=0;
    sp=strchr(s,'@');
    while(!sp && !fin.eof()) {
      s=sanc;
      fin.getline(s,MAX_FIELD_LENGTH);
      sp=strchr(s,'@');
    }
    if (sp) {
      add_citation();
      s=sp+1;
      pos=strcspn(s,"{");
      if (pos==0) {
	std::cout << "BibTeX file corrupted in reference: " << last << "\n";
	std::cout << "Entrytype must be followed by >> { << \n";
	goto ende;
      }
      strncpy(hs,s,pos); *(hs+pos)='\0';
      put_doctype(last,hs);
      s+=pos+1;
      // ******** get key *****************;
      pos=strcspn(s,",");
      if (pos==0) {
	std::cout << "BibTeX file corrupted in reference: " << last << "\n";
	std::cout << "Entrykey must be followed by >> , << \n";
	goto ende;
      }
      strncpy(hs,s,pos); hs[pos]='\0';
      put_id(last,atol(hs));
      s+=pos+1;
      // *******   Entry Fields  **********;
      while(!end_entry) {
	// ******   n e w   l i n e ?   ******;
	while(*s==' ') s++;
	while(*s=='\t') s++;
	if(!*s && !fin.eof()) {
          s=sanc;
	  fin.getline(s,MAX_FIELD_LENGTH);
	  while(*s==' ') s++;
	  while(*s=='\t') s++;
	}
	// ******   a b s t r a c t   ******;
	if(strncmp(s,"abstract",8)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"abstract\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "abstract must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("abstract",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "abstract field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("abstract",last,sp);
	}
	// ******   a d d r e s s   ******;
	else if(strncmp(s,"address",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"address\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "address must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("address",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "address field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("address",last,sp);
	}
	// ******   a n n o t e   ******;
	else if(strncmp(s,"annote",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"annote\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "annote must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("annote",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}') ;
	      else if(*s=='\"' && *(s-1)=='\\') ;
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "annote field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("annote",last,sp);
	}
	// ******   a u t h o r   **********;
	else if(strncmp(s,"author",6)==0) { 
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"author\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "author must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("author",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "author field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("author",last,sp);
	}
	// ******   b o o k t i t l e   **********;
	else if(strncmp(s,"booktitle",9)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"booktitle\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "booktitle must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("booktitle",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "booktitle field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("booktitle",last,sp);
	}
	// ******   C h a p t e r   ******;
	else if(strncmp(s,"chapter",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"chapter\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "chapter must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("chapter",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "chapter field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("chapter",last,sp);
	}
	// ******   C o m m e n t   ******;
	else if(strncmp(s,"comment",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"comment\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "comment must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("comment",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "comment field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("comment",last,sp);
	}
	// ******   C r o s s r e f   ******;
	else if(strncmp(s,"crossref",8)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"crossref\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "crossref must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("crossref",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "crossref field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("crossref",last,sp);
	}
        // ******   e d i t i o n   ******;
	else if(strncmp(s,"edition",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"edition\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "edition must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("edition",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "edition field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("edition",last,sp);
	}
        // ******   e d i t o r   ******;
	else if(strncmp(s,"editor",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"editor\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "editor must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("editor",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "editor field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("editor",last,sp);
	}
	// ******   h o w p u b l i s h e d   ******;
	else if(strncmp(s,"howpublished",12)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"howpublished\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "howpublished must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("howpublished",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "howpublished field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("howpublished",last,sp);
	}
	// ******   i n s t i t u t i o n   ******;
	else if(strncmp(s,"institution",11)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"institution\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "institution must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("institution",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "institution field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("institution",last,sp);
	}
	// ******   j o u r n a l   ******;
	else if(strncmp(s,"journal",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"journal\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "journal must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("journal",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',' ) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "journal field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("journal",last,sp);
	}
	// ******   k e y   ******;
	else if(strncmp(s,"key",3)==0 && strncmp(s,"keyword",7)!=0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"key\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "key must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("key",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "key field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("key",last,sp);
	}
	// ******   k e y w o r d   ******;
	else if(strncmp(s,"keyword",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"keyword\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "keyword must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("keyword",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "keyword field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("keyword",last,sp);
	}
	// ******   m o n t h   ******;
	else if(strncmp(s,"month",5)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"month\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "month must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("month",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "month field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("month",last,sp);
	}
	// ******   n o t e   ******;
	else if(strncmp(s,"note",4)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"note\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "note must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("note",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "note field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("note",last,sp);
	}
	// ******   n u m b e r   ******;
	else if(strncmp(s,"number",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"number\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "number must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("number",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "number field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("number",last,sp);
	}
	// ******   o r g a n i z a t i o n   ******;
	else if(strncmp(s,"organization",12)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"organization\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "organization must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("organization",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "organization field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("organization",last,sp);
	}
	// ******   p a g e s   ******;
	else if(strncmp(s,"pages",5)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"pages\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "pages must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("pages",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "pages field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("pages",last,sp);
	}
	// ******   p u b l i s h e r   ******;
	else if(strncmp(s,"publisher",9)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"publisher\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "publisher must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("publisher",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "publisher field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("publisher",last,sp);
	}
	// ******   r e p r i n t   ******;
	else if(strncmp(s,"reprint",7)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"reprint\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "reprint must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("reprint",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "reprint field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("reprint",last,sp);
	}
	// ******   s c h o o l   ******;
	else if(strncmp(s,"school",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"school\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "school must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("school",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "school field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("school",last,sp);
	}
	// ******   s e r i e s   ******;
	else if(strncmp(s,"series",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"series\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "series must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("series",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "series field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("series",last,sp);
	}
	// ******   T i t l e   ******;
	else if(strncmp(s,"title",5)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"title\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "title must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("title",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "title field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("title",last,sp);
	}
	// ******   t y p e   ******;
	else if(strncmp(s,"type",4)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"type\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "type must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("type",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "type field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("type",last,sp);
	}
	// ******   v o l u m e   ******;
	else if(strncmp(s,"volume",6)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"volume\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "volume must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("volume",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "volume field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("volume",last,sp);
	}
	// ******   year   ******;
	else if(strncmp(s,"year",4)==0) {
	  pos=strcspn(s,"=");
	  if (pos==0) {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "\"year\" must be followed by >> = << \n";
	    goto ende;
	  }
	  s+=pos+1;
	  while(*s==' ') s++;
	  encstart=*s;
	  if(encstart!='{' && encstart!='\"') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "year must start with either >> { << or >> \" << \n";
	    goto ende;
	  }
	  if (encstart=='{') encend='}'; else encend='\"';
	  enclev=1; s++; sp=s;
	  while(enclev!=0) {
	    if(!*s && !fin.eof()) {
	      add_field("year",last,sp);
	      s=sanc;
	      fin.getline(s,MAX_FIELD_LENGTH);
	      sp=s;
	    }
	    if(encstart=='{') {
	      if(*s=='{') enclev++;
	      if(*s=='}') enclev--;
	    }
	    else {
	      if(*s=='\"' && *(s-1)=='{' && *(s+1)=='}');
	      else if(*s=='\"' && *(s-1)=='\\');
	      else if(*s=='\"') enclev--;
	    }
	    s++;
	  }
	  pos=s-sp-1;
	  while(*s==' ') s++;
	  if(!*s || *s=='}') {end_entry=1;}
	  if(*s && *s!=',') {
	    std::cout << "BibTeX file corrupted in reference " << last << " ! \n";
	    std::cout << "year field must be terminated with >> , << \n";
	    goto ende;
	  }
	  *(sp+pos)='\0'; s++;
	  add_field("year",last,sp);
	}
	// *****   end of entry ***********;
	else if(*s=='}') {
	  end_entry=1;
	  s++;
	}
	else {
	  std::cout << "unknown field: " << s << " skip this line! \n";
	  s=sanc;
  	  fin.getline(s,MAX_FIELD_LENGTH);
	}
      }
    }
  }

  fin.close();

 ende:
  s=sanc;
  delete [] s;
  return(rc);
}

int BIBLIO::Write_BibTeX(char *fname) {
  int           rc=0;
  unsigned long n;
  std::ofstream      fout;

  fout.open(fname);
  if (!fout) {
    std::cout << "Cannot open file: " << fname <<"\n";
    rc=-1;
    goto ende;
  }

  for(n=0; n<last; n++) {
    fout << "@" << c[n]->doctype << "{" << c[n]->id << ",\n";
    if (c[n]->abstract && strcmp(c[n]->abstract,"")!=0)         fout <<
      "abstract   = \"" << c[n]->abstract   << "\",\n";
    if (c[n]->address && strcmp(c[n]->address,"")!=0)           fout <<
      "address    = \"" << c[n]->address    << "\",\n";
    if (c[n]->annote && strcmp(c[n]->annote,"")!=0)             fout <<
      "annote     = \""   << c[n]->annote   << "\",\n";
    if (c[n]->author && strcmp(c[n]->author,"")!=0)             fout  <<
      "author     = \"" << c[n]->author     << "\",\n";
    if (c[n]->booktitle && strcmp(c[n]->booktitle,"")!=0)       fout <<
      "booktitle  = \"" << c[n]->booktitle  << "\",\n";
    if (c[n]->chapter && strcmp(c[n]->chapter,"")!=0)           fout <<
      "chapter    = \"" << c[n]->chapter    << "\",\n";
    if (c[n]->comment && strcmp(c[n]->comment,"")!=0)           fout <<
      "comment    = \"" << c[n]->comment    << "\",\n";
    if (c[n]->crossref && strcmp(c[n]->crossref,"")!=0)         fout <<
      "crossref   = \"" << c[n]->crossref   << "\",\n";
    if (c[n]->edition && strcmp(c[n]->edition,"")!=0)           fout <<
      "edition    = \"" << c[n]->edition    << "\",\n";
    if (c[n]->editor && strcmp(c[n]->editor,"")!=0)             fout  <<
      "editor     = \"" << c[n]->editor     << "\",\n";
    if (c[n]->howpublished && strcmp(c[n]->howpublished,"")!=0) fout <<
      "howpublished  = \"" << c[n]->howpublished  << "\",\n";
    if (c[n]->institution && strcmp(c[n]->institution,"")!=0)   fout <<
      "institution= \"" << c[n]->institution << "\",\n";
    if (c[n]->journal && strcmp(c[n]->journal,"")!=0)           fout <<
      "journal    = \"" << c[n]->journal    << "\",\n";
    if (c[n]->key && strcmp(c[n]->key,"")!=0)                   fout <<
      "key        = \"" << c[n]->key        << "\",\n";
    if (c[n]->keyword && strcmp(c[n]->keyword,"")!=0)           fout <<
      "keyword    = \"" << c[n]->keyword    << "\",\n";
    if (c[n]->month && strcmp(c[n]->month,"")!=0)               fout <<
      "month      = \"" << c[n]->month      << "\",\n";
    if (c[n]->note && strcmp(c[n]->note,"")!=0)                 fout <<
      "note       = \"" << c[n]->note       << "\",\n";
    if (c[n]->number && strcmp(c[n]->number,"")!=0)             fout <<
      "number     = \"" << c[n]->number     << "\",\n";
    if (c[n]->organization && strcmp(c[n]->organization,"")!=0) fout <<
      "organization = \"" << c[n]->organization << "\",\n";
    if (c[n]->pages && strcmp(c[n]->pages,"")!=0)               fout <<
      "pages      = \"" << c[n]->pages      << "\",\n";
    if (c[n]->publisher && strcmp(c[n]->publisher,"")!=0)       fout <<
      "publisher  = \"" << c[n]->publisher  << "\",\n";
    if (c[n]->reprint && strcmp(c[n]->reprint,"")!=0)           fout <<
      "reprint    = \"" << c[n]->reprint    << "\",\n";
    if (c[n]->school && strcmp(c[n]->school,"")!=0)             fout <<
      "school     = \"" << c[n]->school     << "\",\n";
    if (c[n]->series && strcmp(c[n]->series,"")!=0)             fout <<
      "series     = \"" << c[n]->series     << "\",\n";
    if (c[n]->title && strcmp(c[n]->title,"")!=0)               fout <<
      "title      = \"" << c[n]->title      << "\",\n";
    if (c[n]->type && strcmp(c[n]->type,"")!=0)                 fout <<
      "type       = \"" << c[n]->type       << "\",\n";
    if (c[n]->volume && strcmp(c[n]->volume,"")!=0)             fout <<
      "volume     = \"" << c[n]->volume     << "\",\n";
    if (c[n]->year && strcmp(c[n]->year,"")!=0)                 fout <<
      "year       = \"" << c[n]->year       << "\",\n";
    fout << "}\n\n";
  }
  fout.close();
 ende:
  return(rc);
}

void BIBLIO::add_citation()
{
  if (!c) {
    last=1;
    c=new CITATION*[1];
    c[0]=new CITATION;
  }
  else {
    last++;
    c=(CITATION **)realloc(c,last*sizeof(CITATION*));
    c[last-1]=new CITATION;
  }
  if (!c) std::cout << "allocation failure *CITATION* ! \n";
  else {c[last-1]->doctype='\0';
	c[last-1]->abstract='\0';
	c[last-1]->address='\0';
	c[last-1]->annote='\0';
	c[last-1]->author='\0';
	c[last-1]->booktitle='\0';
	c[last-1]->chapter='\0';
	c[last-1]->comment='\0';
	c[last-1]->crossref='\0';
	c[last-1]->edition='\0';
	c[last-1]->editor='\0';
	c[last-1]->howpublished='\0';
	c[last-1]->institution='\0';
	c[last-1]->journal='\0';
	c[last-1]->key='\0';
	c[last-1]->keyword='\0';
	c[last-1]->month='\0';
	c[last-1]->note='\0';
	c[last-1]->number='\0';
	c[last-1]->organization='\0';
	c[last-1]->pages='\0';
	c[last-1]->publisher='\0';
	c[last-1]->reprint='\0';
	c[last-1]->school='\0';
	c[last-1]->series='\0';
	c[last-1]->title='\0';
	c[last-1]->type='\0';
	c[last-1]->volume='\0';
	c[last-1]->year='\0';
      }
}

void BIBLIO::del_citation(unsigned long nc) {
  unsigned long n;

  if(c && nc>=0 && nc<last) {
    if(c[nc]->doctype) delete[]c[nc]->doctype;
    if(c[nc]->abstract) delete[]c[nc]->abstract;
    if(c[nc]->address) delete[]c[nc]->address;
    if(c[nc]->annote) delete[]c[nc]->annote;
    if(c[nc]->author) delete[]c[nc]->author;
    if(c[nc]->booktitle) delete[]c[nc]->booktitle;
    if(c[nc]->chapter) delete[]c[nc]->chapter;
    if(c[nc]->comment) delete[]c[nc]->comment;
    if(c[nc]->crossref) delete[]c[nc]->crossref;
    if(c[nc]->edition) delete[]c[nc]->edition;
    if(c[nc]->editor) delete[]c[nc]->editor;
    if(c[nc]->howpublished) delete[]c[nc]->howpublished;
    if(c[nc]->institution) delete[]c[nc]->institution;
    if(c[nc]->journal) delete[]c[nc]->journal;
    if(c[nc]->key) delete[]c[nc]->key;
    if(c[nc]->keyword) delete[]c[nc]->keyword;
    if(c[nc]->month) delete[]c[nc]->month;
    if(c[nc]->note) delete[]c[nc]->note;
    if(c[nc]->number) delete[]c[nc]->number;
    if(c[nc]->organization) delete[]c[nc]->organization;
    if(c[nc]->pages) delete[]c[nc]->pages;
    if(c[nc]->publisher) delete[]c[nc]->publisher;
    if(c[nc]->reprint) delete[]c[nc]->reprint;
    if(c[nc]->school) delete[]c[nc]->school;
    if(c[nc]->series) delete[]c[nc]->series;
    if(c[nc]->title) delete[]c[nc]->title;
    if(c[nc]->type) delete[]c[nc]->type;
    if(c[nc]->volume) delete[]c[nc]->volume;
    if(c[nc]->year) delete[]c[nc]->year;
    delete c[nc];
    for(n=nc; n<last-1; n++) {
      c[n]=c[n+1];
    }
    last--;
    if(last>0)
      c=(CITATION **)realloc(c,last*sizeof(CITATION*));
    else {
      delete[]c;
      c='\0';
    }
  }
}









