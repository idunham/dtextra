/* ................................................. */
/* ...   Input filter for Medline Spirs format   ... */
/* ...   Last change 08.05.1997                  ... */
/* ................................................. */

#include "biblio.h"
#include <iostream>
#include <fstream>

int BIBLIO::Read_MedlineSpirs(char *fname, char *doct) {
  std::ifstream      fin;
  int           rc=0, size;
  unsigned long n,i;
  char          s[6000], *ts;
  char          *tk, *tkstart, *found, auname[256];

  fin.open(fname);
  if (!fin) {
    std::cout << "Cannot open file: " << fname <<"\n";
    rc=-1;
    goto ende;
  }

  /* Hauptschleife */
  while(!fin.eof()) {
    fin.getline(s,6000);
    // ******* neue Referenz *******;
    if(s[0]=='R' && s[1]=='e' && s[2]=='c') {
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
      ts=exright(7,s);
      tk=ts; 
      while (tk) {
	tkstart=strchr(tk,';');
	if (tkstart) {
	  tkstart[0]='\0'; tkstart++;
	}
	found=strrchr(tk,'-');
	found[0]=',';
	strcpy(auname, tk);
	add_author(last, auname);
	tk=tkstart;
      }
    }
    // ******* Title ***************;
    if(s[0]=='T' && s[1]=='I') {
      ts=exright(7,s);
      put_title(last,ts);
    }
    // ******* Abstract **************;
    if(s[0]=='A' && s[1]=='B') {
      ts=exright(7,s);
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
       (s[0]=='U' && s[1]=='O') ||
       (s[0]=='U' && s[1]=='D') ||
       (s[0]=='N' && s[1]=='M')) {
      ts=exright(7,s);
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
      ts=exright(7,s);
      i=strcspn(ts,";");
      // Tag der Publikation;
      for(n=i-1; (isdigit(ts[n]) && n>0); n--);
      if ((i-n)<=3) i=n;
      for(n=i;(isdigit(ts[n])==0 && n>0);n--); i=n;
      for(n=i;(isdigit(ts[n]) && n>0);n--) ;
      for(i=n;(ts[i]==' ' && i>0);i--) ;
      if (strcmp(c[last-1]->doctype,"article")==0)
	put_journal(last,ts,i);
      if (strcmp(c[last-1]->doctype,"book")==0 ||
	  strcmp(c[last-1]->doctype,"inbook")==0)
	put_publisher(last,ts,i);
      put_year(last,ts+n+1);
      if(strchr(ts,';')) put_volume(last,strchr(ts,';')+2);
      if(strchr(ts,':')) put_pages(last,strchr(ts,':')+2);
    }
    // ******* Comment ***************;
    if(s[0]=='C' && s[1]=='M') {
      ts=exright(7,s);
      add_field("comment",last,ts);
    }
    // ******* Annote ***************;
    if(s[0]=='A' && s[1]=='D') {
      ts=exright(7,s);
      add_field("annote",last,ts);
    }
  }
  fin.close();
 ende:
  return(rc);
}

