/* Reference Manager             */
/* Search in all fields Module   */
/* Last change 03.05.1997        */

#include <Xm/List.h>
#include <X11/cursorfont.h>
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include "globals.h"

void searchall_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
  unsigned long n, nk;
  short         hinzu, cis;
  unsigned long *gefunden;
  XmStringTable str_list;
  char          *ref;
  Cursor        uhr, pfeil;

  extern char *mystrstr(const char *s1, const char *s2, short c);
  extern void set_message(unsigned long);

  uhr  =XCreateFontCursor(XtDisplay(XtParent(widget)),XC_watch);
  pfeil=XCreateFontCursor(XtDisplay(XtParent(widget)), XC_arrow);
  XDefineCursor(XtDisplay(XtParent(widget)),XtWindow(XtParent(widget)),uhr);
  XFlush(XtDisplay(XtParent(widget)));

  gefunden   = new unsigned long[b.last+1];
  gefunden[0]= 0;
  ref        = new char[100];
  //cis=(short)client_data;
  assert(((intptr_t)client_data >> 16) == 0);
  cis=(short)((intptr_t)client_data & 0xFFFF);
  switch((int)b.d->id) {
  case 1:  // .... and;
    for (n=1; n<=key[0]; n++) {
      hinzu=0;
      if (b.c[key[n]]->doctype) 
	if (mystrstr(b.c[key[n]]->doctype,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->keyword) 
	if (mystrstr(b.c[key[n]]->keyword,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->abstract) 
	if (mystrstr(b.c[key[n]]->abstract,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->address) 
	if (mystrstr(b.c[key[n]]->address,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->annote) 
	if (mystrstr(b.c[key[n]]->annote,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->author) 
	if (mystrstr(b.c[key[n]]->author,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->booktitle) 
	if (mystrstr(b.c[key[n]]->booktitle,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->chapter) 
	if (mystrstr(b.c[key[n]]->chapter,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->comment) 
	if (mystrstr(b.c[key[n]]->comment,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->crossref) 
	if (mystrstr(b.c[key[n]]->crossref,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->edition) 
	if (mystrstr(b.c[key[n]]->edition,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->editor) 
	if (mystrstr(b.c[key[n]]->editor,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->howpublished) 
	if (mystrstr(b.c[key[n]]->howpublished,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->institution) 
	if (mystrstr(b.c[key[n]]->institution,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->journal) 
	if (mystrstr(b.c[key[n]]->journal,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->key) 
	if (mystrstr(b.c[key[n]]->key,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->month) 
	if (mystrstr(b.c[key[n]]->month,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->note) 
	if (mystrstr(b.c[key[n]]->note,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->number) 
	if (mystrstr(b.c[key[n]]->number,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->organization) 
	if (mystrstr(b.c[key[n]]->organization,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->pages) 
	if (mystrstr(b.c[key[n]]->pages,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->publisher) 
	if (mystrstr(b.c[key[n]]->publisher,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->reprint) 
	if (mystrstr(b.c[key[n]]->reprint,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->school) 
	if (mystrstr(b.c[key[n]]->school,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->series) 
	if (mystrstr(b.c[key[n]]->series,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->title) 
	if (mystrstr(b.c[key[n]]->title,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->type) 
	if (mystrstr(b.c[key[n]]->type,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->volume) 
	if (mystrstr(b.c[key[n]]->volume,b.d->keyword, cis)) hinzu=1;
      if (b.c[key[n]]->year) 
	if (mystrstr(b.c[key[n]]->year,b.d->keyword, cis)) hinzu=1;
      if (hinzu) {
	gefunden[0]++;
	gefunden[gefunden[0]]=key[n];
      }
    } /* endfor */
    // ........................................ Liste neu aufbauen;
    str_list=(XmStringTable) XtMalloc(gefunden[0]*sizeof(XmString));
    key=(unsigned long *)realloc(key,(gefunden[0]+1)*sizeof(unsigned long));
    key[0]=gefunden[0];
    for(n=1; n<=gefunden[0]; n++) {
      key[n]=gefunden[n];
      sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	      b.c[gefunden[n]]->id,
	      b.c[gefunden[n]]->author,
	      b.c[gefunden[n]]->title,
	      b.c[gefunden[n]]->journal,
	      b.c[gefunden[n]]->year);
      str_list[n-1]=XmStringCreateLocalized(ref);
    }
    XmListDeleteAllItems(liste);
    XtVaSetValues(liste,
		  XmNitems, str_list,
		  XmNitemCount, gefunden[0],
		  NULL);
    set_message(gefunden[0]);
    XtFree((char *)str_list);
    break;  

  case 2:  // .... or;
    for (n=0; n<b.last; n++) {
      // ...................................... already selected ? ;
      hinzu=0;
      for (nk=1; nk<=key[0]; nk++) if (n==key[nk]) hinzu=1;
      // ...................................... if not than check;
      if(!hinzu) {
	hinzu=0;
	if (b.c[n]->doctype) 
	  if (mystrstr(b.c[n]->doctype,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->keyword) 
	  if (mystrstr(b.c[n]->keyword,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->abstract) 
	  if (mystrstr(b.c[n]->abstract,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->address) 
	  if (mystrstr(b.c[n]->address,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->annote) 
	  if (mystrstr(b.c[n]->annote,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->author) 
	  if (mystrstr(b.c[n]->author,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->booktitle) 
	  if (mystrstr(b.c[n]->booktitle,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->chapter) 
	  if (mystrstr(b.c[n]->chapter,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->comment) 
	  if (mystrstr(b.c[n]->comment,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->crossref) 
	  if (mystrstr(b.c[n]->crossref,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->edition) 
	  if (mystrstr(b.c[n]->edition,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->editor) 
	  if (mystrstr(b.c[n]->editor,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->howpublished) 
	  if (mystrstr(b.c[n]->howpublished,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->institution) 
	  if (mystrstr(b.c[n]->institution,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->journal) 
	  if (mystrstr(b.c[n]->journal,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->key) 
	  if (mystrstr(b.c[n]->key,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->month) 
	  if (mystrstr(b.c[n]->month,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->note) 
	  if (mystrstr(b.c[n]->note,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->number) 
	  if (mystrstr(b.c[n]->number,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->organization) 
	  if (mystrstr(b.c[n]->organization,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->pages) 
	  if (mystrstr(b.c[n]->pages,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->publisher) 
	  if (mystrstr(b.c[n]->publisher,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->reprint) 
	  if (mystrstr(b.c[n]->reprint,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->school) 
	  if (mystrstr(b.c[n]->school,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->series) 
	  if (mystrstr(b.c[n]->series,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->title) 
	  if (mystrstr(b.c[n]->title,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->type) 
	  if (mystrstr(b.c[n]->type,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->volume) 
	  if (mystrstr(b.c[n]->volume,b.d->keyword, cis)) hinzu=1;
	if (b.c[n]->year) 
	  if (mystrstr(b.c[n]->year,b.d->keyword, cis)) hinzu=1;
      }
      if (hinzu) {
	gefunden[0]++;
	gefunden[gefunden[0]]=n;
      }
    } /* endfor */
    // ........................................ Liste neu aufbauen;
    str_list=(XmStringTable) XtMalloc(gefunden[0]*sizeof(XmString));
    key=(unsigned long *)realloc(key,(gefunden[0]+1)*sizeof(unsigned long));
    key[0]=gefunden[0];
    for(n=1; n<=gefunden[0]; n++) {
      key[n]=gefunden[n];
      sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	      b.c[gefunden[n]]->id,
	      b.c[gefunden[n]]->author,
	      b.c[gefunden[n]]->title,
	      b.c[gefunden[n]]->journal,
	      b.c[gefunden[n]]->year);
      str_list[n-1]=XmStringCreateLocalized(ref);
    }
    XmListDeleteAllItems(liste);
    XtVaSetValues(liste,
		  XmNitems, str_list,
		  XmNitemCount, gefunden[0],
		  NULL);
    set_message(gefunden[0]);
    XtFree((char *)str_list);
    break;

  case 3:  // .... not;
    for (n=1; n<=key[0]; n++) {
      hinzu=1;
      if (b.c[key[n]]->doctype)
	if (mystrstr(b.c[key[n]]->doctype,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->keyword)
	if (mystrstr(b.c[key[n]]->keyword,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->abstract)
	if (mystrstr(b.c[key[n]]->abstract,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->address)
	if (mystrstr(b.c[key[n]]->address,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->annote)
	if (mystrstr(b.c[key[n]]->annote,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->author)
	if (mystrstr(b.c[key[n]]->author,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->booktitle)
	if (mystrstr(b.c[key[n]]->booktitle,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->chapter)
	if (mystrstr(b.c[key[n]]->chapter,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->comment)
	if (mystrstr(b.c[key[n]]->comment,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->crossref)
	if (mystrstr(b.c[key[n]]->crossref,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->edition)
	if (mystrstr(b.c[key[n]]->edition,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->editor)
	if (mystrstr(b.c[key[n]]->editor,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->howpublished)
	if (mystrstr(b.c[key[n]]->howpublished,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->institution)
	if (mystrstr(b.c[key[n]]->institution,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->journal)
	if (mystrstr(b.c[key[n]]->journal,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->key)
	if (mystrstr(b.c[key[n]]->key,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->month)
	if (mystrstr(b.c[key[n]]->month,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->note)
	if (mystrstr(b.c[key[n]]->note,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->number)
	if (mystrstr(b.c[key[n]]->number,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->organization)
	if (mystrstr(b.c[key[n]]->organization,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->pages)
	if (mystrstr(b.c[key[n]]->pages,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->publisher)
	if (mystrstr(b.c[key[n]]->publisher,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->reprint)
	if (mystrstr(b.c[key[n]]->reprint,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->school)
	if (mystrstr(b.c[key[n]]->school,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->series)
	if (mystrstr(b.c[key[n]]->series,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->title)
	if (mystrstr(b.c[key[n]]->title,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->type)
	if (mystrstr(b.c[key[n]]->type,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->volume)
	if (mystrstr(b.c[key[n]]->volume,b.d->keyword, cis)) hinzu=0;
      if (b.c[key[n]]->year)
	if (mystrstr(b.c[key[n]]->year,b.d->keyword, cis)) hinzu=0;
      if (hinzu) {
	gefunden[0]++;
	gefunden[gefunden[0]]=key[n];
      }
    } /* endfor */
    // ........................................ Liste neu aufbauen;
    str_list=(XmStringTable) XtMalloc(gefunden[0]*sizeof(XmString));
    key=(unsigned long *)realloc(key,(gefunden[0]+1)*sizeof(unsigned long));
    key[0]=gefunden[0];
    for(n=1; n<=gefunden[0]; n++) {
      key[n]=gefunden[n];
      sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	      b.c[gefunden[n]]->id,
	      b.c[gefunden[n]]->author,
	      b.c[gefunden[n]]->title,
	      b.c[gefunden[n]]->journal,
	      b.c[gefunden[n]]->year);
      str_list[n-1]=XmStringCreateLocalized(ref);
    }
    XmListDeleteAllItems(liste);
    XtVaSetValues(liste,
		  XmNitems, str_list,
		  XmNitemCount, gefunden[0],
		  NULL);
    set_message(gefunden[0]);
    XtFree((char *)str_list);
    break;
  }

  XDefineCursor(XtDisplay(XtParent(widget)),XtWindow(XtParent(widget)),pfeil);
  XFreeCursor(XtDisplay(XtParent(widget)), pfeil);
  XFreeCursor(XtDisplay(XtParent(widget)), uhr);
  XtUnmanageChild(widget);
}
