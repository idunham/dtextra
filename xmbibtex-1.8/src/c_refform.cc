/* Reference entry form */

#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <inttypes.h>
#include <cstdio>
#include "globals.h"

/* reas: reason why this function is called */
/*    0: add reference */
/*    1: edit reference */
/*    2: search reference */

int c_refform(short rt, short reas) {

  void buildform(short, short, int*);

  int answer=-1;

  buildform(rt, reas, &answer);

  while (answer < 0)
    XtAppProcessEvent(app, XtIMAll);

  return answer;
}

// ................   b u i l d f o r m ( )   .....................;
void buildform(short rt, short reas, int *a) {

  Widget shell, form, rowcol;
  Widget l,t,o,p;
  short  fcol;
  static short reason;
  static int   *answer;
  char   refid[80];

  void id_cb(Widget, XtPointer, XtPointer);
  void reftype_cb(Widget, XtPointer, XtPointer);
  void reprint_cb(Widget, XtPointer, XtPointer);
  void key_cb(Widget, XtPointer, XtPointer);
  void crossref_cb(Widget, XtPointer, XtPointer);
  void journal_cb(Widget, XtPointer, XtPointer);
  void howpublished_cb(Widget, XtPointer, XtPointer);
  void volume_cb(Widget, XtPointer, XtPointer);
  void number_cb(Widget, XtPointer, XtPointer);
  void edition_cb(Widget, XtPointer, XtPointer);
  void month_cb(Widget, XtPointer, XtPointer);
  void chapter_cb(Widget, XtPointer, XtPointer);
  void pages_cb(Widget, XtPointer, XtPointer);
  void year_cb(Widget, XtPointer, XtPointer);
  void title_cb(Widget, XtPointer, XtPointer);
  void booktitle_cb(Widget, XtPointer, XtPointer);
  void series_cb(Widget, XtPointer, XtPointer);
  void type_cb(Widget, XtPointer, XtPointer);
  void author_cb(Widget, XtPointer, XtPointer);
  void institution_cb(Widget, XtPointer, XtPointer);
  void organization_cb(Widget, XtPointer, XtPointer);
  void school_cb(Widget, XtPointer, XtPointer);
  void editor_cb(Widget, XtPointer, XtPointer);
  void publisher_cb(Widget, XtPointer, XtPointer);
  void address_cb(Widget, XtPointer, XtPointer);
  void abstract_cb(Widget, XtPointer, XtPointer);
  void keyword_cb(Widget, XtPointer, XtPointer);
  void note_cb(Widget, XtPointer, XtPointer);
  void annote_cb(Widget, XtPointer, XtPointer);
  void comment_cb(Widget, XtPointer, XtPointer);
  void ok_cb(Widget, XtPointer, XtPointer);
  void cancel_cb(Widget, XtPointer, XtPointer);

  if (*a < 0) answer=a;

  if (reas>=0) reason=reas;

  switch(reason) {
  case 0: // ................add;
    strcpy(refid, "Add new reference");
    break;
  case 1: // ................edit;
    strcpy(refid, "Edit reference");
    break;
  case 2: // ................search;
    strcpy(refid, "Search references");
  }

  shell= XtVaCreatePopupShell("ReferenceForm",
			      topLevelShellWidgetClass, toplevel,
			      XtNtitle, refid, NULL);

  form=XtVaCreateManagedWidget("form",
			       xmFormWidgetClass, shell,
			       NULL);

  rowcol=XtVaCreateWidget("rowcol", xmRowColumnWidgetClass, form,
			  XmNtopAttachment, XmATTACH_FORM,
			  XmNleftAttachment, XmATTACH_FORM,
			  XmNrightAttachment, XmATTACH_FORM,
			  XmNorientation, XmHORIZONTAL,
			  XmNnumColumns, 1,
			  NULL);
  

  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, rowcol,
			    XmNlabelString, XmStringCreateLocalized("Ref. ID:"),
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  sprintf(refid,"%d",b.d->id);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, rowcol,
			    XmNcolumns, 8,
			    XmNvalue, refid,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, id_cb, &reason);

  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, rowcol,
			    XmNlabelString, XmStringCreateLocalized("Key:"),
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, rowcol,
			    XmNcolumns, 8,
			    XmNvalue, b.d->key,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, key_cb, NULL);

  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, rowcol,
			    XmNlabelString, XmStringCreateLocalized("Crossref:"),
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, rowcol,
			    XmNcolumns, 8,
			    XmNvalue, b.d->crossref,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, crossref_cb, NULL);

  o=XmVaCreateSimpleOptionMenu(rowcol,"option",
			       XmStringCreateLocalized(""), 0, rt, reftype_cb,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("article"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("book"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("booklet"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("inbook"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("incollection"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("inproceedings"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("manual"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("mastersthesis"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("misc"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("phdthesis"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("proceedings"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("techreport"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("unpublished"), NULL, NULL, NULL,
			       NULL);
  XtManageChild(o);
  o=XmVaCreateSimpleOptionMenu(rowcol,"option",
			       XmStringCreateLocalized(""), 0,
			       (strcmp(b.d->reprint,"IN FILE")==0) ? 0 : 1,
			       reprint_cb,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("IN FILE"), NULL, NULL, NULL,
			       XmVaPUSHBUTTON, XmStringCreateLocalized("NOT IN FILE"), NULL, NULL, NULL,
			       NULL);
  XtManageChild(o);

  p=XtVaCreateManagedWidget("ok", xmPushButtonWidgetClass, rowcol,
			    XmNlabelString, XmStringCreateLocalized("Ok"),
			    NULL);
  XtAddCallback(p, XmNactivateCallback, ok_cb, answer);
  p=XtVaCreateManagedWidget("cancel", xmPushButtonWidgetClass, rowcol,
			    XmNlabelString, XmStringCreateLocalized("Cancel"),
			    NULL);
  XtAddCallback(p, XmNactivateCallback, cancel_cb, answer);
  XtManageChild(rowcol);

  // ..............   s t a r t   n e u e   Z e i l e  ............;
  fcol=0;
  if (rt==0) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Journal:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 35,
			      XmNvalue, b.d->journal,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, journal_cb, NULL);
    fcol=1;
  }
  if (rt==2 || rt==8) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Howpublished:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 35,
			      XmNvalue, b.d->howpublished,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, howpublished_cb, NULL);
    fcol=1;
  }
  if (rt==0 || rt==1 || rt==3 || rt==4 || rt==5 || rt==10) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Volume:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 12,
			      XmNvalue, b.d->volume,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, volume_cb, NULL);
    fcol=1;
  }
  if (rt==0 || rt==1 || rt==3 || rt==4 || rt==5 || rt==10 || rt==11) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Number:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 8,
			      XmNvalue, b.d->number,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, number_cb, NULL);
    fcol=1;
  }
  if (rt==1 || rt==3 || rt==4 || rt==6) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Edition:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 12,
			      XmNvalue, b.d->edition,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, edition_cb, NULL);
    fcol=1;
  }
  // month is allways possible;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, rowcol,
			    XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			    XmNleftWidget, t,
			    XmNlabelString, XmStringCreateLocalized("Month:"),
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, l,
			    XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			    XmNleftWidget, t,
			    XmNcolumns, 12,
			    XmNvalue, b.d->month,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, month_cb, NULL);
  fcol=1;
  if (rt==3 || rt==4) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Chapter:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 12,
			      XmNvalue, b.d->chapter,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, chapter_cb, NULL);
    fcol=1;
  }
  if (rt==0 || rt==3 || rt==4 || rt==5) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, rowcol,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNlabelString, XmStringCreateLocalized("Pages:"),
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, l,
			      XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			      XmNleftWidget, t,
			      XmNcolumns, 12,
			      XmNvalue, b.d->pages,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, pages_cb, NULL);
    fcol=1;
  }
  // year is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, rowcol,
			    XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			    XmNleftWidget, t,
			    XmNlabelString, XmStringCreateLocalized("Year:"),
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, l,
			    XmNleftAttachment, fcol ? XmATTACH_WIDGET : XmATTACH_FORM,
			    XmNleftWidget, t,
			    XmNcolumns, 8,
			    XmNvalue, b.d->year,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, year_cb, NULL);

  // ..............   s t a r t   n e u e   Z e i l e  ............;
  // title is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNlabelString, XmStringCreateLocalized("Title:"),
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNrows, 3,
			    XmNwordWrap, True,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNvalue, b.d->title,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, title_cb, NULL);
  if(rt==4 || rt==5) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Booktitle:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows,3,
			      XmNwordWrap, True,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->booktitle,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, booktitle_cb, NULL);
  }
  if(rt==1 || rt==3 || rt==4 || rt==5 || rt==10) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Series:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows,1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->series,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, series_cb, NULL);
  }
  if(rt==3 || rt==4 || rt==7 || rt==9 || rt==11) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Type:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->type,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, type_cb, NULL);
  }
  // author is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNlabelString, XmStringCreateLocalized("Author:"),
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNrows, 2,
			    XmNwordWrap, True,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNvalue, b.d->author,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, author_cb, NULL);
  if (rt==11) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Institution:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNwordWrap, True,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->institution,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, institution_cb, NULL);
  }
  if (rt==5 || rt==6) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Organization:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->organization,
			      NULL); 
    XtAddCallback(t, XmNlosingFocusCallback, organization_cb, NULL);
  }
  if (rt==7 || rt==9) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("School:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->school,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, school_cb, NULL);
  }
  if (rt==1 || rt==3 || rt==4 || rt==5 || rt==10) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Editor:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 2,
			      XmNwordWrap, True,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->editor,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, editor_cb, NULL);
  }
  if (rt==1 || rt==3 || rt==4 || rt==5 ||rt==10) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Publisher:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->publisher,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, publisher_cb, NULL);
  }
  if (rt==1 || rt==2 || rt==3 || rt==4 || rt==5 || rt==6 ||
      rt==7 || rt==9 || rt==10 || rt==11) {
    l=XtVaCreateManagedWidget("label",
			      xmLabelGadgetClass, form,
			      XmNlabelString, XmStringCreateLocalized("Address:"),
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_FORM,
			      XmNwidth, 100,
			      XmNalignment, XmALIGNMENT_END,
			      NULL);
    t=XtVaCreateManagedWidget("text",
			      xmTextWidgetClass, form,
			      XmNeditMode, XmMULTI_LINE_EDIT,
			      XmNrows, 1,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, t,
			      XmNleftAttachment, XmATTACH_WIDGET,
			      XmNleftWidget, l,
			      XmNrightAttachment, XmATTACH_FORM,
			      XmNvalue, b.d->address,
			      NULL);
    XtAddCallback(t, XmNlosingFocusCallback, address_cb, NULL);
  }
  // abstract is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNlabelString, XmStringCreateLocalized("Abstract:"),
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNrows, 8,
			    XmNwordWrap, True,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNvalue, b.d->abstract,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, abstract_cb, NULL);


  // ..............   s t a r t   n e u e   Z e i l e  ............;
  // keyword is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNlabelString, XmStringCreateLocalized("Keyword:"),
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  o=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNcolumns, 45,
			    XmNrows, 4,
			    XmNwordWrap, True,
			    XmNvalue, b.d->keyword,
			    NULL);
  XtAddCallback(o, XmNlosingFocusCallback, keyword_cb, NULL);
  // note is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, o,
			    XmNlabelString, XmStringCreateLocalized("Note:"),
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrows, 4,
			    XmNwordWrap, True,
			    XmNvalue, b.d->note,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, note_cb, NULL);
  // annote is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNlabelString, XmStringCreateLocalized("Annote:"),
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  o=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNcolumns, 45,
			    XmNrows, 4,
			    XmNwordWrap, True,
			    XmNvalue, b.d->annote,
			    NULL);
  XtAddCallback(o, XmNlosingFocusCallback, annote_cb, NULL);
  // comment is always included;
  l=XtVaCreateManagedWidget("label",
			    xmLabelGadgetClass, form,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, o,
			    XmNlabelString, XmStringCreateLocalized("Comment:"),
			    XmNwidth, 100,
			    XmNalignment, XmALIGNMENT_END,
			    NULL);
  t=XtVaCreateManagedWidget("text",
			    xmTextWidgetClass, form,
			    XmNeditMode, XmMULTI_LINE_EDIT,
			    XmNtopAttachment, XmATTACH_WIDGET,
			    XmNtopWidget, t,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, l,
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrows, 4,
			    XmNwordWrap, True,
			    XmNvalue, b.d->comment,
			    NULL);
  XtAddCallback(t, XmNlosingFocusCallback, comment_cb, NULL);

  XtPopup(shell, XtGrabNone);
}


void ok_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  int *answer;

  answer=(int *)client_data;
  while(w && !XtIsWMShell(w)) w=XtParent(w);
  XtPopdown(w);
  XtDestroyWidget(w);
  *answer=1;
}

void cancel_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  int *answer;

  answer=(int *)client_data;
  while(w && !XtIsWMShell(w)) w=XtParent(w);
  XtPopdown(w);
  XtDestroyWidget(w);
  *answer=0;
}

void reftype_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  intptr_t    rc;
  char   dt[80];
  int    answer=0;

  rc= (intptr_t)client_data;
  while(w && !XtIsWMShell(w)) w=XtParent(w);
  XtPopdown(w);
  XtDestroyWidget(w);
  switch(rc) {
  case 0:
    strcpy(dt,"article");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 1:
    strcpy(dt,"book");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 2:
    strcpy(dt,"booklet");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 3:
    strcpy(dt,"inbook");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 4:
    strcpy(dt,"incollection");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 5:
    strcpy(dt,"inproceedings");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 6:
    strcpy(dt,"manual");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 7:
    strcpy(dt,"mastersthesis");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 8:
    strcpy(dt,"misc");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 9:
    strcpy(dt,"phdthesis");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 10:
    strcpy(dt,"proceedings");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 11:
    strcpy(dt,"techreport");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  case 12:
    strcpy(dt,"unpublished");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
    break;
  default:
    strcpy(dt,"article");
    b.d->doctype=(char *)realloc(b.d->doctype, strlen(dt)+1);
    strcpy(b.d->doctype,dt);
  }
  buildform(rc,-1,&answer);
}

void reprint_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  if ((intptr_t)client_data==0) {
    b.d->reprint=(char *)realloc(b.d->reprint,strlen("IN FILE")+1);
    strcpy(b.d->reprint, "IN FILE");
  }
  else {
    b.d->reprint=(char *)realloc(b.d->reprint,strlen("NOT IN FILE")+1);
    strcpy(b.d->reprint, "NOT IN FILE");
  }

}

void id_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (*(short *)client_data==2)  
    if (text=XmTextGetString(w)) {
      b.d->id=strtoul(text,NULL,10);
      XtFree(text);
    }
}

void key_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->key=(char *)realloc(b.d->key,strlen(text)+1);
    strcpy(b.d->key, text);
    XtFree(text);
  }
}
void crossref_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->crossref=(char *)realloc(b.d->crossref,strlen(text)+1);
    strcpy(b.d->crossref, text);
    XtFree(text);
  }
}
void journal_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->journal=(char *)realloc(b.d->journal,strlen(text)+1);
    strcpy(b.d->journal, text);
    XtFree(text);
  }
}
void howpublished_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->howpublished=(char *)realloc(b.d->howpublished,strlen(text)+1);
    strcpy(b.d->howpublished, text);
    XtFree(text);
  }
}
void volume_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->volume=(char *)realloc(b.d->volume,strlen(text)+1);
    strcpy(b.d->volume, text);
    XtFree(text);
  }
}
void number_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->number=(char *)realloc(b.d->number,strlen(text)+1);
    strcpy(b.d->number, text);
    XtFree(text);
  }
}
void edition_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->edition=(char *)realloc(b.d->edition,strlen(text)+1);
    strcpy(b.d->edition, text);
    XtFree(text);
  }
}
void month_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->month=(char *)realloc(b.d->month,strlen(text)+1);
    strcpy(b.d->month, text);
    XtFree(text);
  }
}
void chapter_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->chapter=(char *)realloc(b.d->chapter,strlen(text)+1);
    strcpy(b.d->chapter, text);
    XtFree(text);
  }
}
void pages_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->pages=(char *)realloc(b.d->pages,strlen(text)+1);
    strcpy(b.d->pages, text);
    XtFree(text);
  }
}
void year_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->year=(char *)realloc(b.d->year,strlen(text)+1);
    strcpy(b.d->year, text);
    XtFree(text);
  }
}
void title_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->title=(char *)realloc(b.d->title,strlen(text)+1);
    strcpy(b.d->title, text);
    XtFree(text);
  }
}
void booktitle_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->booktitle=(char *)realloc(b.d->booktitle,strlen(text)+1);
    strcpy(b.d->booktitle, text);
    XtFree(text);
  }
}
void series_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->series=(char *)realloc(b.d->series,strlen(text)+1);
    strcpy(b.d->series, text);
    XtFree(text);
  }
}
void type_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->type=(char *)realloc(b.d->type,strlen(text)+1);
    strcpy(b.d->type, text);
    XtFree(text);
  }
}
void author_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->author=(char *)realloc(b.d->author,strlen(text)+1);
    strcpy(b.d->author, text);
    XtFree(text);
  }
}
void institution_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->institution=(char *)realloc(b.d->institution,strlen(text)+1);
    strcpy(b.d->institution, text);
    XtFree(text);
  }
}
void organization_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->organization=(char *)realloc(b.d->organization,strlen(text)+1);
    strcpy(b.d->organization, text);
    XtFree(text);
  }
}
void school_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->school=(char *)realloc(b.d->school,strlen(text)+1);
    strcpy(b.d->school, text);
    XtFree(text);
  }
}
void editor_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->editor=(char *)realloc(b.d->editor,strlen(text)+1);
    strcpy(b.d->editor, text);
    XtFree(text);
  }
}
void publisher_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->publisher=(char *)realloc(b.d->publisher,strlen(text)+1);
    strcpy(b.d->publisher, text);
    XtFree(text);
  }
}
void address_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->address=(char *)realloc(b.d->address,strlen(text)+1);
    strcpy(b.d->address, text);
    XtFree(text);
  }
}
void abstract_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->abstract=(char *)realloc(b.d->abstract,strlen(text)+1);
    strcpy(b.d->abstract, text);
    XtFree(text);
  }
}
void keyword_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->keyword=(char *)realloc(b.d->keyword,strlen(text)+1);
    strcpy(b.d->keyword, text);
    XtFree(text);
  }
}
void note_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->note=(char *)realloc(b.d->note,strlen(text)+1);
    strcpy(b.d->note, text);
    XtFree(text);
  }
}
void annote_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->annote=(char *)realloc(b.d->annote,strlen(text)+1);
    strcpy(b.d->annote, text);
    XtFree(text);
  }
}
void comment_cb(Widget w, XtPointer client_data, XtPointer call_data) {
  char *text;
  if (text=XmTextGetString(w)) {
    b.d->comment=(char *)realloc(b.d->comment,strlen(text)+1);
    strcpy(b.d->comment, text);
    XtFree(text);
  }
}

