/* ................................................ */
/* ...   Input filter for Medline Ovid format   ... */
/* ...   Last change 08.05.1997                 ... */
/* ................................................ */

#include "biblio.h"
#include <iostream>
#include <fstream>

int BIBLIO::Read_MedlineOvid(char *fname, char *doct) {
  std::ifstream      fin;
  int           rc=0;
  unsigned long n,i;
  char          s[256], *ts, item[3];
  short         limit_reached=0;


  fin.open(fname);
  if (!fin) {
    std::cout << "Cannot open file: " << fname <<"\n";
    rc=-1;
    goto ende;
  }

  /* Hauptschleife */
  while(!fin.eof()) {
    fin.getline(s,255);
    // ******* neue Referenz *******;
    if(isdigit(s[0])){
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
    };
    // ******* Author **************;
    if(s[0]=='A' && s[1]=='U') {
      strcpy(item,"AU");
      limit_reached=0;
      ts=exright(6,s);
      add_author(last,ts);
    }    // ******* Editor **************;
    if(s[0]=='E' && s[1]=='D') {
      strcpy(item,"ED");
      limit_reached=0;
      ts=exright(6,s);
      add_editor(last,ts);
    }
    // ******* Title ***************;
    if(s[0]=='T' && s[1]=='I') {
      strcpy(item,"TI");
      limit_reached=0;
      ts=exright(6,s);
      put_title(last,ts);
    }
    // ******* Booktitle ***************;
    if(s[0]=='B' && s[1]=='K') {
      strcpy(item,"BK");
      limit_reached=0;
      ts=exright(6,s);
      put_booktitle(last,ts);
    }
    // ******* Institution *********;
    if(s[0]=='I' && s[1]=='N') {
      strcpy(item,"IN");
      limit_reached=0;
      ts=exright(6,s);
      put_institution(last,ts);
    }
    // ******* Abstract **************;
    if(s[0]=='A' && s[1]=='B') {
      strcpy(item,"AB");
      limit_reached=0;
      ts=exright(6,s);
      if(strcmp(doct,"unpublished")==0)
	put_note(last,ts);
      else put_abstract(last,ts);
    }
    // ******* Keyword ***************;
    if(s[0]=='M' && s[1]=='H') {
      strcpy(item,"MH");
      limit_reached=0;
      ts=exright(6,s);
      add_keyword(last,ts);
    }
    // ******* Source ***************;
    if(s[0]=='S' && s[1]=='O') {
      strcpy(item,"SO");
      limit_reached=0;
      ts=exright(6,s);
      i=strcspn(ts,";");
      for(n=i;(isdigit(ts[n])==0 && n>0);n--);
      i=n;
      for(n=i;(isdigit(ts[n]) && n>0);n--) ;
      for(i=n;(ts[i]==' ' && i>0);i--) ;
      if (strcmp(c[last-1]->doctype,"article")==0)
	put_journal(last,ts,i+1);
      if (strcmp(c[last-1]->doctype,"book")==0 ||
	  strcmp(c[last-1]->doctype,"inbook")==0)
	put_publisher(last,ts,i+1);
      put_year(last,ts+n+1);
      if(strchr(ts,';')) put_volume(last,strchr(ts,';')+1);
      if(strchr(ts,':')) put_pages(last,strchr(ts,':')+1);
    }
    // ******* Space ***************;
    if(s[0]==' ' && s[1]==' ') {
      if(strcmp(item,"TI")==0) {
	ts=exright(6,s);
	n=strlen(c[last-1]->title)+1+strlen(ts)+1;
	c[last-1]->title=(char *)realloc(c[last-1]->title,n);
	strcat(c[last-1]->title,"\n");
	strcat(c[last-1]->title,ts);
      }
      if(strcmp(item,"BK")==0) {
	ts=exright(6,s);
	n=strlen(c[last-1]->booktitle)+1+strlen(ts)+1;
	c[last-1]->booktitle=(char *)realloc(c[last-1]->booktitle,n);
	strcat(c[last-1]->booktitle,"\n");
	strcat(c[last-1]->booktitle,ts);
      }
      if(strcmp(item,"IN")==0) {
	ts=exright(6,s);
	n=strlen(c[last-1]->institution)+1+strlen(ts)+1;
	c[last-1]->institution=(char *)realloc(c[last-1]->institution,n);
	strcat(c[last-1]->institution,"\n");
	strcat(c[last-1]->institution,ts);
      }
      if(strcmp(item,"AB")==0) {
	ts=exright(6,s);
	if (strcmp(doct,"unpublished")==0) {
	  n=strlen(c[last-1]->note)+1+strlen(ts)+1;
	  if (n<MAX_FIELD_LENGTH && !limit_reached) {
	    c[last-1]->note=(char *)realloc(c[last-1]->note,n);
	    strcat(c[last-1]->note,"\n");
	    strcat(c[last-1]->note,ts); }
	  else {limit_reached=1;}
	}
	else {  
	  n=strlen(c[last-1]->abstract)+1+strlen(ts)+1;
	  if (n<MAX_FIELD_LENGTH && !limit_reached) {
	    c[last-1]->abstract=(char *)realloc(c[last-1]->abstract,n);
	    strcat(c[last-1]->abstract,"\n");
	    strcat(c[last-1]->abstract,ts); }
	  else {limit_reached=1;}
	}
      }
    }
  }
  fin.close();
 ende:
  return(rc);
}

