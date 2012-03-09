/* Reference Manager      */
/* Print LaTeX Module     */
/* Last change 18.08.1996 */

#include <iostream.h>
#include <fstream.h>
#include <string.h>
#include <X11/cursorfont.h>
#include "globals.h"

void print_cb(Widget widget, XtPointer client_data, XtPointer call_data) {

  char          *filename;
  char          *db, *pdb;
  unsigned long n;
  ofstream      out;
  Cursor        uhr, pfeil;

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

  out.open(filename);
  if(!out) {
    cout << "Cannot open LaTeX file for output! \n";
    return;
  }


  db=new char[255];
  strcpy(db,database);
  pdb=strrchr(db,'.');
  if (pdb) pdb[0]='\0';

  out << "\\documentclass[12pt]{article} \n";
  out << "\\begin{document} \n";
  for (n=1; n<=key[0]; n++) out << "\\nocite{" << b.c[key[n]]->id << "} \n";
  out << "\\bibliography{";
  out << db;
  out << "} \n";
  out << "\\bibliographystyle{plain} \n";
  out << "\\end{document} \n";

  out.close();

  delete[]db;

  XDefineCursor(XtDisplay(widget),XtWindow(widget),pfeil);
  XtUnmanageChild(widget);

  return;
}
