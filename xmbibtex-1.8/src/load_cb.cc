/* Reference Manager      */
/* Load File Module       */
/* Last change 05.05.1996 */


#include <iostream>
#include <X11/cursorfont.h>
#include "globals.h"


void load_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
  char          *filename;
  char          *ref;
  int           i, ftype;
  short         changekey;
  unsigned long n, nalt;
  XmStringTable str_list;
  Cursor        uhr, pfeil;

  extern unsigned long find_freekey(unsigned long);
  extern void set_message(unsigned long);

  using namespace std;

  ref    =new char[100];

  /* userData: ftype==1 -> bibtex,        ftype==2 -> Medline Ovid,   */
  /*           ftype==3 -> Medline Spirs, ftype==4 -> Medline PubMed, */
  /*           ftype==5 -> Inspec                                     */
  XtVaGetValues(widget, XmNuserData, &ftype, NULL);

  XmFileSelectionBoxCallbackStruct *cbs=
    (XmFileSelectionBoxCallbackStruct *) call_data;

  if (!XmStringGetLtoR(cbs->value, XmFONTLIST_DEFAULT_TAG, &filename)) {
    cout << "Internal error pasting filename! \n";
    return;
  }
  if (!*filename) {
    cout << "No file selected! \n";
    XtFree(filename);
    return;
  }

  uhr  =XCreateFontCursor(XtDisplay(widget),XC_watch);
  pfeil=XCreateFontCursor(XtDisplay(widget), XC_arrow);

  XDefineCursor(XtDisplay(widget),XtWindow(widget),uhr);
  XFlush(XtDisplay(widget));

  strcpy(database,filename);
  XtVaSetValues(toplevel,
		XmNtitle, database,
		NULL);

  if (b.last>0) {
    changekey=1;
    nalt=b.last;
  }
  else {
    changekey=0;
    nalt=0;
  }

  /* E i n g a b e d a t e i   o e f f n e n */
  switch(ftype) {
  case 1:    // BibTeX file format;
    b.Read_BibTeX(filename);
    break;
  case 2:    // Ovid file format;
    b.Read_MedlineOvid(filename, "article");
    break;
  case 3:    // Spirs file format;
    b.Read_MedlineSpirs(filename, "article");
    break;
  case 4:    // Spirs file format;
    b.Read_MedlinePubmed(filename, "article");
    break;
  case 5:    // Inspec file format;
    b.Read_Inspec(filename, "article");
    break;
  }

  if (changekey) {
    for (n=nalt; n<b.last; n++)
      b.c[n]->id=find_freekey(n);
  }

  str_list=(XmStringTable) XtMalloc(b.last*sizeof(XmString));
  key=(unsigned long *)realloc(key,(b.last+1)*sizeof(unsigned long));
  key[0]=b.last;
  for(n=0; n<b.last; n++) {
    sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	    b.c[n]->id,
	    b.c[n]->author,
	    b.c[n]->title,
	    b.c[n]->journal,
	    b.c[n]->year);
    str_list[n]=XmStringCreateLocalized(ref);
    key[n+1]=n;
  }

  XtVaSetValues(liste,
		XmNitems, str_list,
		XmNitemCount, b.last,
		NULL);
  set_message(b.last);

  if (b.last>0) {
    XtSetSensitive(mymenu.file.f_new,True);
    XtSetSensitive(mymenu.file.f_save,True);
    XtSetSensitive(mymenu.file.f_print,True);
    XtSetSensitive(mymenu.edit.e_edit,True);
    XtSetSensitive(mymenu.edit.e_delete,True);
    XtSetSensitive(mymenu.edit.e_reduce,True);
    XtSetSensitive(mymenu.search.s_searchall,True);
    XtSetSensitive(mymenu.search.s_and,True);
    XtSetSensitive(mymenu.search.s_or,True);
    XtSetSensitive(mymenu.search.s_not,True);
    XtSetSensitive(mymenu.search.s_showall,True);
    XtSetSensitive(mymenu.search.s_case,True);
  }

  for(n=0; n<b.last; n++) XmStringFree(str_list[n]);
  XtFree((char *)str_list);
  XtFree(filename);
  delete[]ref;

  XDefineCursor(XtDisplay(widget),XtWindow(widget),pfeil);
  XtUnmanageChild(widget);

  return;
}



