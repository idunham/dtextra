/* Reference Manager      */
/* by Harald M. Stauss    */
/* Main Module            */
/* Last change 30.04.2003 */

#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/CascadeB.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/List.h>
#include <Xm/FileSB.h>
#include <Xm/Label.h>
#include <Xm/MessageB.h>
#include <Xm/Text.h>
#include <X11/Xmu/Editres.h>
#include <stdio.h>
#include <inttypes.h>

#include "nrutil.c"
#include "sort.c"
#include "mybiblio.h"

#define VERSION "BibTeX Manager, Ver. 1.7"

/*   G l o b a l   V a r i a b e l s                           */
/* key: pointer to an array of unsigned long                   */
/*      index of key = number of element in Widget liste       */
/*      value of key = index to citation in CLASS BIBLIO b     */
/*      element 0 in key gives the number of elements in key   */

#define N_INPUT_FILTER 5   /* number of input filters */

/* Structure for menu system */
typedef struct {
  Widget f_menu;
  Widget f_new;
  Widget f_load;
  Widget f_save;
  Widget f_print;
  Widget f_quit;
} f_menu;

typedef struct {
  Widget e_menu;
  Widget e_add;
  Widget e_edit;
  Widget e_delete;
  Widget e_reduce;
} e_menu;

typedef struct {
  Widget s_menu;
  Widget s_searchall;
  Widget s_and;
  Widget s_or;
  Widget s_not;
  Widget s_showall;
  Widget s_case;
} s_menu;

typedef struct {
  f_menu file;
  e_menu edit;
  s_menu search;
} main_menu;


XtAppContext  app;
MYBIBLIO      b;
unsigned long *key;
char          *database;
Widget        toplevel;
Widget        liste;
Widget        message;
main_menu     mymenu;


main(int argc, char *argv[])
{
  Widget        main_w, menubar, PullDown;

  void file_cb(Widget, XtPointer, XtPointer);
  void edit_cb(Widget, XtPointer, XtPointer);
  void search_cb(Widget, XtPointer, XtPointer);
  void set_message(unsigned long);

  /* toplevel */
  toplevel = XtVaAppInitialize (&app, "XmBibTeX", NULL, 0,
				&argc, argv, NULL, NULL);
  XtVaSetValues(toplevel,
		XmNtitle, VERSION,
		NULL);

  XtAddEventHandler(toplevel, (EventMask)0, True,
		    _XEditResCheckMessages, NULL);
  /* mainwindow */
  main_w=XtVaCreateManagedWidget("main_w", xmMainWindowWidgetClass, toplevel,
				 XmNheight, 400,
				 XmNwidth, 600,
				 NULL);
  liste=XtVaCreateManagedWidget("reflist", xmListWidgetClass, main_w,
				XmNvisibleItemCount, 10,
				NULL);

  
  /* messagearea */
  message = XtVaCreateManagedWidget("message", xmLabelWidgetClass, main_w,
				    XmNalignment, XmALIGNMENT_BEGINNING,
				    XmNlabelString, XmStringCreateLocalized(" "),
				    NULL);
  XtVaSetValues(main_w, XmNmessageWindow, message, NULL);
  set_message(0);

  /* menusystem */
  menubar = XmCreateMenuBar(main_w, "menubar", NULL, 0);
  /*   F i l e m e n u   */;
  PullDown=XmCreatePulldownMenu(menubar, "FilePullDown", NULL,0);
  mymenu.file.f_menu=XtVaCreateManagedWidget("File",
					     xmCascadeButtonWidgetClass, menubar,
					     XmNlabelString, XmStringCreateLocalized("File"),
					     XmNmnemonic, 'F',
					     XmNsubMenuId, PullDown,
					     NULL);
  mymenu.file.f_new=XtVaCreateManagedWidget("New",
					    xmPushButtonGadgetClass, PullDown,
					    XmNmnemonic, 'N',
					    NULL);
  XtAddCallback(mymenu.file.f_new, XmNactivateCallback, file_cb, (XtPointer)0);
  mymenu.file.f_load=XtVaCreateManagedWidget("Load",
					     xmPushButtonGadgetClass, PullDown,
					     XmNmnemonic, 'L',
					     NULL);
  XtAddCallback(mymenu.file.f_load, XmNactivateCallback, file_cb, (XtPointer)1);
  mymenu.file.f_save=XtVaCreateManagedWidget("Save",
					     xmPushButtonGadgetClass, PullDown,
					     XmNmnemonic, 'S',
					     NULL);
  XtAddCallback(mymenu.file.f_save, XmNactivateCallback, file_cb, (XtPointer)2);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.file.f_print=XtVaCreateManagedWidget("Print",
					      xmPushButtonGadgetClass, PullDown,
					      XmNmnemonic, 'P',
					      NULL);
  XtAddCallback(mymenu.file.f_print, XmNactivateCallback, file_cb, (XtPointer)3);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.file.f_quit=XtVaCreateManagedWidget("Quit",
					     xmPushButtonGadgetClass, PullDown,
					     XmNmnemonic, 'Q',
					     NULL);
  XtAddCallback(mymenu.file.f_quit, XmNactivateCallback, file_cb, (XtPointer)4);
  /*   E d i t m e n u   */;
  PullDown=XmCreatePulldownMenu(menubar, "EditPullDown", NULL,0);
  mymenu.edit.e_menu=XtVaCreateManagedWidget("Edit",
					     xmCascadeButtonWidgetClass, menubar,
					     XmNlabelString, XmStringCreateLocalized("Edit"),
					     XmNmnemonic, 'E',
					     XmNsubMenuId, PullDown,
					     NULL);
  mymenu.edit.e_add=XtVaCreateManagedWidget("Add reference",
					    xmPushButtonGadgetClass, PullDown,
					    XmNmnemonic, 'A',
					    NULL);
  XtAddCallback(mymenu.edit.e_add, XmNactivateCallback, edit_cb, (XtPointer)0);
  mymenu.edit.e_edit=XtVaCreateManagedWidget("Edit reference",
					     xmPushButtonGadgetClass, PullDown,
					     XmNmnemonic, 'E',
					     NULL);
  XtAddCallback(mymenu.edit.e_edit, XmNactivateCallback, edit_cb, (XtPointer)1);
  mymenu.edit.e_delete=XtVaCreateManagedWidget("Delete reference",
					       xmPushButtonGadgetClass, PullDown,
					       XmNmnemonic, 'D',
					       NULL);
  XtAddCallback(mymenu.edit.e_delete, XmNactivateCallback, edit_cb, (XtPointer)2);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.edit.e_reduce=XtVaCreateManagedWidget("Reduce to listed references",
					       xmPushButtonGadgetClass, PullDown,
					       XmNmnemonic, 'R',
					       NULL);
  XtAddCallback(mymenu.edit.e_reduce, XmNactivateCallback, edit_cb, (XtPointer)3);
  /*   S e a r c h m e n u   */;
  PullDown=XmCreatePulldownMenu(menubar, "SearchPullDown", NULL,0);
  mymenu.search.s_menu=XtVaCreateManagedWidget("Search",
					       xmCascadeButtonWidgetClass, menubar,
					       XmNlabelString, XmStringCreateLocalized("Search"),
					       XmNmnemonic, 'S',
					       XmNsubMenuId, PullDown,
					       NULL);
  mymenu.search.s_searchall=XtVaCreateManagedWidget("Search in all fields",
						    xmPushButtonGadgetClass, PullDown,
						    XmNmnemonic, 'S',
						    NULL);
  XtAddCallback(mymenu.search.s_searchall, XmNactivateCallback, search_cb, (XtPointer)0);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.search.s_and=XtVaCreateManagedWidget("AND",
					      xmPushButtonGadgetClass, PullDown,
					      XmNmnemonic, 'A',
					      NULL);
  XtAddCallback(mymenu.search.s_and, XmNactivateCallback, search_cb, (XtPointer)1);
  mymenu.search.s_or=XtVaCreateManagedWidget("OR",
					     xmPushButtonGadgetClass, PullDown,
					     XmNmnemonic, 'O',
					     NULL);
  XtAddCallback(mymenu.search.s_or, XmNactivateCallback, search_cb, (XtPointer)2);
  mymenu.search.s_not=XtVaCreateManagedWidget("NOT",
					      xmPushButtonGadgetClass, PullDown,
					      XmNmnemonic, 'N',
					      NULL);
  XtAddCallback(mymenu.search.s_not, XmNactivateCallback, search_cb, (XtPointer)3);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.search.s_showall=XtVaCreateManagedWidget("View all references",
						  xmPushButtonGadgetClass, PullDown,
						  XmNmnemonic, 'V',
						  NULL);
  XtAddCallback(mymenu.search.s_showall, XmNactivateCallback, search_cb, (XtPointer)4);
  XtVaCreateManagedWidget("separator", xmSeparatorGadgetClass, PullDown, NULL);
  mymenu.search.s_case=XtVaCreateManagedWidget("Case sensitive",
					       xmToggleButtonGadgetClass, PullDown,
					       XmNmnemonic, 'C',
					       NULL);
  XtAddCallback(mymenu.search.s_case, XmNvalueChangedCallback, search_cb, (XtPointer)5);

  XtSetSensitive(mymenu.file.f_new,False);
  XtSetSensitive(mymenu.file.f_save,False);
  XtSetSensitive(mymenu.file.f_print,False);
  XtSetSensitive(mymenu.edit.e_edit,False);
  XtSetSensitive(mymenu.edit.e_delete,False);
  XtSetSensitive(mymenu.edit.e_reduce,False);
  XtSetSensitive(mymenu.search.s_searchall,False);
  XtSetSensitive(mymenu.search.s_and,False);
  XtSetSensitive(mymenu.search.s_or,False);
  XtSetSensitive(mymenu.search.s_not,False);
  XtSetSensitive(mymenu.search.s_showall,False);
  XtSetSensitive(mymenu.search.s_case,False);

  XtManageChild(menubar);
  XmMainWindowSetAreas(main_w, menubar, NULL, NULL, NULL, liste);

  /* setup arrays */
  database=new char[255];
  key=new unsigned long[1];
  key[0]=0;

  XtRealizeWidget (toplevel);
  XtAppMainLoop (app);
}


/*   C a l l b a c k   F u n c t i o n s   */

void file_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  extern void load_cb(Widget, XtPointer, XtPointer);
  extern void ftype_cb(Widget, XtPointer, XtPointer);
  extern void save_cb(Widget, XtPointer, XtPointer);
  extern void print_cb(Widget, XtPointer, XtPointer);
  void fsb_cancel(Widget, XtPointer, XtPointer);
  void set_message(unsigned long);

  static Widget fsb_load, ftype, fsb_save, fsb_print;
  XmStringTable str_list;

  switch((intptr_t)client_data) {
  case 0:    // .................................new;
    XmListDeleteAllItems(liste);
    key=(unsigned long *)realloc(key, sizeof(unsigned long));
    key[0]=0;
    b.~MYBIBLIO();
    b=*(new MYBIBLIO());
    XtVaSetValues(toplevel,
		  XmNtitle, VERSION,
		  NULL);
    set_message(0);
    XtSetSensitive(mymenu.file.f_new,False);
    XtSetSensitive(mymenu.file.f_save,False);
    XtSetSensitive(mymenu.file.f_print,False);
    XtSetSensitive(mymenu.edit.e_edit,False);
    XtSetSensitive(mymenu.edit.e_delete,False);
    XtSetSensitive(mymenu.edit.e_reduce,False);
    XtSetSensitive(mymenu.search.s_searchall,False);
    XtSetSensitive(mymenu.search.s_and,False);
    XtSetSensitive(mymenu.search.s_or,False);
    XtSetSensitive(mymenu.search.s_not,False);
    XtSetSensitive(mymenu.search.s_showall,False);
    XtSetSensitive(mymenu.search.s_case,False);
    break;
  case 1:    // ..................................load;
    if (!fsb_load) {
      fsb_load=XmCreateFileSelectionDialog(toplevel, "filesb", NULL, 0);
      XtAddCallback(fsb_load,XmNokCallback, load_cb, NULL);
      XtAddCallback(fsb_load,XmNcancelCallback, fsb_cancel, NULL);
      str_list=(XmStringTable) XtMalloc(N_INPUT_FILTER*sizeof(XmString));
      str_list[0]=XmStringCreateLocalized("BibTeX");
      str_list[1]=XmStringCreateLocalized("Medline Ovid");
      str_list[2]=XmStringCreateLocalized("Medline Spirs");
      str_list[3]=XmStringCreateLocalized("Medline PubMed");
      str_list[4]=XmStringCreateLocalized("Inspec");
      ftype=XmCreateScrolledList(fsb_load, "filetype", NULL, 0);
      XtVaSetValues(ftype,
		    XmNselectionPolicy, XmSINGLE_SELECT,
		    XmNvisibleItemCount, 3,
		    XmNitemCount, N_INPUT_FILTER,
		    XmNitems, str_list,
		    NULL);
      XtAddCallback(ftype, XmNsingleSelectionCallback, ftype_cb, NULL);
      XmListSelectPos(ftype, 1, True);
      XtFree((char *)str_list);
    }
    XtManageChild(ftype);
    XtManageChild(fsb_load);
    XtPopup (XtParent(fsb_load), XtGrabNone);
    break;
  case 2:    // ..................................save;
    if (!fsb_save) {
      fsb_save=XmCreateFileSelectionDialog(toplevel, "filesb", NULL, 0);
      XtAddCallback(fsb_save,XmNokCallback, save_cb, NULL);
      XtAddCallback(fsb_save,XmNcancelCallback, fsb_cancel, NULL);
      XtVaSetValues(XtParent(fsb_save),
		    XmNtitle, "Save Database",
		    NULL);
    }
    XtManageChild(fsb_save);
    break;
  case 3:    // ...................................print;
    if (!fsb_print) {
      fsb_print=XmCreateFileSelectionDialog(toplevel, "filesb", NULL, 0);
      XtAddCallback(fsb_print,XmNokCallback, print_cb, NULL);
      XtAddCallback(fsb_print,XmNcancelCallback, fsb_cancel, NULL);
      XtVaSetValues(XtParent(fsb_print),
		    XmNtitle, "Save LaTeX",
		    NULL);
    }
    XtManageChild(fsb_print);
    break;
  case 4:    // ...................................quit;
    delete[]key;
    delete[]database;
    XtDestroyWidget(toplevel);
    exit(0);
    break;
  }
}

void ftype_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
  XmListCallbackStruct *cbs=(XmListCallbackStruct *) call_data;
  Widget fsbox=XtParent(XtParent(widget));
  char *choice, tit[80];

  XmStringGetLtoR(cbs->item, XmFONTLIST_DEFAULT_TAG, &choice);
  XtVaSetValues(fsbox,
		XmNuserData, cbs->item_position,
		NULL);
  strcpy(tit,"Load "); strcat(tit, choice);
  XtVaSetValues(XtParent(fsbox),
		XmNtitle, tit,
		NULL);
}


void fsb_cancel(Widget widget, XtPointer client_data, XtPointer call_data) {
  XtUnmanageChild(widget);
}

void edit_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  unsigned long listid, n, nk;
  short         islisted;
  int           *pos_list, pos_cnt;
  int           rc, i;
  char          *ref;
  XmStringTable str_list;

  extern int    c_refform(short, short);
  unsigned long find_freekey(unsigned long);
  void          set_message(unsigned long);

  ref    =new char[100];
  str_list=(XmStringTable) XtMalloc(sizeof(XmString));

  switch((intptr_t)client_data) {
  case 0:  // ................................add new reference
    b.ClearDummy();
    b.d->doctype=(char *)realloc(b.d->doctype, sizeof("article")+1);
    strcpy(b.d->doctype, "article");
    b.d->id=find_freekey(b.last);
    if (c_refform(0,0)) {
      b.add_citation();
      key[0]++;
      key=(unsigned long *)realloc(key,(key[0]+1)*sizeof(unsigned long));
      key[key[0]]=b.last-1;
      b.FromDummy(key[key[0]]);
      sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	      b.c[key[key[0]]]->id,
	      b.c[key[key[0]]]->author,
	      b.c[key[key[0]]]->title,
	      b.c[key[key[0]]]->journal,
	      b.c[key[key[0]]]->year);
      str_list[0]=XmStringCreateLocalized(ref);
      XmListAddItems(liste,str_list,1,0);
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
    }
    break;
  case 1:  // ................................edit reference
    if(XmListGetSelectedPos(liste,&pos_list,&pos_cnt) && pos_cnt>0) {
      listid=pos_list[0];
      XtFree((char *)pos_list);
      b.ToDummy(key[listid]);
      if (c_refform(b.DocTypeN(key[listid]),1)) {
	b.FromDummy(key[listid]);
	sprintf(ref,"%u %.15s %.30s %.15s %.4s",
	    b.c[key[listid]]->id,
	    b.c[key[listid]]->author,
	    b.c[key[listid]]->title,
	    b.c[key[listid]]->journal,
	    b.c[key[listid]]->year);
	str_list[0]=XmStringCreateLocalized(ref);
	XmListReplaceItemsPos(liste,str_list,1,listid);
      }
    }
    break;
  case 2:  // ................................delete reference
    if(XmListGetSelectedPos(liste,&pos_list,&pos_cnt) && pos_cnt>0) {
      listid=pos_list[0];
      XtFree((char *)pos_list);
      b.del_citation(key[listid]);
      for (n=listid; n<key[0]; n++) key[n]=key[n+1]-1;
      key[0]--;
      key=(unsigned long *)realloc(key,(key[0]+1)*sizeof(unsigned long));
      XmListDeletePos(liste,listid);
      set_message(key[0]);
      if (b.last==0) {
	XtSetSensitive(mymenu.file.f_new,False);
	XtSetSensitive(mymenu.file.f_save,False);
	XtSetSensitive(mymenu.file.f_print,False);
	XtSetSensitive(mymenu.edit.e_edit,False);
	XtSetSensitive(mymenu.edit.e_delete,False);
	XtSetSensitive(mymenu.edit.e_reduce,False);
	XtSetSensitive(mymenu.search.s_searchall,False);
	XtSetSensitive(mymenu.search.s_and,False);
	XtSetSensitive(mymenu.search.s_or,False);
	XtSetSensitive(mymenu.search.s_not,False);
	XtSetSensitive(mymenu.search.s_showall,False);
	XtSetSensitive(mymenu.search.s_case,False);
      }
    }
    break;
  case 3:  // ................................reduce to listed references
    for (n=0; n<b.last; n++) {
      // ...................................... already selected ? ;
      islisted=0;
      for (nk=1; nk<=key[0]; nk++) if (n==key[nk]) islisted=1;
      // ...................................... if not than delete;
      if(!islisted) {
	b.del_citation(n);
	for (nk=1; nk<=key[0]; nk++)
	  if (key[nk]>n) key[nk]--;
	n--;
       }
    }

    break;
  }

  XtFree((char *)str_list);
  delete[]ref;
}

unsigned long find_freekey(unsigned long limit) {
  unsigned long i, n, erste;
  float         *id;
  char          text[100];

  if (b.last>0) {
    if (limit>b.last) limit=b.last;
    id=new float[limit+1];

    for(i=0; i<limit; i++) id[i]=b.c[i]->id;
    sort(limit,id-1);

    n=(unsigned long)id[0]; erste=0;
    for(i=0; i<limit; i++) {
      if(id[i]>n) {
	erste=n;
	i=limit;
      }
      n++;
    }

    if (erste==0) erste=(unsigned long)id[limit-1]+1;
    delete[]id;
  }
  else
    erste=1;
  return erste;
}

void set_message(unsigned long nref) {
  char text[100];
  sprintf(text,"number of references: %u", nref);
  XtVaSetValues(message,
		XmNlabelString, XmStringCreateLocalized(text),
		NULL);
}

void search_cb(Widget widget, XtPointer client_data, XtPointer call_data)
{
  unsigned long n, nk;
  short         hinzu;
  int           i;
  static  int cis=1; // case insensitive
  unsigned long *gefunden;
  XmStringTable str_list;
  char          *ref;
  Arg           args[5];
  Widget        mbox, rowcol, w;

  extern int  c_refform(short, short);
  extern char *mystrstr(const char *s1, const char *s2, short c);
  extern void searchall_cb(Widget widget, XtPointer client_data, XtPointer call_data);
         void set_message(unsigned long);
	 void satext_cb(Widget widget, XtPointer client_data, XtPointer call_data);
	 void satoggle_cb(Widget widget, XtPointer client_data, XtPointer call_data);


  gefunden   = new unsigned long[b.last+1];
  gefunden[0]= 0;
  ref        = new char[100];

  switch((intptr_t)client_data) {
  case 0:    // ................................. search in all fields;
    i=0;
    XtSetArg(args[i], XmNtitle, "Search all fields"); i++;
    XtSetArg(args[i], XmNautoUnmanage, False); i++;
    XtSetArg(args[i], XmNmessageString,
	     XmStringCreateLocalized("Search in all fields for:")); i++;
    mbox=XmCreateMessageDialog(liste, "mbox", args, i);
    XtUnmanageChild(XmMessageBoxGetChild(mbox, XmDIALOG_HELP_BUTTON));
    XtAddCallback(mbox, XmNokCallback, searchall_cb, (XtPointer)cis);
    XtAddCallback(mbox, XmNcancelCallback, fsb_cancel,  NULL);
    rowcol=XtVaCreateWidget("rowcol", xmRowColumnWidgetClass, mbox,
			    XmNradioBehavior, True,
			    NULL);
    w=XtVaCreateManagedWidget("text", xmTextWidgetClass, rowcol,
			    NULL);
    XtAddCallback(w, XmNvalueChangedCallback, satext_cb, NULL);
    w=XtVaCreateManagedWidget("and", xmToggleButtonGadgetClass, rowcol,
			      XmNset, True,
			      NULL);
    b.d->id=1;
    XtAddCallback(w, XmNvalueChangedCallback, satoggle_cb, (XtPointer)1);
    w=XtVaCreateManagedWidget("or ", xmToggleButtonGadgetClass, rowcol,
			    NULL);
    XtAddCallback(w, XmNvalueChangedCallback, satoggle_cb, (XtPointer)2);
    w=XtVaCreateManagedWidget("not", xmToggleButtonGadgetClass, rowcol,
			    NULL);
    XtAddCallback(w, XmNvalueChangedCallback, satoggle_cb, (XtPointer)3);
    XtManageChild(rowcol);
    XtManageChild(mbox);
    break;
  case 1:    // ................................. AND;
    b.ClearDummy();
    b.d->id=0;
    if (c_refform(0,2)) {
      for (n=1; n<=key[0]; n++) {
	hinzu=1;
	if (b.d->id)
	  if (b.c[key[n]]->id != b.d->id) hinzu=0;
	if (strcmp(b.d->doctype,""))
	  if (!b.c[key[n]]->doctype) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->doctype,b.d->doctype, cis)) hinzu=0;
	if (strcmp(b.d->keyword,""))
	  if (!b.c[key[n]]->keyword) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->keyword,b.d->keyword, cis)) hinzu=0;
	if (strcmp(b.d->abstract,""))
	  if (!b.c[key[n]]->abstract) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->abstract,b.d->abstract, cis)) hinzu=0;
	if (strcmp(b.d->address,""))
	  if (!b.c[key[n]]->address) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->address,b.d->address, cis)) hinzu=0;
	if (strcmp(b.d->annote,""))
	  if (!b.c[key[n]]->annote) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->annote,b.d->annote, cis)) hinzu=0;
	if (strcmp(b.d->author,""))
	  if (!b.c[key[n]]->author) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->author,b.d->author, cis)) hinzu=0;
	if (strcmp(b.d->booktitle,""))
	  if (!b.c[key[n]]->booktitle) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->booktitle,b.d->booktitle, cis)) hinzu=0;
	if (strcmp(b.d->chapter,""))
	  if (!b.c[key[n]]->chapter) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->chapter,b.d->chapter, cis)) hinzu=0;
	if (strcmp(b.d->comment,""))
	  if (!b.c[key[n]]->comment) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->comment,b.d->comment, cis)) hinzu=0;
	if (strcmp(b.d->crossref,""))
	  if (!b.c[key[n]]->crossref) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->crossref,b.d->crossref, cis)) hinzu=0;
	if (strcmp(b.d->edition,""))
	  if (!b.c[key[n]]->edition) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->edition,b.d->edition, cis)) hinzu=0;
	if (strcmp(b.d->editor,""))
	  if (!b.c[key[n]]->editor) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->editor,b.d->editor, cis)) hinzu=0;
	if (strcmp(b.d->howpublished,""))
	  if (!b.c[key[n]]->howpublished) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->howpublished,b.d->howpublished, cis)) hinzu=0;
	if (strcmp(b.d->institution,""))
	  if (!b.c[key[n]]->institution) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->institution,b.d->institution, cis)) hinzu=0;
	if (strcmp(b.d->journal,""))
	  if (!b.c[key[n]]->journal) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->journal,b.d->journal, cis)) hinzu=0;
	if (strcmp(b.d->key,""))
	  if (!b.c[key[n]]->key) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->key,b.d->key, cis)) hinzu=0;
	if (strcmp(b.d->month,""))
	  if (!b.c[key[n]]->month) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->month,b.d->month, cis)) hinzu=0;
	if (strcmp(b.d->note,""))
	  if (!b.c[key[n]]->note) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->note,b.d->note, cis)) hinzu=0;
	if (strcmp(b.d->number,""))
	  if (!b.c[key[n]]->number) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->number,b.d->number, cis)) hinzu=0;
	if (strcmp(b.d->organization,""))
	  if (!b.c[key[n]]->organization) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->organization,b.d->organization, cis)) hinzu=0;
	if (strcmp(b.d->pages,""))
	  if (!b.c[key[n]]->pages) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->pages,b.d->pages, cis)) hinzu=0;
	if (strcmp(b.d->publisher,""))
	  if (!b.c[key[n]]->publisher) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->publisher,b.d->publisher, cis)) hinzu=0;
	if (strcmp(b.d->reprint,""))
	  if (!b.c[key[n]]->reprint) hinzu=0;
	  else if (strcmp(b.c[key[n]]->reprint,b.d->reprint)) hinzu=0;
	if (strcmp(b.d->school,""))
	  if (!b.c[key[n]]->school) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->school,b.d->school, cis)) hinzu=0;
	if (strcmp(b.d->series,""))
	  if (!b.c[key[n]]->series) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->series,b.d->series, cis)) hinzu=0;
	if (strcmp(b.d->title,""))
	  if (!b.c[key[n]]->title) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->title,b.d->title, cis)) hinzu=0;
	if (strcmp(b.d->type,""))
	  if (!b.c[key[n]]->type) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->type,b.d->type, cis)) hinzu=0;
	if (strcmp(b.d->volume,""))
	  if (!b.c[key[n]]->volume) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->volume,b.d->volume, cis)) hinzu=0;
	if (strcmp(b.d->year,""))
	  if (!b.c[key[n]]->year) hinzu=0;
	  else if (!mystrstr(b.c[key[n]]->year,b.d->year, cis)) hinzu=0;
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
      
    } /* endif */
    break;
  case 2:    // ................................. OR;
    b.ClearDummy();
    b.d->id=0;
    if (c_refform(0,2)) {
      for (n=0; n<b.last; n++) {
	// ...................................... already selected ? ;
	hinzu=0;
	for (nk=1; nk<=key[0]; nk++) if (n==key[nk]) hinzu=1;
	// ...................................... if not than check;
	if(!hinzu) {
	  hinzu=1;
	  if (b.d->id)
	    if (b.c[n]->id != b.d->id) hinzu=0;
	  if (strcmp(b.d->doctype,""))
	    if (!b.c[n]->doctype) hinzu=0;
	    else if (!mystrstr(b.c[n]->doctype,b.d->doctype, cis)) hinzu=0;
	  if (strcmp(b.d->keyword,""))
	    if (!b.c[n]->keyword) hinzu=0;
	    else if (!mystrstr(b.c[n]->keyword,b.d->keyword, cis)) hinzu=0;
	  if (strcmp(b.d->abstract,""))
	    if (!b.c[n]->abstract) hinzu=0;
	    else if (!mystrstr(b.c[n]->abstract,b.d->abstract, cis)) hinzu=0;
	  if (strcmp(b.d->address,""))
	    if (!b.c[n]->address) hinzu=0;
	    else if (!mystrstr(b.c[n]->address,b.d->address, cis)) hinzu=0;
	  if (strcmp(b.d->annote,""))
	    if (!b.c[n]->annote) hinzu=0;
	    else if (!mystrstr(b.c[n]->annote,b.d->annote, cis)) hinzu=0;
	  if (strcmp(b.d->author,""))
	    if (!b.c[n]->author) hinzu=0;
	    else if (!mystrstr(b.c[n]->author,b.d->author, cis)) hinzu=0;
	  if (strcmp(b.d->booktitle,""))
	    if (!b.c[n]->booktitle) hinzu=0;
	    else if (!mystrstr(b.c[n]->booktitle,b.d->booktitle, cis)) hinzu=0;
	  if (strcmp(b.d->chapter,""))
	    if (!b.c[n]->chapter) hinzu=0;
	    else if (!mystrstr(b.c[n]->chapter,b.d->chapter, cis)) hinzu=0;
	  if (strcmp(b.d->comment,""))
	    if (!b.c[n]->comment) hinzu=0;
	    else if (!mystrstr(b.c[n]->comment,b.d->comment, cis)) hinzu=0;
	  if (strcmp(b.d->crossref,""))
	    if (!b.c[n]->crossref) hinzu=0;
	    else if (!mystrstr(b.c[n]->crossref,b.d->crossref, cis)) hinzu=0;
	  if (strcmp(b.d->edition,""))
	    if (!b.c[n]->edition) hinzu=0;
	    else if (!mystrstr(b.c[n]->edition,b.d->edition, cis)) hinzu=0;
	  if (strcmp(b.d->editor,""))
	    if (!b.c[n]->editor) hinzu=0;
	    else if (!mystrstr(b.c[n]->editor,b.d->editor, cis)) hinzu=0;
	  if (strcmp(b.d->howpublished,""))
	    if (!b.c[n]->howpublished) hinzu=0;
	    else if (!mystrstr(b.c[n]->howpublished,b.d->howpublished, cis)) hinzu=0;
	  if (strcmp(b.d->institution,""))
	    if (!b.c[n]->institution) hinzu=0;
	    else if (!mystrstr(b.c[n]->institution,b.d->institution, cis)) hinzu=0;
	  if (strcmp(b.d->journal,""))
	    if (!b.c[n]->journal) hinzu=0;
	    else if (!mystrstr(b.c[n]->journal,b.d->journal, cis)) hinzu=0;
	  if (strcmp(b.d->key,""))
	    if (!b.c[n]->key) hinzu=0;
	    else if (!mystrstr(b.c[n]->key,b.d->key, cis)) hinzu=0;
	  if (strcmp(b.d->month,""))
	    if (!b.c[n]->month) hinzu=0;
	    else if (!mystrstr(b.c[n]->month,b.d->month, cis)) hinzu=0;
	  if (strcmp(b.d->note,""))
	    if (!b.c[n]->note) hinzu=0;
	    else if (!mystrstr(b.c[n]->note,b.d->note, cis)) hinzu=0;
	  if (strcmp(b.d->number,""))
	    if (!b.c[n]->number) hinzu=0;
	    else if (!mystrstr(b.c[n]->number,b.d->number, cis)) hinzu=0;
	  if (strcmp(b.d->organization,""))
	    if (!b.c[n]->organization) hinzu=0;
	    else if (!mystrstr(b.c[n]->organization,b.d->organization, cis)) hinzu=0;
	  if (strcmp(b.d->pages,""))
	    if (!b.c[n]->pages) hinzu=0;
	    else if (!mystrstr(b.c[n]->pages,b.d->pages, cis)) hinzu=0;
	  if (strcmp(b.d->publisher,""))
	    if (!b.c[n]->publisher) hinzu=0;
	    else if (!mystrstr(b.c[n]->publisher,b.d->publisher, cis)) hinzu=0;
	  if (strcmp(b.d->reprint,""))
	    if (!b.c[n]->reprint) hinzu=0;
	    else if (strcmp(b.c[n]->reprint,b.d->reprint)) hinzu=0;
	  if (strcmp(b.d->school,""))
	    if (!b.c[n]->school) hinzu=0;
	    else if (!mystrstr(b.c[n]->school,b.d->school, cis)) hinzu=0;
	  if (strcmp(b.d->series,""))
	    if (!b.c[n]->series) hinzu=0;
	    else if (!mystrstr(b.c[n]->series,b.d->series, cis)) hinzu=0;
	  if (strcmp(b.d->title,""))
	    if (!b.c[n]->title) hinzu=0;
	    else if (!mystrstr(b.c[n]->title,b.d->title, cis)) hinzu=0;
	  if (strcmp(b.d->type,""))
	    if (!b.c[n]->type) hinzu=0;
	    else if (!mystrstr(b.c[n]->type,b.d->type, cis)) hinzu=0;
	  if (strcmp(b.d->volume,""))
	    if (!b.c[n]->volume) hinzu=0;
	    else if (!mystrstr(b.c[n]->volume,b.d->volume, cis)) hinzu=0;
	  if (strcmp(b.d->year,""))
	    if (!b.c[n]->year) hinzu=0;
	    else if (!mystrstr(b.c[n]->year,b.d->year, cis)) hinzu=0;
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
      
    } /* endif */
    break;
  case 3:    // ................................. NOT;
    b.ClearDummy();
    b.d->id=0;
    if (c_refform(0,2)) {
      for (n=1; n<=key[0]; n++) {
	hinzu=1;
	if (b.d->id)
	  if (b.c[key[n]]->id == b.d->id) hinzu=0;
	if (strcmp(b.d->doctype,""))
	  if (!b.c[key[n]]->doctype) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->doctype,b.d->doctype, cis)) hinzu=0;
	if (strcmp(b.d->keyword,""))
	  if (!b.c[key[n]]->keyword) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->keyword,b.d->keyword, cis)) hinzu=0;
	if (strcmp(b.d->abstract,""))
	  if (!b.c[key[n]]->abstract) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->abstract,b.d->abstract, cis)) hinzu=0;
	if (strcmp(b.d->address,""))
	  if (!b.c[key[n]]->address) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->address,b.d->address, cis)) hinzu=0;
	if (strcmp(b.d->annote,""))
	  if (!b.c[key[n]]->annote) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->annote,b.d->annote, cis)) hinzu=0;
	if (strcmp(b.d->author,""))
	  if (!b.c[key[n]]->author) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->author,b.d->author, cis)) hinzu=0;
	if (strcmp(b.d->booktitle,""))
	  if (!b.c[key[n]]->booktitle) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->booktitle,b.d->booktitle, cis)) hinzu=0;
	if (strcmp(b.d->chapter,""))
	  if (!b.c[key[n]]->chapter) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->chapter,b.d->chapter, cis)) hinzu=0;
	if (strcmp(b.d->comment,""))
	  if (!b.c[key[n]]->comment) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->comment,b.d->comment, cis)) hinzu=0;
	if (strcmp(b.d->crossref,""))
	  if (!b.c[key[n]]->crossref) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->crossref,b.d->crossref, cis)) hinzu=0;
	if (strcmp(b.d->edition,""))
	  if (!b.c[key[n]]->edition) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->edition,b.d->edition, cis)) hinzu=0;
	if (strcmp(b.d->editor,""))
	  if (!b.c[key[n]]->editor) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->editor,b.d->editor, cis)) hinzu=0;
	if (strcmp(b.d->howpublished,""))
	  if (!b.c[key[n]]->howpublished) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->howpublished,b.d->howpublished, cis)) hinzu=0;
	if (strcmp(b.d->institution,""))
	  if (!b.c[key[n]]->institution) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->institution,b.d->institution, cis)) hinzu=0;
	if (strcmp(b.d->journal,""))
	  if (!b.c[key[n]]->journal) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->journal,b.d->journal, cis)) hinzu=0;
	if (strcmp(b.d->key,""))
	  if (!b.c[key[n]]->key) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->key,b.d->key, cis)) hinzu=0;
	if (strcmp(b.d->month,""))
	  if (!b.c[key[n]]->month) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->month,b.d->month, cis)) hinzu=0;
	if (strcmp(b.d->note,""))
	  if (!b.c[key[n]]->note) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->note,b.d->note, cis)) hinzu=0;
	if (strcmp(b.d->number,""))
	  if (!b.c[key[n]]->number) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->number,b.d->number, cis)) hinzu=0;
	if (strcmp(b.d->organization,""))
	  if (!b.c[key[n]]->organization) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->organization,b.d->organization, cis)) hinzu=0;
	if (strcmp(b.d->pages,""))
	  if (!b.c[key[n]]->pages) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->pages,b.d->pages, cis)) hinzu=0;
	if (strcmp(b.d->publisher,""))
	  if (!b.c[key[n]]->publisher) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->publisher,b.d->publisher, cis)) hinzu=0;
	if (strcmp(b.d->reprint,""))
	  if (!b.c[key[n]]->reprint) hinzu=1;
	  else if (!strcmp(b.c[key[n]]->reprint,b.d->reprint)) hinzu=0;
	if (strcmp(b.d->school,""))
	  if (!b.c[key[n]]->school) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->school,b.d->school, cis)) hinzu=0;
	if (strcmp(b.d->series,""))
	  if (!b.c[key[n]]->series) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->series,b.d->series, cis)) hinzu=0;
	if (strcmp(b.d->title,""))
	  if (!b.c[key[n]]->title) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->title,b.d->title, cis)) hinzu=0;
	if (strcmp(b.d->type,""))
	  if (!b.c[key[n]]->type) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->type,b.d->type, cis)) hinzu=0;
	if (strcmp(b.d->volume,""))
	  if (!b.c[key[n]]->volume) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->volume,b.d->volume, cis)) hinzu=0;
	if (strcmp(b.d->year,""))
	  if (!b.c[key[n]]->year) hinzu=1;
	  else if (mystrstr(b.c[key[n]]->year,b.d->year, cis)) hinzu=0;
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
      
    } /* endif */

    break;
  case 4:    // ................................. view all;
      str_list=(XmStringTable) XtMalloc(b.last*sizeof(XmString));
      key=(unsigned long *)realloc(key,(b.last+1)*sizeof(unsigned long));
      key[0]=b.last;
      for(n=0; n<b.last; n++) {
	key[n+1]=n;
	sprintf(ref,"%u %.15s %.30s %.15s %.4s",
		b.c[n]->id,
		b.c[n]->author,
		b.c[n]->title,
		b.c[n]->journal,
		b.c[n]->year);
	str_list[n]=XmStringCreateLocalized(ref);
      }
      XmListDeleteAllItems(liste);
      XtVaSetValues(liste,
		    XmNitems, str_list,
		    XmNitemCount, b.last,
		    NULL);
      set_message(b.last);
      XtFree((char *)str_list);
    break;
  case 5:    // ................................ case insensitive ?;
    if (cis) cis=0; else cis=1;
    break;
  }
  delete[]gefunden; delete[]ref;
}


void satext_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
  char *text;

  if (text = XmTextGetString(widget)) {
    b.d->keyword=(char *)realloc(b.d->keyword, strlen(text)+1);
    strcpy(b.d->keyword, text);
    XtFree(text);
  }
  
}

void satoggle_cb(Widget widget, XtPointer client_data, XtPointer call_data) {
  if (XmToggleButtonGadgetGetState(widget))
      b.d->id=(unsigned long) client_data;
}
