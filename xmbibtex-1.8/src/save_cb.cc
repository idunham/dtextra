/* Reference Manager      */
/* Save File Module       */
/* Last change 15.06.1998 */

#include <X11/cursorfont.h>
#include <Xm/MessageB.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include "globals.h"

void save_cb(Widget widget, XtPointer client_data, XtPointer call_data) {

  char          *filename;
  int           i;
  unsigned long n;
  Cursor        uhr, pfeil;
  Arg           args[10];
  Widget        fexist;
  XmString      fname;
  std::ofstream outdat;
  std::ifstream indat;

  void kill_file(Widget widget, XtPointer client_data, XtPointer call_data);

  XtVaGetValues(widget, XmNdirSpec, &fname, NULL);
  if (!XmStringGetLtoR(fname, XmFONTLIST_DEFAULT_TAG, &filename)) {
    std::cout << "Internal error pasting filename! \n";
    return;
  }
  if (!*filename) {
    std::cout << "No file selected! \n";
    XtFree(filename);
    return;
  }

  /* check whether filename already exists */
  indat.open(filename, std::ifstream::in);
  /*  outdat.open(filename, std::ofstream::app); */
  if (indat) {
    i=0;
    XtSetArg(args[i], XmNmessageString, XmStringCreateLtoR("File already exists.\n     Overwrite?",XmFONTLIST_DEFAULT_TAG)); i++;
    XtSetArg(args[i], XmNokLabelString, XmStringCreateLocalized(" Yes ")); i++;
    XtSetArg(args[i], XmNcancelLabelString, XmStringCreateLocalized(" No ")); i++;
    XtSetArg(args[i], XmNtitle, "Save Database"); i++;
    XtSetArg(args[i], XmNuserData, client_data); i++;
    fexist=XmCreateQuestionDialog(XtParent(widget), "fexist", args, i);
    XtAddCallback(fexist, XmNokCallback, kill_file, widget);
    XtUnmanageChild(XmMessageBoxGetChild(fexist, XmDIALOG_HELP_BUTTON));
    XtManageChild(fexist);
    XtPopup(XtParent(fexist), XtGrabNone);
  }
  else {
    /* file does not exist - now write data */
    uhr  =XCreateFontCursor(XtDisplay(widget),XC_watch);
    pfeil=XCreateFontCursor(XtDisplay(widget), XC_arrow);
    XDefineCursor(XtDisplay(widget),XtWindow(widget),uhr);
    XFlush(XtDisplay(widget));
    strcpy(database,filename);
    XtVaSetValues(toplevel,
		  XmNtitle, database,
		  NULL);
    /* A u s g a b e d a t e i   s c h r e i b e n   */
    b.Write_BibTeX(filename);
    //  b.Write_Medline(filename);
    XDefineCursor(XtDisplay(widget),XtWindow(widget),pfeil);
    XtUnmanageChild(widget);
    XFreeCursor(XtDisplay(widget), pfeil);
    XFreeCursor(XtDisplay(widget), uhr);
  }
  XtFree(filename);
  return;
}


void kill_file(Widget widget, XtPointer client_data, XtPointer call_data) {
  Widget   fsb_sd, ft_sd;
  XmString fname;
  char     *filename, *cmd;

  cmd=new char[256];
  // XtVaGetValues(widget, XmNuserData, &ft_sd, NULL);
  fsb_sd=(Widget) client_data;
  XtVaGetValues(fsb_sd, XmNdirSpec, &fname, NULL);
  if (XmStringGetLtoR(fname, XmFONTLIST_DEFAULT_TAG, &filename)) {
    sprintf(cmd, "rm %s", filename);
    system(cmd);
    XtFree(filename);
    // save_cb(fsb_sd, (XtPointer)ft_sd, NULL);
    save_cb(fsb_sd, NULL, NULL);
  }
  delete[]cmd;
}
