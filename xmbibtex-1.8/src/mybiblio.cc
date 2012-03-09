#include "mybiblio.h"

  MYBIBLIO::MYBIBLIO() {
    d=new CITATION;
    d->doctype=new char[10];
    d->abstract=new char[10];
    d->address=new char[10];
    d->annote=new char[10];
    d->author=new char[10];
    d->booktitle=new char[10];
    d->chapter=new char[10];
    d->comment=new char[10];
    d->crossref=new char[10];
    d->edition=new char[10];
    d->editor=new char[10];
    d->howpublished=new char[10];
    d->institution=new char[10];
    d->journal=new char[10];
    d->key=new char[10];
    d->keyword=new char[10];
    d->month=new char[10];
    d->note=new char[10];
    d->number=new char[10];
    d->organization=new char[10];
    d->pages=new char[10];
    d->publisher=new char[10];
    d->reprint=new char[10];
    d->school=new char[10];
    d->series=new char[10];
    d->title=new char[10];
    d->type=new char[10];
    d->volume=new char[10];
    d->year=new char[10];
  }
  MYBIBLIO::~MYBIBLIO() {
    delete [] d->doctype;
    delete [] d->abstract;
    delete [] d->address;
    delete [] d->annote;
    delete [] d->author;
    delete [] d->booktitle;
    delete [] d->chapter;
    delete [] d->comment;
    delete [] d->crossref;
    delete [] d->edition;
    delete [] d->editor;
    delete [] d->howpublished;
    delete [] d->institution;
    delete [] d->journal;
    delete [] d->key;
    delete [] d->keyword;
    delete [] d->month;
    delete [] d->note;
    delete [] d->number;
    delete [] d->organization;
    delete [] d->pages;
    delete [] d->publisher;
    delete [] d->reprint;
    delete [] d->school;
    delete [] d->series;
    delete [] d->title;
    delete [] d->type;
    delete [] d->volume;
    delete [] d->year;
    delete d;
  }

void MYBIBLIO::ClearDummy() {
  d->doctype=(char *)realloc(d->doctype,1);
  strcpy(d->doctype,"");
  d->abstract=(char *)realloc(d->abstract,1);
  strcpy(d->abstract,"");
  d->address=(char *)realloc(d->address,1);
  strcpy(d->address,"");
  d->annote=(char *)realloc(d->annote,1);
  strcpy(d->annote,"");
  d->author=(char *)realloc(d->author,1);
  strcpy(d->author,"");
  d->booktitle=(char *)realloc(d->booktitle,1);
  strcpy(d->booktitle,"");
  d->chapter=(char *)realloc(d->chapter,1);
  strcpy(d->chapter,"");
  d->comment=(char *)realloc(d->comment,1);
  strcpy(d->comment,"");
  d->crossref=(char *)realloc(d->crossref,1);
  strcpy(d->crossref,"");
  d->edition=(char *)realloc(d->edition,1);
  strcpy(d->edition,"");
  d->editor=(char *)realloc(d->editor,1);
  strcpy(d->editor,"");
  d->howpublished=(char *)realloc(d->howpublished,1);
  strcpy(d->howpublished,"");
  d->institution=(char *)realloc(d->institution,1);
  strcpy(d->institution,"");
  d->journal=(char *)realloc(d->journal,1);
  strcpy(d->journal,"");
  d->key=(char *)realloc(d->key,1);
  strcpy(d->key,"");
  d->keyword=(char *)realloc(d->keyword,1);
  strcpy(d->keyword,"");
  d->month=(char *)realloc(d->month,1);
  strcpy(d->month,"");
  d->note=(char *)realloc(d->note,1);
  strcpy(d->note,"");
  d->number=(char *)realloc(d->number,1);
  strcpy(d->number,"");
  d->organization=(char *)realloc(d->organization,1);
  strcpy(d->organization,"");
  d->pages=(char *)realloc(d->pages,1);
  strcpy(d->pages,"");
  d->publisher=(char *)realloc(d->publisher,1);
  strcpy(d->publisher,"");
  d->reprint=(char *)realloc(d->reprint,1);
  strcpy(d->reprint,"");
  d->school=(char *)realloc(d->school,1);
  strcpy(d->school,"");
  d->series=(char *)realloc(d->series,1);
  strcpy(d->series,"");
  d->title=(char *)realloc(d->title,1);
  strcpy(d->title,"");
  d->type=(char *)realloc(d->type,1);
  strcpy(d->type,"");
  d->volume=(char *)realloc(d->volume,1);
  strcpy(d->volume,"");
  d->year=(char *)realloc(d->year,1);
  strcpy(d->year,"");
}


  void MYBIBLIO::ToDummy(unsigned long index) {
    d->id=c[index]->id;
    if(c[index]->doctype) {
      d->doctype=(char *)realloc(d->doctype,strlen(c[index]->doctype)+1);
      strcpy(d->doctype,c[index]->doctype);
    }
    else strcpy(d->doctype,"");
    if(c[index]->abstract) {
      d->abstract=(char *)realloc(d->abstract,strlen(c[index]->abstract)+1);
      strcpy(d->abstract,c[index]->abstract);
    }
    else strcpy(d->abstract,"");
    if(c[index]->address) {
      d->address=(char *)realloc(d->address,strlen(c[index]->address)+1);
      strcpy(d->address,c[index]->address);
    }
    else strcpy(d->address, "");
    if(c[index]->annote) {
      d->annote=(char *)realloc(d->annote,strlen(c[index]->annote)+1);
      strcpy(d->annote,c[index]->annote);
    }
    else strcpy(d->annote, "");
    if (c[index]->author) {
      d->author=(char *)realloc(d->author,strlen(c[index]->author)+1);
      strcpy(d->author,c[index]->author);
    }
    else strcpy(d->author, "");
    if (c[index]->booktitle) {
      d->booktitle=(char *)realloc(d->booktitle,strlen(c[index]->booktitle)+1);
      strcpy(d->booktitle,c[index]->booktitle);
    }
    else strcpy(d->booktitle, "");
    if (c[index]->chapter) {
      d->chapter=(char *)realloc(d->chapter,strlen(c[index]->chapter)+1);
      strcpy(d->chapter,c[index]->chapter);
    }
    else strcpy(d->chapter, "");
    if (c[index]->comment) {
      d->comment=(char *)realloc(d->comment,strlen(c[index]->comment)+1);
      strcpy(d->comment,c[index]->comment);
    }
    else strcpy(d->comment, "");
    if (c[index]->crossref) {
      d->crossref=(char *)realloc(d->crossref,strlen(c[index]->crossref)+1);
      strcpy(d->crossref,c[index]->crossref);
    }
    else strcpy(d->crossref, "");
    if (c[index]->edition) {
      d->edition=(char *)realloc(d->edition,strlen(c[index]->edition)+1);
      strcpy(d->edition,c[index]->edition);
    }
    else strcpy(d->edition, "");
    if (c[index]->editor) {
      d->editor=(char *)realloc(d->editor,strlen(c[index]->editor)+1);
      strcpy(d->editor,c[index]->editor);
    }
    else strcpy(d->editor, "");
    if (c[index]->howpublished) {
      d->howpublished=(char *)realloc(d->howpublished,strlen(c[index]->howpublished)+1);
      strcpy(d->howpublished,c[index]->howpublished);
    }
    else strcpy(d->howpublished, "");
    if (c[index]->institution) {
      d->institution=(char *)realloc(d->institution,strlen(c[index]->institution)+1);
      strcpy(d->institution,c[index]->institution);
    }
    else strcpy(d->institution, "");
    if (c[index]->journal) {
      d->journal=(char *)realloc(d->journal,strlen(c[index]->journal)+1);
      strcpy(d->journal,c[index]->journal);
    }
    else strcpy(d->journal, "");
    if (c[index]->key) {
      d->key=(char *)realloc(d->key,strlen(c[index]->key)+1);
      strcpy(d->key,c[index]->key);
    }
    else strcpy(d->key, "");
    if (c[index]->keyword) {
      d->keyword=(char *)realloc(d->keyword,strlen(c[index]->keyword)+1);
      strcpy(d->keyword,c[index]->keyword);
    }
    else strcpy(d->keyword, "");
    if (c[index]->month) {
      d->month=(char *)realloc(d->month,strlen(c[index]->month)+1);
      strcpy(d->month,c[index]->month);
    }
    else strcpy(d->month, "");
    if (c[index]->note) {
      d->note=(char *)realloc(d->note,strlen(c[index]->note)+1);
      strcpy(d->note,c[index]->note);
    }
    else strcpy(d->note, "");
    if (c[index]->number) {
      d->number=(char *)realloc(d->number,strlen(c[index]->number)+1);
      strcpy(d->number,c[index]->number);
    }
    else strcpy(d->number, "");
    if (c[index]->organization) {
      d->organization=(char *)realloc(d->organization,strlen(c[index]->organization)+1);
      strcpy(d->organization,c[index]->organization);
    }
    else strcpy(d->organization, "");
    if (c[index]->pages) {
      d->pages=(char *)realloc(d->pages,strlen(c[index]->pages)+1);
      strcpy(d->pages,c[index]->pages);
    }
    else strcpy(d->pages, "");
    if (c[index]->publisher) {
      d->publisher=(char *)realloc(d->publisher,strlen(c[index]->publisher)+1);
      strcpy(d->publisher,c[index]->publisher);
    }
    else strcpy(d->publisher, "");
    if (c[index]->reprint) {
      d->reprint=(char *)realloc(d->reprint,strlen(c[index]->reprint)+1);
      strcpy(d->reprint,c[index]->reprint);
    }
    else strcpy(d->reprint, "");
    if (c[index]->school) {
      d->school=(char *)realloc(d->school,strlen(c[index]->school)+1);
      strcpy(d->school,c[index]->school);
    }
    else strcpy(d->school, "");
    if (c[index]->series) {
      d->series=(char *)realloc(d->series,strlen(c[index]->series)+1);
      strcpy(d->series,c[index]->series);
    }
    else strcpy(d->series, "");
    if (c[index]->title) {
      d->title=(char *)realloc(d->title,strlen(c[index]->title)+1);
      strcpy(d->title,c[index]->title);
    }
    else strcpy(d->title, "");
    if (c[index]->type) {
      d->type=(char *)realloc(d->type,strlen(c[index]->type)+1);
      strcpy(d->type,c[index]->type);
    }
    else strcpy(d->type, "");
    if (c[index]->volume) {
      d->volume=(char *)realloc(d->volume,strlen(c[index]->volume)+1);
      strcpy(d->volume,c[index]->volume);
    }
    else strcpy(d->volume, "");
    if (c[index]->year) {
      d->year=(char *)realloc(d->year,strlen(c[index]->year)+1);
      strcpy(d->year,c[index]->year);
    }
    else strcpy(d->year, "");
  }

void MYBIBLIO::FromDummy(unsigned long index) {
  c[index]->id=d->id;

  if (strlen(d->doctype)>0) {
    if (c[index]->doctype) 
      c[index]->doctype=(char *)realloc(c[index]->doctype,strlen(d->doctype)+1);
    else
      c[index]->doctype=new char[strlen(d->doctype)+1];
    strcpy(c[index]->doctype,d->doctype);
  }
  else {
    if (c[index]->doctype) {
      delete[]c[index]->doctype;
      c[index]->doctype='\0';
    }
  }
  if (strlen(d->abstract)>0) {
    if (c[index]->abstract) 
      c[index]->abstract=(char *)realloc(c[index]->abstract,strlen(d->abstract)+1);
    else
      c[index]->abstract=new char[strlen(d->abstract)+1];
    strcpy(c[index]->abstract,d->abstract);
  }
  else {
    if (c[index]->abstract) {
      delete[]c[index]->abstract;
      c[index]->abstract='\0';
    }
  }
  if (strlen(d->address)>0) {
    if (c[index]->address) 
      c[index]->address=(char *)realloc(c[index]->address,strlen(d->address)+1);
    else
      c[index]->address=new char[strlen(d->address)+1];
    strcpy(c[index]->address,d->address);
  }
  else {
    if (c[index]->address) {
      delete[]c[index]->address;
      c[index]->address='\0';
    }
  }
  if (strlen(d->annote)>0) {
    if (c[index]->annote) 
      c[index]->annote=(char *)realloc(c[index]->annote,strlen(d->annote)+1);
    else
      c[index]->annote=new char[strlen(d->annote)+1];
    strcpy(c[index]->annote,d->annote);
  }
  else {
    if (c[index]->annote) {
      delete[]c[index]->annote;
      c[index]->annote='\0';
    }
  }
  if (strlen(d->author)>0) {
    if (c[index]->author) 
      c[index]->author=(char *)realloc(c[index]->author,strlen(d->author)+1);
    else
      c[index]->author=new char[strlen(d->author)+1];
    strcpy(c[index]->author,d->author);
  }
  else {
    if (c[index]->author) {
      delete[]c[index]->author;
      c[index]->author='\0';
    }
  }
  if (strlen(d->booktitle)>0) {
    if (c[index]->booktitle) 
      c[index]->booktitle=(char *)realloc(c[index]->booktitle,strlen(d->booktitle)+1);
    else
      c[index]->booktitle=new char[strlen(d->booktitle)+1];
    strcpy(c[index]->booktitle,d->booktitle);
  }
  else {
    if (c[index]->booktitle) {
      delete[]c[index]->booktitle;
      c[index]->booktitle='\0';
    }
  }
  if (strlen(d->chapter)>0) {
    if (c[index]->chapter) 
      c[index]->chapter=(char *)realloc(c[index]->chapter,strlen(d->chapter)+1);
    else
      c[index]->chapter=new char[strlen(d->chapter)+1];
    strcpy(c[index]->chapter,d->chapter);
  }
  else {
    if (c[index]->chapter) {
      delete[]c[index]->chapter;
      c[index]->chapter='\0';
    }
  }
  if (strlen(d->comment)>0) {
    if (c[index]->comment) 
      c[index]->comment=(char *)realloc(c[index]->comment,strlen(d->comment)+1);
    else
      c[index]->comment=new char[strlen(d->comment)+1];
    strcpy(c[index]->comment,d->comment);
  }
  else {
    if (c[index]->comment) {
      delete[]c[index]->comment;
      c[index]->comment='\0';
    }
  }
  if (strlen(d->crossref)>0) {
    if (c[index]->crossref) 
      c[index]->crossref=(char *)realloc(c[index]->crossref,strlen(d->crossref)+1);
    else
      c[index]->crossref=new char[strlen(d->crossref)+1];
    strcpy(c[index]->crossref,d->crossref);
  }
  else {
    if (c[index]->crossref) {
      delete[]c[index]->crossref;
      c[index]->crossref='\0';
    }
  }
  if (strlen(d->edition)>0) {
    if (c[index]->edition) 
      c[index]->edition=(char *)realloc(c[index]->edition,strlen(d->edition)+1);
    else
      c[index]->edition=new char[strlen(d->edition)+1];
    strcpy(c[index]->edition,d->edition);
  }
  else {
    if (c[index]->edition) {
      delete[]c[index]->edition;
      c[index]->edition='\0';
    }
  }
  if (strlen(d->editor)>0) {
    if (c[index]->editor) 
      c[index]->editor=(char *)realloc(c[index]->editor,strlen(d->editor)+1);
    else
      c[index]->editor=new char[strlen(d->editor)+1];
    strcpy(c[index]->editor,d->editor);
  }
  else {
    if (c[index]->editor) {
      delete[]c[index]->editor;
      c[index]->editor='\0';
    }
  }
  if (strlen(d->howpublished)>0) {
    if (c[index]->howpublished) 
      c[index]->howpublished=(char *)realloc(c[index]->howpublished,strlen(d->howpublished)+1);
    else
      c[index]->howpublished=new char[strlen(d->howpublished)+1];
    strcpy(c[index]->howpublished,d->howpublished);
  }
  else {
    if (c[index]->howpublished) {
      delete[]c[index]->howpublished;
      c[index]->howpublished='\0';
    }
  }
  if (strlen(d->institution)>0) {
    if (c[index]->institution) 
      c[index]->institution=(char *)realloc(c[index]->institution,strlen(d->institution)+1);
    else
      c[index]->institution=new char[strlen(d->institution)+1];
    strcpy(c[index]->institution,d->institution);
  }
  else {
    if (c[index]->institution) {
      delete[]c[index]->institution;
      c[index]->institution='\0';
    }
  }
  if (strlen(d->journal)>0) {
    if (c[index]->journal) 
      c[index]->journal=(char *)realloc(c[index]->journal,strlen(d->journal)+1);
    else
      c[index]->journal=new char[strlen(d->journal)+1];
    strcpy(c[index]->journal,d->journal);
  }
  else {
    if (c[index]->journal) {
      delete[]c[index]->journal;
      c[index]->journal='\0';
    }
  }
  if (strlen(d->key)>0) {
    if (c[index]->key) 
      c[index]->key=(char *)realloc(c[index]->key,strlen(d->key)+1);
    else
      c[index]->key=new char[strlen(d->key)+1];
    strcpy(c[index]->key,d->key);
  }
  else {
    if (c[index]->key) {
      delete[]c[index]->key;
      c[index]->key='\0';
    }
  }
  if (strlen(d->keyword)>0) {
    if (c[index]->keyword) 
      c[index]->keyword=(char *)realloc(c[index]->keyword,strlen(d->keyword)+1);
    else
      c[index]->keyword=new char[strlen(d->keyword)+1];
    strcpy(c[index]->keyword,d->keyword);
  }
  else {
    if (c[index]->keyword) {
      delete[]c[index]->keyword;
      c[index]->keyword='\0';
    }
  }
  if (strlen(d->month)>0) {
    if (c[index]->month) 
      c[index]->month=(char *)realloc(c[index]->month,strlen(d->month)+1);
    else
      c[index]->month=new char[strlen(d->month)+1];
    strcpy(c[index]->month,d->month);
  }
  else {
    if (c[index]->month) {
      delete[]c[index]->month;
      c[index]->month='\0';
    }
  }
  if (strlen(d->note)>0) {
    if (c[index]->note) 
      c[index]->note=(char *)realloc(c[index]->note,strlen(d->note)+1);
    else
      c[index]->note=new char[strlen(d->note)+1];
    strcpy(c[index]->note,d->note);
  }
  else {
    if (c[index]->note) {
      delete[]c[index]->note;
      c[index]->note='\0';
    }
  }
  if (strlen(d->number)>0) {
    if (c[index]->number) 
      c[index]->number=(char *)realloc(c[index]->number,strlen(d->number)+1);
    else
      c[index]->number=new char[strlen(d->number)+1];
    strcpy(c[index]->number,d->number);
  }
  else {
    if (c[index]->number) {
      delete[]c[index]->number;
      c[index]->number='\0';
    }
  }
  if (strlen(d->organization)>0) {
    if (c[index]->organization) 
      c[index]->organization=(char *)realloc(c[index]->organization,strlen(d->organization)+1);
    else
      c[index]->organization=new char[strlen(d->organization)+1];
    strcpy(c[index]->organization,d->organization);
  }
  else {
    if (c[index]->organization) {
      delete[]c[index]->organization;
      c[index]->organization='\0';
    }
  }
  if (strlen(d->pages)>0) {
    if (c[index]->pages) 
      c[index]->pages=(char *)realloc(c[index]->pages,strlen(d->pages)+1);
    else
      c[index]->pages=new char[strlen(d->pages)+1];
    strcpy(c[index]->pages,d->pages);
  }
  else {
    if (c[index]->pages) {
      delete[]c[index]->pages;
      c[index]->pages='\0';
    }
  }
  if (strlen(d->publisher)>0) {
    if (c[index]->publisher) 
      c[index]->publisher=(char *)realloc(c[index]->publisher,strlen(d->publisher)+1);
    else
      c[index]->publisher=new char[strlen(d->publisher)+1];
    strcpy(c[index]->publisher,d->publisher);
  }
  else {
    if (c[index]->publisher) {
      delete[]c[index]->publisher;
      c[index]->publisher='\0';
    }
  }
  if (strlen(d->reprint)>0) {
    if (c[index]->reprint) 
      c[index]->reprint=(char *)realloc(c[index]->reprint,strlen(d->reprint)+1);
    else
      c[index]->reprint=new char[strlen(d->reprint)+1];
    strcpy(c[index]->reprint,d->reprint);
  }
  else {
    if (c[index]->reprint) {
      delete[]c[index]->reprint;
      c[index]->reprint='\0';
    }
  }
  if (strlen(d->school)>0) {
    if (c[index]->school) 
      c[index]->school=(char *)realloc(c[index]->school,strlen(d->school)+1);
    else
      c[index]->school=new char[strlen(d->school)+1];
    strcpy(c[index]->school,d->school);
  }
  else {
    if (c[index]->school) {
      delete[]c[index]->school;
      c[index]->school='\0';
    }
  }
  if (strlen(d->series)>0) {
    if (c[index]->series) 
      c[index]->series=(char *)realloc(c[index]->series,strlen(d->series)+1);
    else
      c[index]->series=new char[strlen(d->series)+1];
    strcpy(c[index]->series,d->series);
  }
  else {
    if (c[index]->series) {
      delete[]c[index]->series;
      c[index]->series='\0';
    }
  }
  if (strlen(d->title)>0) {
    if (c[index]->title) 
      c[index]->title=(char *)realloc(c[index]->title,strlen(d->title)+1);
    else
      c[index]->title=new char[strlen(d->title)+1];
    strcpy(c[index]->title,d->title);
  }
  else {
    if (c[index]->title) {
      delete[]c[index]->title;
      c[index]->title='\0';
    }
  }
  if (strlen(d->type)>0) {
    if (c[index]->type) 
      c[index]->type=(char *)realloc(c[index]->type,strlen(d->type)+1);
    else
      c[index]->type=new char[strlen(d->type)+1];
    strcpy(c[index]->type,d->type);
  }
  else {
    if (c[index]->type) {
      delete[]c[index]->type;
      c[index]->type='\0';
    }
  }
  if (strlen(d->volume)>0) {
    if (c[index]->volume) 
      c[index]->volume=(char *)realloc(c[index]->volume,strlen(d->volume)+1);
    else
      c[index]->volume=new char[strlen(d->volume)+1];
    strcpy(c[index]->volume,d->volume);
  }
  else {
    if (c[index]->volume) {
      delete[]c[index]->volume;
      c[index]->volume='\0';
    }
  }
  if (strlen(d->year)>0) {
    if (c[index]->year) 
      c[index]->year=(char *)realloc(c[index]->year,strlen(d->year)+1);
    else
      c[index]->year=new char[strlen(d->year)+1];
    strcpy(c[index]->year,d->year);
  }
  else {
    if (c[index]->year) {
      delete[]c[index]->year;
      c[index]->year='\0';
    }
  }

}

  short MYBIBLIO::DocTypeN(unsigned long index) {
    short i;
    if (strcmp(c[index]->doctype, "article")==0) i=0;
    else if (strcmp(c[index]->doctype, "book")==0) i=1;
    else if (strcmp(c[index]->doctype, "booklet")==0) i=2;
    else if (strcmp(c[index]->doctype, "inbook")==0) i=3;
    else if (strcmp(c[index]->doctype, "incollection")==0) i=4;
    else if (strcmp(c[index]->doctype, "inproceedings")==0) i=5;
    else if (strcmp(c[index]->doctype, "manual")==0) i=6;
    else if (strcmp(c[index]->doctype, "mastersthesis")==0) i=7;
    else if (strcmp(c[index]->doctype, "misc")==0) i=8;
    else if (strcmp(c[index]->doctype, "phdthesis")==0) i=9;
    else if (strcmp(c[index]->doctype, "proceedings")==0) i=10;
    else if (strcmp(c[index]->doctype, "techreport")==0) i=11;
    else if (strcmp(c[index]->doctype, "unpublished")==0) i=12;
    else i=-1;
    return(i);
  }



