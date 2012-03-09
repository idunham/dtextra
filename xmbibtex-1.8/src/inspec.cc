/* .......................................... */
/* ...   Input filter for Inspec format   ... */
/* ...   Last change 08.05.1997           ... */
/* ...   Provided by Ralf Mrowka          ... */
/* .......................................... */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "biblio.h"

int BIBLIO::Read_Inspec(char *fname, char *doct) {
  std::ifstream      fin;
  int           rc=0, size,flag,distance,string_flag=0;
  unsigned long n,i;
  char          s[6000],s_temp1[6000],s_temp2[6000], *ts, s_new[6000],sss[6000];
  char          s_tempo1[6000],s_tempo2[6000],s_tempo3[6000],s_tempo4[6000];
  char          *tk, *tkstart, *found, auname[256];
  char          asterix[2000];
  int		ralf=0,i_g,i_au,i_ad,i_l;

  fin.open(fname);
  if (!fin) {
    std::cout << "Cannot open file: " << fname <<"\n";
    rc=-1;
    goto ende;
  }

  /* Hauptschleife */
  while(!fin.eof()) {
    if(string_flag==0)
      fin.getline(s,6000-1);
    else
      strcpy(s,s_new);
    fin.getline(s_new,6000-1);
    string_flag=1;
    while(s_new[0]==' '){
      strcat(s,s_new+4); 
      fin.getline(s_new,6000-1);
    }
    
    sprintf(asterix,"  ***");
    strcpy(sss,s);
    while(strstr(sss,asterix)!=NULL){
      ts=strstr(sss,asterix);
      strcpy(s_tempo3,ts);
      strcpy(s_tempo2,s_tempo3+strlen(asterix));
      strcpy(s_tempo1,sss);
      s_tempo1[strlen(sss)-strlen(s_tempo2)-strlen(asterix)]='\0';
      strcpy(sss,s_tempo1);
      strcat(sss,s_tempo2);
    }
    strcpy(s,sss);

    sprintf(asterix,"***  ");
    strcpy(sss,s);
    while(strstr(sss,asterix)!=NULL){
      ts=strstr(sss,asterix);
      strcpy(s_tempo3,ts);
      strcpy(s_tempo2,s_tempo3+strlen(asterix));
      strcpy(s_tempo1,sss);
      s_tempo1[strlen(sss)-strlen(s_tempo2)-strlen(asterix)]='\0';
      strcpy(sss,s_tempo1);
      strcat(sss,s_tempo2);
    }
    strcpy(s,sss);

    sprintf(asterix,"***");
    strcpy(sss,s);
    while(strstr(sss,asterix)!=NULL){
      ts=strstr(sss,asterix);
      strcpy(s_tempo3,ts);
      strcpy(s_tempo2,s_tempo3+strlen(asterix));
      strcpy(s_tempo1,sss);
      s_tempo1[strlen(sss)-strlen(s_tempo2)-strlen(asterix)]='\0';
      strcpy(sss,s_tempo1);
      strcat(sss,s_tempo2);
    }
    strcpy(s,sss);

    // ******* neue Referenz *******;
    if(s[0]=='A' && s[1]=='N' ) {
      ralf++;
      //cout << ralf << "hi\n" ;
      add_citation();
      put_id(last,last);
      if (strcmp(doct,"abstract")==0) {
	put_doctype(last,"article");
	put_note(last,"[Abstract]");
      }
      else if(strcmp(doct,"inpress")==0) {
	put_doctype(last,"article");
	put_note(last,"[in press]");
      }
      else put_doctype(last,doct);
    }
    // ******* Author **************;
    if(s[0]=='A' && s[1]=='U') {
      ts=exright(5,s);
      tk=ts;
      i_g=0;
      i_au=0;
      i_ad=0;
      flag=0;
      while ((ts[i_g] != '\0')&&(ts[i_g] != '\n')){
	if(ts[i_g]=='(')flag=1;
	if(ts[i_g]==')')flag=0;
	if(flag==0){
	  s_temp1[i_au]=ts[i_g];
	  s_temp1[i_au+1]='\0';
	  i_au++;
	}

	if(ts[i_g+1]==';'){
	  add_author(last,s_temp1);
	  //cout << s_temp1 << "\n";
	  i_au=0;
	  s_temp1[0]='\0';
                       
	}
	if(flag==1){
	  s_temp2[i_ad]=ts[i_g];
	  s_temp2[i_ad+1]='\0';
	  i_ad++;
	}
	i_g++;
      } 
      if(s_temp1[0]!='\0'){
	add_author(last,s_temp1);
      }
      add_field("annote",last,s_temp2); 

    }
    // ******* Title ***************;
    if(s[0]=='T' && s[1]=='I') {
      ts=exright(5,s);
      put_title(last,ts);
    }
    // ******* Abstract **************;
    if(s[0]=='A' && s[1]=='B') {
      ts=exright(5,s);
      if(strcmp(doct,"unpublished")==0)
      	put_note(last,ts);
      else put_abstract(last,ts);
    }
    // ******* Keyword ***************;
    if((s[0]=='M' && s[1]=='E' && s[2]=='S' && s[3]=='H') ||
       (s[0]=='T' && s[1]=='G') ||
       (s[0]=='*' && s[1]=='L' && s[2]=='H' && s[3]=='M') ||
       (s[0]=='*' && s[1]=='L' && s[2]=='H' && s[3]=='C') || 
       (s[0]=='M' && s[1]=='I' && s[2]=='M' && s[3]=='E') ||
       (s[0]=='M' && s[1]=='J' && s[2]=='M' && s[3]=='E') || 
       (s[0]=='T' && s[1]=='O') ||
       (s[0]=='C' && s[1]=='P') ||
       (s[0]=='L' && s[1]=='A') ||
       (s[0]=='I' && s[1]=='S' && s[2]=='S' && s[3]=='N') ||
       (s[0]=='P' && s[1]=='Y') ||
       (s[0]=='A' && s[1]=='N') ||
       (s[0]=='P' && s[1]=='T') ||
       (s[0]=='C' && s[1]=='N') ||
       (s[0]=='R' && s[1]=='N') ||
       (s[0]=='G' && s[1]=='S') ||
       (s[0]=='P' && s[1]=='S') ||
       (s[0]=='S' && s[1]=='I') ||
       (s[0]=='S' && s[1]=='B') ||
       (s[0]=='D' && s[1]=='T') ||
       (s[0]=='T' && s[1]=='C') ||
       (s[0]=='C' && s[1]=='Y') ||
       (s[0]=='U' && s[1]=='O') ||
       (s[0]=='U' && s[1]=='D') ||

       (s[0]=='N' && s[1]=='M')) {
      ts=exright(5,s);
      tk=ts; 
      while (tk) {
	tkstart=strchr(tk,';');
	if (tkstart) {
	  tkstart[0]='\0'; tkstart+=2;
	}
	strcpy(auname, tk);
	add_keyword(last, auname);
	tk=tkstart;
      }
    }
    // ******* Source ***************;
    if(s[0]=='S' && s[1]=='O') {
      //cout << "so...";
      ts=exright(5,s);
      tk=ts;
      
      i_g=0;
      while ((tk[i_g] != '\0')&&(tk[i_g] != '\n') &&(tk[i_g] != '(')){
	i_g++;
      }
      put_journal(last,ts,i_g);
     

      //****************************************; 
      i_l=0;flag=0;
      while ((tk[i_l] != '\0')&&(tk[i_l] != '\n')&&(flag==0)){
	if((tk[i_l] == 'v')&&(tk[i_l+1] == 'o')&&(tk[i_l+2] == 'l'))
	  { 
	    flag=1;
	    i_l+=4;
	    i_au=0;
	    while ((tk[i_l] != '\0') &&(tk[i_l] != '\n') &&(tk[i_l] != '.') &&(tk[i_l] != ',')){
	      s_temp2[i_au]=tk[i_l];
	      s_temp2[i_au+1]='\0';
	      i_au++;
	      i_l++;
	    }
	    if(s_temp2[0]!='\0'){
	      //cout << "vol..." << s_temp2 << "\n" ;
	      put_volume(last,s_temp2);
	    }

	  }
	i_l++;
      }
      //****************************************;

      //****************************************;
      i_l=0;flag=0;
      while ((tk[i_l] != '\0')&&(tk[i_l] != '\n')&&(flag==0)){
	if((tk[i_l] == ' ')&&(tk[i_l+1] == 'p')&&(tk[i_l+2] == '.'))
	  {
	    i_l+=3;
	    flag=1;
	    i_au=0;
	    while ((tk[i_l] != '\0') &&(tk[i_l] != '\n') &&(tk[i_l] != '.') &&(tk[i_l] != ',')){
	      s_temp2[i_au]=tk[i_l];
	      s_temp2[i_au+1]='\0';
	      i_au++;
	      i_l++;
	    }
	    if(s_temp2[0]!='\0'){
	      //cout << "page..." << s_temp2 << "\n" ;
	      put_pages(last,s_temp2);
	    }

	  }
	i_l++;
      }
      //****************************************;
      //****************************************;
      i_l=0;flag=0;
      while ((tk[i_l] != '\0')&&(tk[i_l] != '\n')&&(flag==0)){
	if((tk[i_l] == ')')&&(tk[i_l+1] == ' '))
	  {
	    i_l-=4;
	    flag=1;
	    i_au=0;
	    while ((tk[i_l] != '\0') &&(tk[i_l] != '\n') &&(tk[i_l] != ')') &&(tk[i_l] != ',')){
	      s_temp2[i_au]=tk[i_l];
	      s_temp2[i_au+1]='\0';
	      i_au++;
	      i_l++;
	    }
	    if(s_temp2[0]!='\0'){
	      //cout << "year..." << s_temp2 << "\n" ;
	      put_year(last,s_temp2);
	    }

	  }
	i_l++;
      }
      //****************************************;
      //****************************************;
      i_l=0;flag=0;
      while ((tk[i_l] != '\0')&&(tk[i_l] != '\n')&&(flag==0)){
	if((tk[i_l] == '('))
	  {
	    i_l=i_l+1;
	    flag=1;
	    i_au=0;
	    while ((tk[i_l] != '\0') &&(tk[i_l] != '\n') &&(tk[i_l]!=' ')&&(tk[i_l+4]!=')') ){
	      s_temp2[i_au]=tk[i_l];
	      s_temp2[i_au+1]='\0';
	      i_au++;
	      i_l++;
	    }
	    if(s_temp2[0]!='\0')if(s_temp2[0]!='1'){
	      //cout << "month..." << s_temp2 << "\n" ;
	      put_month(last,s_temp2);
	    }

	  }
	i_l++;
      }
      //****************************************;
      //****************************************;
      i_l=0;flag=0;
      while ((tk[i_l] != '\0')&&(tk[i_l] != '\n')&&(flag==0)){
	if((tk[i_l] == 'n')&&(tk[i_l+1] == 'o')&&(tk[i_l+2] == '.'))
	  {
	    i_l+=3;
	    i_au=0;
	    flag=1;
	    while ((tk[i_l] != '\0') &&(tk[i_l] != '\n') &&(tk[i_l] != '.') &&(tk[i_l] != ',')){
	      s_temp2[i_au]=tk[i_l];
	      s_temp2[i_au+1]='\0';
	      i_au++;
	      i_l++;
	    }
	    if(s_temp2[0]!='\0'){
	      //cout << "number..." << s_temp2 << "\n" ;
	      put_number(last,s_temp2);
	    }

	  }
	i_l++;
      }
      //**********************************;
    }
    // ******* Comment ***************;
    if(s[0]=='C' && s[1]=='M') {
      ts=exright(7,s);
      add_field("comment",last,ts);
    }
  }
  fin.close();
ende:
  return(rc);
}
