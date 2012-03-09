/*******************************************************/
/* Linux  GNU     C++ Compiler                         */
/* Name : biblio.cc                                    */
/* Autor: Harald Stauss                                */
/*                                                     */
/*******************************************************/

#ifndef BIBTEXCLASS


#define BIBTEXCLASS
#define MAX_FIELD_LENGTH 3000

#include <stdlib.h>
/* #include <iostream.h> */
/* #include <fstream.h> */
/* #include <strstream.h> */
#include <string.h>
#include <ctype.h>

class BIBLIO {
  /* ...   private part   ... */
  /* const int MAX_FIELD_LENGTH=3000; */   /* max fieldlength for BibTeX fields */
  /* ...   functions used by public functions   ... */
  void put_id(unsigned long n, unsigned long refid);
  void put_doctype(unsigned long n, char *s);
  void add_keyword(unsigned long n, char *s);
  void put_abstract(unsigned long n, char *s);
  //void put_address(unsigned long n, char *s);
  //void put_annote(unsigned long n, char *s);
  void add_author(unsigned long n, char *s);
  void put_booktitle(unsigned long n, char *s);
  //void put_chapter(unsigned long n, char *s);
  void put_comment(unsigned long n, char *s);
  //void put_crossref(unsigned long n, char *s);
  //void put_edition(unsigned long n, char *s);
  void add_editor(unsigned long n, char *s);
  //void put_howpublished(unsigned long n, char *s);
  void put_institution(unsigned long n, char *s);
  void put_journal(unsigned long n, char *s, unsigned long i);
  //void put_key(unsigned long n, char *s);
  void put_month(unsigned long n, char *s);
  void put_note(unsigned long n, char *s);
  void put_number(unsigned long n, char *s);
  //void put_organization(unsigned long n, char *s);
  void put_pages(unsigned long n, char *s);
  void put_publisher(unsigned long n, char *s, unsigned long i);
  //void put_school(unsigned long n, char *s);
  //void put_series(unsigned long n, char *s);
  void put_title(unsigned long n, char *s);
  //void put_type(unsigned long n, char *s);
  void put_volume(unsigned long n, char *s);
  void put_year(unsigned long n, char *s);
  void add_field(char *t, unsigned long n, char *s);
  char *exright(int i,char *s); /* function that changes special characters to LaTeX format */

  /* ...   public part   ... */
public:
  struct CITATION {
    unsigned long id;/* made  unsigned long to simplify ref ID generation */
    char *doctype;   /* added to standard BibTeX fields */
    char *keyword;   /* added to standard BibTeX fields */
    char *abstract;  /* added to standard BibTeX fields */
    char *address;
    char *annote;
    char *author;
    char *booktitle;
    char *chapter;
    char *comment;   /* added to standard BibTeX fields */
    char *crossref;
    char *edition;
    char *editor;
    char *howpublished;
    char *institution;
    char *journal;
    char *key;
    char *month;
    char *note;
    char *number;
    char *organization;
    char *pages;
    char *publisher;
    char *reprint;    /* added to standard BibTeX fields */
    char *school;
    char *series;
    char *title;
    char *type;
    char *volume;
    char *year;
  };

  CITATION **c;
  unsigned long last;

  BIBLIO() {last=0; c='\0';}
  ~BIBLIO() {
    unsigned long n;
    if (c) for(n=last-1; n==0; n--) del_citation(n);
  }

  int Read_BibTeX(char *fname);
  int Read_MedlineOvid(char *fname, char *doct);
  int Read_MedlineSpirs(char *fname, char *doct);
  int Read_MedlinePubmed(char *fname, char *doct);
  int Read_Inspec(char *fname, char *doct);
  int Write_BibTeX(char *fname);

  void add_citation();
  void del_citation(unsigned long nc);

};

#endif
