
#include "xplore.h"
#include "dialogs.h"

#include "callbacks.h"
#include "devmount.h"
#include "ftype.h"
#include "icons.h"
#include "interface.h"
#include "util.h"

#include <Xm/XmAll.h>

/* dialog types */

enum {
  QUESTION, INFO, WARNING, ERRORMSG,
};

/* message strings */

#define CONFIRM_DROP		"Drop %d file(s)\nfrom %s\nonto %s?"
#define CONFIRM_MOVE		"Move %d file(s)\nfrom %s\nto %s?"
#define CONFIRM_COPY		"Copy %d file(s)\nfrom %s\nto %s?"
#define CONFIRM_LINK		"Link %d file(s)\nfrom %s\nto %s?"
#define CONFIRM_DELETE		"Delete %d file(s)\nfrom %s?"
#define CONFIRM_OVERWRITE_DIR	"Directory %s\nalready exists. Overwrite?"
#define CONFIRM_OVERWRITE_FILE	"File %s\nalready exists. Overwrite?"
#define CONFIRM_RENAME_DIR	"Directory %s\nalready exists. Rename?"
#define CONFIRM_RENAME_FILE	"File %s\nalready exists. Rename?"
#define CONFIRM_DELETE_DIR	"Delete directory\n%s\n" \
                                "and all files contained in it?"
#define CONFIRM_DELETE_EMPTYDIR	"Delete directory\n%s?"
#define CONFIRM_DELETE_FILE	"Delete file\n%s?"
#define CONFIRM_QUIT		"Quit xplore?"
#define CONFIRM_NOMATCH_TYPE	"Type \"%s\"\ndoes not match file \"%s\".\n" \
				"Add the type anyway?"
#define CONFIRM_DELETE_TYPE	"Ok to delete type #%d:\n\"%s\"?"
#define MOUNT_RETRY		"Directory %s\ncould not be mounted. Retry?"
#define UMOUNT_RETRY		"Directory %s\ncould not be unmounted. Retry?"
#define MKDIR_MSG		"Make a new directory"
#define MKDIR_PROMPT		"Directory name:"
#define CHDIR_MSG		"Change to directory"
#define CHDIR_PROMPT		"Directory name:"
#define RENAME_MSG		"Rename file"
#define RENAME_PROMPT		"New name:"
#define MOVE_MSG		"Move files"
#define MOVE_PROMPT		"Target:"
#define COPY_MSG		"Copy files"
#define COPY_PROMPT		"Target:"
#define LINK_MSG		"Link files"
#define LINK_PROMPT		"Target:"
#define FILTER_MSG		"Filter files"
#define FILTER_PROMPT		"Pattern:"
#define SELECT_MSG		"Select files"
#define SELECT_PROMPT		"Pattern:"
#define PARAM_MSG		"Enter parameters"
#define PROPS_MSG		"Properties"
#define PROPS_FILE_INFO		"File info"
#define PROPS_PERMS		"Permissions"
#define PROPS_SUMMARY		"%s byte%s in %s selected item%s"
#define PROPS_DIR		"Directory:"
#define PROPS_FILE		"Filename:"
#define PROPS_SIZE		"Size:"
#define PROPS_COMMENT		"Description:"
#define PROPS_DESC		"Type:"
#define PROPS_LINK		"Link to:"
#define PROPS_LAST_ACC		"Last access:"
#define PROPS_LAST_MOD		"Last modification:"
#define PROPS_LAST_CHG		"Last status change:"
#define PROPS_USER		"User:"
#define PROPS_GROUP		"Group:"
#define PROPS_OTHERS		"Others:"
#define BIND_MSG		"File type"
#define BIND_TYPE_INFO		"Type #%d (%s)"
#define BIND_TYPE_NONE		"Type <NONE>"
#define BIND_ICONS		"Icons"
#define BIND_ACTIONS		"Actions"
#define BIND_PATTERNS		"Pattern list:"
#define BIND_OPTIONS		"Options:"
#define BIND_COMMENT		"Description:"
#define BIND_POS		"Priority:"
#define BIND_LARGE_ICON		"Large icon:"
#define BIND_SMALL_ICON		"Small icon:"
#define BIND_ACTION_LIST	"Action list:"
#define SCHEME_MSG		"Select a color scheme"
#define SCHEME_PROMPT		"Color scheme file:"

/* button masks */

#define Ok		(1<<(OK-1))
#define Cancel		(1<<(CANCEL-1))
#define Yes		(1<<(YES-1))
#define No		(1<<(NO-1))
#define Doall		(1<<(DOALL-1))
#define Ignore		(1<<(IGNORE-1))
#define Replace		(1<<(REPLACE-1))
#define Add		(1<<(ADD-1))
#define Remove		(1<<(REMOVE-1))
#define Clear		(1<<(CLEAR-1))
#define Add2		(1<<(ADD2-1))
#define Replace2	(1<<(REPLACE2-1))
#define Delete		(1<<(DELETE-1))

/* field descriptions */

typedef struct _FieldDescRec {
  int id;
  String label, value;
  Boolean editable;
  int line;
  String *buttons;
  unsigned long flags, mask;
  int i, n;
  String *args;
  XtCallbackProc value_changed_cb;
  String directory, pattern;
} FieldDescRec;

enum {
  ID_SEPARATOR, ID_LABEL, ID_CONST_FIELD, ID_EDIT_FIELD, ID_TEXT_FIELD,
  ID_FILE_FIELD, ID_COMBO_FIELD, ID_FLAGS_FIELD, ID_FLAGS_MASK_FIELD,
};

#define SEPARATOR \
  { ID_SEPARATOR, NULL, NULL, False, NONE, NULL, 0L, 0L, 0, 0, NULL, NULL, \
    NULL, NULL }
#define LABEL(label) \
  { ID_LABEL, label, NULL, False, NONE, NULL, 0L, 0L, 0, 0, NULL, NULL, NULL, \
    NULL }
#define CONST_FIELD(label,value) \
  { ID_CONST_FIELD, label, value, False, NONE, NULL, 0L, 0L, 0, 0, NULL, \
    NULL, NULL, NULL }
#define EDIT_FIELD(label,value,cb) \
  { ID_EDIT_FIELD, label, value, True, NONE, NULL, 0L, 0L, 0, 0, NULL, cb, \
    NULL, NULL }
#define TEXT_FIELD(label,value,line,cb) \
  { ID_TEXT_FIELD, label, value, True, line, NULL, 0L, 0L, 0, 0, NULL, cb, \
    NULL, NULL }
#define FILE_FIELD(label,value,cb,dir,pat) \
  { ID_FILE_FIELD, label, value, True, NONE, NULL, 0L, 0L, 0, 0, NULL, cb, \
    dir, pat }
#define COMBO_FIELD(label,cb) \
  { ID_COMBO_FIELD, label, NULL, True, NONE, NULL, 0L, 0L, 0, 0, NULL, cb, \
    NULL, NULL }
#define FLAGS_FIELD(label,buttons,flags,n,cb) \
  { ID_FLAGS_FIELD, label, NULL, True, NONE, buttons, flags, 0L, 0, n, NULL, \
    cb, NULL, NULL }
#define FLAGS_MASK_FIELD(label,buttons,flags,mask,n,cb) \
  { ID_FLAGS_MASK_FIELD, label, NULL, True, NONE, buttons, flags, mask, 0, n, \
    NULL, cb, NULL, NULL }

static FieldDescRec *fields;
static int button_return;
static WidgetList label_widgets, value_widgets, *flag_widgets;

/* generic dialog creation routine */

static void DestroyPixmapCB(Widget w, Pixmap pixmap)
{
  XFreePixmap(display, pixmap);
}

static Widget create_dlg(String name, IconPtr icon, String msg, int type)
{
  XmString message = msg?XmStringCreateLtoR(msg, XmFONTLIST_DEFAULT_TAG):NULL;
  Arg args[3];
  int i = 0;
  Widget dlg;

  XtSetArg(args[i], XmNautoUnmanage, True); i++;
  XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
  if (message) {
    XtSetArg(args[i], XmNmessageString, message); i++;
  }
  switch (type) {
  case QUESTION:
    dlg = XmCreateQuestionDialog(app_shell, name, args, i);
    break;
  case INFO:
    dlg = XmCreateInformationDialog(app_shell, name, args, i);
    break;
  case WARNING:
    dlg = XmCreateWarningDialog(app_shell, name, args, i);
    break;
  case ERRORMSG:
    dlg = XmCreateErrorDialog(app_shell, name, args, i);
    break;
  default:
    /* should not happen */
    fatal("unknown dialog type (create_dlg)");
  }
  if (icon) {
    Pixel background;
    Pixmap pixmap;
    Widget symbol;

    XtVaGetValues(dlg, XmNbackground, &background, NULL);
    pixmap = combine_pixmap(icon, background);
    symbol = XmMessageBoxGetChild(dlg, XmDIALOG_SYMBOL_LABEL);
    XtVaSetValues(symbol, XmNlabelPixmap, pixmap, NULL);
    XtAddCallback(symbol, XmNdestroyCallback, (XtCallbackProc)DestroyPixmapCB,
		  (XtPointer)pixmap);
  }
  return dlg;
}

/* create the buttons */

static Widget button_widgets[N_BUTTONS];
static XmString button_labels[N_BUTTONS];
static String button_strings[N_BUTTONS] = {
  NULL, NULL, NULL, "Yes", "No", "All", "Ignore", "Replace", "Add",
  "Remove", "Clear", "Add", "Replace", "Delete",
};

static void button_cb(Widget w, int button, XtPointer widget_data)
{
  button_return = button;
}

static void create_buttons(Widget dlg, int type, int buttons, int mask)
{
  int i;

  /* when invoked for the first time, initialize the button labels */
  if (button_labels[YES] == NULL)
    for (i = YES; i < N_BUTTONS; i++)
      button_labels[i] = XmStringCreateLocalized(button_strings[i]);
  /* now initialize the requested button widgets */
  /* standard buttons */
  button_widgets[OK] = XmMessageBoxGetChild(dlg, XmDIALOG_OK_BUTTON);
  button_widgets[CANCEL] = XmMessageBoxGetChild(dlg, XmDIALOG_CANCEL_BUTTON);
  XtUnmanageChild(XmMessageBoxGetChild(dlg, XmDIALOG_HELP_BUTTON));
  if (buttons & Ok) {
    XtAddCallback(dlg, XmNokCallback, (XtCallbackProc) button_cb,
		  (XtPointer)OK);
    if (mask & Ok)
      XtSetSensitive(button_widgets[OK], True);
    else
      XtSetSensitive(button_widgets[OK], False);
  } else
    XtUnmanageChild(button_widgets[OK]);
  if (buttons & Cancel) {
    XtAddCallback(dlg, XmNcancelCallback, (XtCallbackProc) button_cb,
		  (XtPointer)CANCEL);
    if (mask & Cancel)
      XtSetSensitive(button_widgets[CANCEL], True);
    else
      XtSetSensitive(button_widgets[CANCEL], False);
  } else
    XtUnmanageChild(button_widgets[CANCEL]);
  /* our own special buttons */
  for (i = YES; i < N_BUTTONS; i++)
    if (buttons & (1<<(i-1))) {
      button_widgets[i] =
	XtVaCreateManagedWidget(button_strings[i], xmPushButtonGadgetClass,
				dlg, XmNlabelString, button_labels[i], NULL);
      XtAddCallback(button_widgets[i], XmNactivateCallback,
		    (XtCallbackProc) button_cb, (XtPointer)i);
      if (mask & (1<<(i-1)))
	XtSetSensitive(button_widgets[i], True);
      else
	XtSetSensitive(button_widgets[i], False);
    } else
      button_widgets[i] = NULL;
  if (!(buttons & Ok))
    for (i = YES; i < N_BUTTONS; i++)
      if (button_widgets[i]) {
	XtVaSetValues(dlg, XmNdefaultButton, button_widgets[i], NULL);
	break;
      }
}

/* handle a nested file section dialog */

static char xpm_pat[MAXPATHLEN+1] = "*.xpm",
  scheme_pat[MAXPATHLEN+1] = "*.scheme";
static int file_button_return;

static void file_button_cb(Widget w, int button, XtPointer widget_data)
{
  file_button_return = button;
}

static void browse_cb(Widget w, int k, XtPointer widget_data)
{
  Arg args[5];
  int i = 0;
  Widget dlg;
  XmString dir_s = NULL, pat_s = NULL;

  XtSetArg(args[i], XmNautoUnmanage, True); i++;
  XtSetArg(args[i], XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL); i++;
  if (fields[k].label) {
    XtSetArg(args[i], XmNtitle, fields[k].label);
    i++;
  }
  if (fields[k].directory) {
    dir_s = XmStringCreateLocalized(fields[k].directory);
    XtSetArg(args[i], XmNdirectory, dir_s);
    i++;
  }
  if (fields[k].pattern) {
    pat_s = XmStringCreateLocalized(fields[k].pattern);
    XtSetArg(args[i], XmNpattern, pat_s);
    i++;
  }
  dlg = XmCreateFileSelectionDialog(app_shell, "file_selection_dlg", args, i);
  if (dir_s) XmStringFree(dir_s);
  if (pat_s) XmStringFree(pat_s);
  XtUnmanageChild(XmFileSelectionBoxGetChild(dlg, XmDIALOG_HELP_BUTTON));
  XtAddCallback(dlg, XmNokCallback, (XtCallbackProc) file_button_cb,
		(XtPointer)OK);
  XtAddCallback(dlg, XmNcancelCallback, (XtCallbackProc) file_button_cb,
		(XtPointer)CANCEL);
  XtManageChild(dlg);
  file_button_return = CANCEL;
  while (XtIsManaged(dlg)) {
    XEvent event;

    XtAppNextEvent(app, &event);
    XtDispatchEvent(&event);
  }
  XtDestroyWidget(dlg);
  XmUpdateDisplay(app_shell);
  if (file_button_return == OK) {
    Widget text_w = XmFileSelectionBoxGetChild(dlg, XmDIALOG_TEXT);
    String text_s = XmTextFieldGetString(text_w);
    Widget filter_w = XmFileSelectionBoxGetChild(dlg, XmDIALOG_FILTER_TEXT);
    String filter_s = XmTextFieldGetString(filter_w);
    
    XmTextFieldSetString(value_widgets[k], text_s);
    dirpart(fields[k].directory, filter_s);
    filepart(fields[k].pattern, filter_s);
    FREE(text_s);
    FREE(filter_s);
  }
}

/* create a collection of dialog fields */

static void create_fields(Widget dlg, FieldDescRec *fld_desc, int n, int init)
{
  if (n > 0) {
    int i;
    Dimension max_wd = 0;
    Widget rowcol =
      XtVaCreateManagedWidget("Fields", xmRowColumnWidgetClass, dlg, NULL);

    label_widgets = (WidgetList)MALLOC(n * sizeof(Widget));
    value_widgets = (WidgetList)MALLOC(n * sizeof(Widget));
    flag_widgets = (WidgetList*)MALLOC(n * sizeof(WidgetList));
    for (i = 0; i < n; i++)
      switch (fld_desc[i].id) {
      case ID_SEPARATOR:
	XtManageChild(XmCreateSeparator(rowcol, "Separator", NULL, 0));
	label_widgets[i] = value_widgets[i] = NULL;
	flag_widgets[i] = NULL;
	break;
      case ID_LABEL:
	if (fld_desc[i].label) {
	  XmString labelstr = XmStringCreateLocalized(fld_desc[i].label);
	  label_widgets[i] =
	    XtVaCreateManagedWidget("Label", xmLabelGadgetClass,
				    rowcol, XmNlabelType, XmSTRING,
				    XmNlabelString, labelstr,
				    NULL);
	  XmStringFree(labelstr);
	} else
	  label_widgets[i] = NULL;
	value_widgets[i] = NULL;
	flag_widgets[i] = NULL;
	break;
      case ID_CONST_FIELD:
      case ID_EDIT_FIELD:
      case ID_TEXT_FIELD:
      case ID_FILE_FIELD:
      case ID_COMBO_FIELD:
      case ID_FLAGS_FIELD:
      case ID_FLAGS_MASK_FIELD:
	if (fld_desc[i].label &&
	    ((fld_desc[i].id == ID_COMBO_FIELD) ?
	     (fld_desc[i].args && fld_desc[i].n && fld_desc[i].i >= 0) :
	     (fld_desc[i].id == ID_FLAGS_FIELD ||
	      fld_desc[i].id == ID_FLAGS_MASK_FIELD) ?
	     (fld_desc[i].buttons && fld_desc[i].n) :
	     (fld_desc[i].value != NULL))) {
	  Widget form = XtVaCreateManagedWidget("Line", xmFormWidgetClass,
						rowcol, NULL);
	  Dimension wd;
	  XmString labelstr = XmStringCreateLocalized(fld_desc[i].label);

	  label_widgets[i] =
	    XtVaCreateManagedWidget("Label", xmLabelGadgetClass,
				    form, XmNlabelType, XmSTRING,
				    XmNlabelString, labelstr,
				    XmNtopAttachment, XmATTACH_FORM,
				    XmNbottomAttachment, XmATTACH_FORM,
				    XmNleftAttachment, XmATTACH_FORM,
				    NULL);
	  XmStringFree(labelstr);
	  XtVaGetValues(label_widgets[i], XmNwidth, &wd, NULL);
	  if (wd > max_wd) max_wd = wd;
	  if (i == init)
	    XtVaSetValues(rowcol, XmNinitialFocus, form, NULL);
	  if (fld_desc[i].id == ID_EDIT_FIELD ||
	      fld_desc[i].id == ID_FILE_FIELD) {
	    value_widgets[i] =
	      XtVaCreateManagedWidget("Input", xmTextFieldWidgetClass, form,
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_WIDGET,
				      XmNleftWidget, label_widgets[i], NULL);
	    XmTextFieldSetString(value_widgets[i], fld_desc[i].value);
	    if (!fld_desc[i].editable)
	      XtVaSetValues(value_widgets[i], XmNeditable, False, NULL);
	    if (fld_desc[i].value_changed_cb)
	      XtAddCallback(value_widgets[i], XmNvalueChangedCallback,
			    fld_desc[i].value_changed_cb, (XtPointer)i);
	    if (fld_desc[i].id == ID_FILE_FIELD) {
	      Widget browse_button =
		XtVaCreateManagedWidget("Browse", xmPushButtonGadgetClass,
					form,
					XmNlabelString,
					XmStringCreateLocalized("Browse..."),
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_WIDGET,
					XmNleftWidget, value_widgets[i],
					NULL);
	      XtAddCallback(browse_button, XmNactivateCallback,
			    (XtCallbackProc) browse_cb, (XtPointer)i);
	      XtVaSetValues(form, XmNinitialFocus, value_widgets[i], NULL);
	    }
	    flag_widgets[i] = NULL;
	  } else if (fld_desc[i].id == ID_TEXT_FIELD) {
	    value_widgets[i] =
	      XmCreateScrolledText(form, "Text", NULL, 0);
	    XtVaSetValues(XtParent(value_widgets[i]),
			  XmNtopAttachment, XmATTACH_FORM,
			  XmNbottomAttachment, XmATTACH_FORM,
			  XmNleftAttachment, XmATTACH_WIDGET,
			  XmNleftWidget, label_widgets[i],
			  NULL);
	    XtManageChild(value_widgets[i]);
	    XmTextSetString(value_widgets[i], fld_desc[i].value);
	    if (!fld_desc[i].editable)
	      XtVaSetValues(value_widgets[i], XmNeditable, False, NULL);
	    if (fld_desc[i].line) {
	      int line = fld_desc[i].line-1;
	      XmTextPosition pos = 0;
	      while (fld_desc[i].value[pos] && line > 0)
		if (fld_desc[i].value[pos++] == '\n')
		  line--;
	      XmTextSetInsertionPosition(value_widgets[i], pos);
	      XmTextShowPosition(value_widgets[i], pos);
	    }
	    if (fld_desc[i].value_changed_cb)
	      XtAddCallback(value_widgets[i], XmNvalueChangedCallback,
			    fld_desc[i].value_changed_cb, (XtPointer)i);
	    flag_widgets[i] = NULL;
	  } else if (fld_desc[i].id == ID_CONST_FIELD) {
	    labelstr = XmStringCreateLocalized(fld_desc[i].value);
	    value_widgets[i] =
	      XtVaCreateManagedWidget("Value", xmLabelGadgetClass,
				      form, XmNlabelType, XmSTRING,
				      XmNlabelString, labelstr,
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_WIDGET,
				      XmNleftWidget, label_widgets[i], NULL);
	    XmStringFree(labelstr);
	    flag_widgets[i] = NULL;
	  } else if (fld_desc[i].id == ID_COMBO_FIELD) {
	    int j, count;
	    XmString *item = alloca(fld_desc[i].n * sizeof(XmString));

	    for (j = 0; j < fld_desc[i].n; j++)
	      item[j] = XmStringCreateLocalized(fld_desc[i].args[j]);
	    value_widgets[i] =
	      XtVaCreateManagedWidget("ComboBox", xmComboBoxWidgetClass, form,
				      XmNtopAttachment, XmATTACH_FORM,
				      XmNbottomAttachment, XmATTACH_FORM,
				      XmNleftAttachment, XmATTACH_WIDGET,
				      XmNleftWidget, label_widgets[i],
				      XmNcomboBoxType, XmDROP_DOWN_LIST,
				      XmNitems, item,
				      XmNitemCount, fld_desc[i].n,
				      XmNselectedPosition, fld_desc[i].i+1,
#if XmREVISION > 0
				      XmNpositionMode, XmONE_BASED,
#endif
				      NULL);
	    XtVaGetValues(XtNameToWidget(value_widgets[i], "*List"),
			  XmNvisibleItemCount, &count, NULL);
	    if (count > fld_desc[i].n) count = fld_desc[i].n;
	    XtVaSetValues(XtNameToWidget(value_widgets[i], "*List"),
			  XmNvisibleItemCount, count, NULL);
	    if (fld_desc[i].value_changed_cb)
	      XtAddCallback(XtNameToWidget(value_widgets[i],"*Text"),
			    XmNvalueChangedCallback,
			    fld_desc[i].value_changed_cb, (XtPointer)i);
	    for (j = 0; j < fld_desc[i].n; j++)
	      XmStringFree(item[j]);
	    flag_widgets[i] = NULL;
	  } else {
	    Widget prev = label_widgets[i];
	    int j;

	    flag_widgets[i] =
	      (WidgetList) MALLOC(fld_desc[i].n * sizeof(Widget));
	    for (j = 0; j < fld_desc[i].n; j++) {
	      XmString labelstr =
		XmStringCreateLocalized(fld_desc[i].buttons[j]);

	      flag_widgets[i][j] =
		XtVaCreateManagedWidget("Button", xmToggleButtonGadgetClass,
					form,
					XmNlabelString, labelstr,
					XmNtoggleMode,
					(fld_desc[i].id == ID_FLAGS_FIELD)?
					XmTOGGLE_BOOLEAN:
					XmTOGGLE_INDETERMINATE,
					XmNindicatorType, XmN_OF_MANY,
					XmNset,
					(!(fld_desc[i].mask & (1L<<j)) &&
					 (fld_desc[i].id ==
					  ID_FLAGS_MASK_FIELD))?
					XmINDETERMINATE:
					(fld_desc[i].flags & (1L<<j))?
					XmSET:XmUNSET,
					XmNtopAttachment, XmATTACH_FORM,
					XmNbottomAttachment, XmATTACH_FORM,
					XmNleftAttachment, XmATTACH_WIDGET,
					XmNleftWidget, prev,
 					NULL);
	      if (fld_desc[i].value_changed_cb)
		XtAddCallback(flag_widgets[i][j], XmNvalueChangedCallback,
			      fld_desc[i].value_changed_cb, (XtPointer)i);
	      XmStringFree(labelstr);
	      prev = flag_widgets[i][j];
	    }
	    value_widgets[i] = NULL;
	  }
	} else {
	  label_widgets[i] = value_widgets[i] = NULL;
	  flag_widgets[i] = NULL;
	}
	break;
      default:
	/* shouldn't happen */
	fatal("invalid dialog description");
      }
    for (i = 0; i < n; i++)
      if (value_widgets[i] || flag_widgets[i])
	XtVaSetValues(label_widgets[i], XmNwidth, max_wd, NULL);
    XtVaSetValues(dlg, XmNinitialFocus, rowcol, NULL);
  } else {
    label_widgets = value_widgets = NULL;
    flag_widgets = NULL;
  }
}

static void get_fields(FieldDescRec *fld_desc, int n, int maxlen)
{
  int i;

  for (i = 0; i < n; i++)
    if (fld_desc[i].id == ID_EDIT_FIELD || fld_desc[i].id == ID_FILE_FIELD ||
	fld_desc[i].id == ID_TEXT_FIELD) {
      if (fld_desc[i].value && value_widgets[i]) {
	String s = ((fld_desc[i].id == ID_TEXT_FIELD)?XmTextGetString:
		    XmTextFieldGetString)(value_widgets[i]);
      
	strncpy(fld_desc[i].value, s, maxlen);
	fld_desc[i].value[maxlen-1] = '\0';
	if (fld_desc[i].id == ID_FILE_FIELD) {
	  int l = strlen(fld_desc[i].value);
	  if (l > 1 && fld_desc[i].value[l-1] == '/')
	    fld_desc[i].value[l-1] = '\0';
	}
	FREE(s);
      }
    } else if (fld_desc[i].id == ID_COMBO_FIELD) {
      int pos;
      XtVaGetValues(value_widgets[i], XmNselectedPosition, &pos, NULL);
      fld_desc[i].i = pos-1;
    } else if (fld_desc[i].id == ID_FLAGS_FIELD) {
      int j;

      for (j = 0; j < fld_desc[i].n; j++) {
	unsigned char set;

	XtVaGetValues(flag_widgets[i][j], XmNset, &set, NULL);
	if (set == XmSET)
	  fld_desc[i].flags |= (1L<<j);
	else
	  fld_desc[i].flags &= ~(1L<<j);
      }
    } else if (fld_desc[i].id == ID_FLAGS_MASK_FIELD) {
      int j;

      for (j = 0; j < fld_desc[i].n; j++) {
	unsigned char set;

	XtVaGetValues(flag_widgets[i][j], XmNset, &set, NULL);
	fld_desc[i].mask |= (1L<<j);
	if (set == XmSET)
	  fld_desc[i].flags |= (1L<<j);
	else if (set == XmUNSET)
	  fld_desc[i].flags &= ~(1L<<j);
	else
	  fld_desc[i].mask &= ~(1L<<j);
      }
    }
}

/* process a dialog */

static void process_dlg(Widget dlg)
{
  XtManageChild(dlg);
  if (!XtIsRealized(app_shell)) {
    Position x, y;
    Dimension disp_w, disp_h, dlg_w, dlg_h;
    /* make the dialog appear in the middle of the screen */
    disp_w = DisplayWidth(display, screen_no);
    disp_h = DisplayHeight(display, screen_no);
    XtVaGetValues(dlg, XmNwidth, &dlg_w, XmNheight, &dlg_h, NULL);
    x = (disp_w - dlg_w)/2;
    y = (disp_h - dlg_h)/2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    XtVaSetValues(dlg,
		  XmNx, x, XmNy, y,
		  NULL);
  }
  while (XtIsManaged(dlg)) {
    XEvent event;

    XtAppNextEvent(app, &event);
    XtDispatchEvent(&event);
  }
}

/* create and manage a dialog box */

static int dlg(String name, IconPtr icon, String msg,
	       FieldDescRec *fld_desc, int n, int init, int maxlen,
	       int type, int buttons, int mask, int default_button)
{
  Boolean SaveWait = WaitFlag;
  Widget dlg = create_dlg(name, icon, msg, type);
  int i;

  create_buttons(dlg, type, buttons, mask);
  create_fields(dlg, fld_desc, n, init);
  button_return = default_button;
  fields = fld_desc;
  /* make sure we don't get an update during processing of the dialog */
  lock = True;
  Done();
  process_dlg(dlg);
  if (SaveWait) Wait();
  if (button_return != CANCEL)
    get_fields(fld_desc, n, maxlen);
  FREE(label_widgets);
  FREE(value_widgets);
  if (flag_widgets)
    for (i = 0; i < n; i++)
      FREE(flag_widgets[i]);
  FREE(flag_widgets);
  XtDestroyWidget(dlg);
  XmUpdateDisplay(app_shell);
  lock = False;
  return button_return;
}

#define msg_dlg(name,icon,msg,type,buttons,mask,button) \
  dlg(name, icon, msg, NULL, 0, NONE, 0, type, buttons, mask, button)
#define input_dlg(name,icon,msg,fld_desc,n,init,maxlen,buttons,mask,button) \
  dlg(name, icon, msg, fld_desc, n, init, maxlen, QUESTION, buttons, mask, \
      button)

/* dialog descriptions */

FieldDescRec mkdir_fld_desc[] = {
  FILE_FIELD(MKDIR_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec chdir_fld_desc[] = {
  FILE_FIELD(CHDIR_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec rename_fld_desc[] = {
  FILE_FIELD(RENAME_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec move_fld_desc[] = {
  FILE_FIELD(MOVE_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec copy_fld_desc[] = {
  FILE_FIELD(COPY_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec link_fld_desc[] = {
  FILE_FIELD(LINK_PROMPT, NULL, NULL, NULL, NULL),
};

FieldDescRec filter_fld_desc[] = {
  EDIT_FIELD(FILTER_PROMPT, NULL, NULL),
};

FieldDescRec select_fld_desc[] = {
  EDIT_FIELD(SELECT_PROMPT, NULL, NULL),
};

String user_perm_buttons[] = { "read", "write", "execute", "set uid" };
String group_perm_buttons[] = { "read", "write", "execute", "set gid" };
String other_perm_buttons[] = { "read", "write", "execute", "sticky" };

FieldDescRec props_fld_desc[] = {
  SEPARATOR,
  LABEL(PROPS_FILE_INFO),
  SEPARATOR,
  CONST_FIELD(PROPS_DIR, NULL),
  CONST_FIELD(PROPS_FILE, NULL),
  CONST_FIELD(PROPS_SIZE, NULL),
  CONST_FIELD(PROPS_DESC, NULL),
  CONST_FIELD(PROPS_LINK, NULL),
  CONST_FIELD(PROPS_COMMENT, NULL),
  CONST_FIELD(PROPS_LAST_ACC, NULL),
  CONST_FIELD(PROPS_LAST_MOD, NULL),
  CONST_FIELD(PROPS_LAST_CHG, NULL),
  EDIT_FIELD(PROPS_USER, NULL, NULL),
  EDIT_FIELD(PROPS_GROUP, NULL, NULL),
  SEPARATOR,
  LABEL(PROPS_PERMS),
  SEPARATOR,
  FLAGS_MASK_FIELD(PROPS_USER, user_perm_buttons, 0L, 0L,
		   XtNumber(user_perm_buttons), NULL),
  FLAGS_MASK_FIELD(PROPS_GROUP, group_perm_buttons, 0L, 0L,
		   XtNumber(group_perm_buttons), NULL),
  FLAGS_MASK_FIELD(PROPS_OTHERS, other_perm_buttons, 0L, 0L,
		   XtNumber(other_perm_buttons), NULL),
};

String bind_option_buttons[] = { "Magic", "Exclusive (this file only)" };

static int bind_maxlen;
static Boolean bind_edited = False, bind_use_default = False;
static String bind_other_pattern;

extern FieldDescRec bind_fld_desc[];

static void bind_value_changed_cb(Widget w,
				  int i,
				  XtPointer widget_data)
{
  Boolean changed = False;

  /* check whether "Exclusive" option has been switched */
  if (i == 4) {
    unsigned char set;
    XtVaGetValues(flag_widgets[i][1], XmNset, &set, NULL);
    if (bind_use_default && set != XmSET ||
	!bind_use_default && set == XmSET) {
      String s = XmTextFieldGetString(value_widgets[3]);
      String t = bind_other_pattern;
      strncpy(bind_fld_desc[3].value, s, bind_maxlen);
      bind_fld_desc[3].value[bind_maxlen-1] = '\0';
      if (strcmp(s, t))
	XmTextFieldSetString(value_widgets[3], bind_other_pattern);
      bind_other_pattern = bind_fld_desc[3].value;
      bind_fld_desc[3].value = t;
      FREE(s);
      bind_use_default = (set == XmSET);
      if (bind_use_default)
	XtVaSetValues(value_widgets[3], XmNeditable, False, NULL);
      else
	XtVaSetValues(value_widgets[3], XmNeditable, True, NULL);
    } else
      changed = True;
  } else
    changed = True;
  if (!bind_edited && changed) {
    /* disable Delete button when user edits the type */
    if (button_widgets[DELETE] && XtIsManaged(button_widgets[DELETE]) &&
	XtIsSensitive(button_widgets[DELETE]))
      XtSetSensitive(button_widgets[DELETE], False);
  }
  bind_edited = bind_edited || changed;
}

FieldDescRec bind_fld_desc[] = {
  SEPARATOR,
  LABEL(NULL),
  SEPARATOR,
  EDIT_FIELD(BIND_PATTERNS, NULL, (XtCallbackProc)bind_value_changed_cb),
  FLAGS_FIELD(BIND_OPTIONS, bind_option_buttons, 0L,
	      XtNumber(bind_option_buttons),
	      (XtCallbackProc)bind_value_changed_cb),
  EDIT_FIELD(BIND_COMMENT, NULL, (XtCallbackProc)bind_value_changed_cb),
  COMBO_FIELD(BIND_POS, (XtCallbackProc)bind_value_changed_cb),
  SEPARATOR,
  LABEL(BIND_ICONS),
  SEPARATOR,
  FILE_FIELD(BIND_LARGE_ICON, NULL, (XtCallbackProc)bind_value_changed_cb,
	     libicons, xpm_pat),
  FILE_FIELD(BIND_SMALL_ICON, NULL, (XtCallbackProc)bind_value_changed_cb,
	     libicons, xpm_pat),
  SEPARATOR,
  LABEL(BIND_ACTIONS),
  SEPARATOR,
  TEXT_FIELD(BIND_ACTION_LIST, NULL, NONE,
	     (XtCallbackProc)bind_value_changed_cb),
};

FieldDescRec scheme_fld_desc[] = {
  FILE_FIELD(SCHEME_PROMPT, NULL, NULL, libschemes, scheme_pat),
};

static char s[3000];

Boolean force_overwrt = False, force_rename = False, force_delete = False,
  force_deldir = False, force_delfile = False;

int confirm_drop_dlg(IconPtr icon, int nFiles, String dirname, String target)
{
  if (confirm_drop) {
    sprintf(s, CONFIRM_DROP, nFiles, dirname, target);
    return msg_dlg("confirm_drop_dlg", icon, s, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  } else
    return OK;
}

int confirm_move_dlg(IconPtr icon, int nFiles, String dirname, String target)
{
  if (confirm_move) {
    sprintf(s, CONFIRM_MOVE, nFiles, dirname, target);
    return msg_dlg("confirm_move_dlg", icon, s, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  } else
    return OK;
}

int confirm_copy_dlg(IconPtr icon, int nFiles, String dirname, String target)
{
  if (confirm_copy) {
    sprintf(s, CONFIRM_COPY, nFiles, dirname, target);
    return msg_dlg("confirm_copy_dlg", icon, s, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  } else
    return OK;
}

int confirm_link_dlg(IconPtr icon, int nFiles, String dirname, String target)
{
  if (confirm_link) {
    sprintf(s, CONFIRM_LINK, nFiles, dirname, target);
    return msg_dlg("confirm_link_dlg", icon, s, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  } else
    return OK;
}

int confirm_delete_dlg(IconPtr icon, int nFiles, String dirname)
{
  if (confirm_delete) {
    sprintf(s, CONFIRM_DELETE, nFiles, dirname);
    return msg_dlg("confirm_delete_dlg", icon, s, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  } else
    return OK;
}

int confirm_overwrite_dir_dlg(IconPtr icon, String target)
{
  if (confirm_overwrt && !force_overwrt) {
    sprintf(s, CONFIRM_OVERWRITE_DIR, target);
    return msg_dlg("confirm_overwrite_dir_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

int confirm_overwrite_file_dlg(IconPtr icon, String target)
{
  if (confirm_overwrt && !force_overwrt) {
    sprintf(s, CONFIRM_OVERWRITE_FILE, target);
    return msg_dlg("confirm_overwrite_file_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

int confirm_rename_dir_dlg(IconPtr icon, String target)
{
  if (confirm_overwrt && !force_overwrt) {
    sprintf(s, CONFIRM_RENAME_DIR, target);
    return msg_dlg("confirm_rename_dir_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

int confirm_rename_file_dlg(IconPtr icon, String target)
{
  if (confirm_overwrt && !force_overwrt) {
    sprintf(s, CONFIRM_RENAME_FILE, target);
    return msg_dlg("confirm_rename_file_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

static Boolean emptydir(char *name)
{
  DIR *dir;
  struct dirent *entry;
  if (!(dir = opendir(name)))
    return False;
  while ((entry = readdir(dir)) &&
	 (!strcmp(entry->d_name, ".") ||
	  !strcmp(entry->d_name, "..")))
    ;
  closedir(dir);
  return entry == NULL;
}

int confirm_delete_dir_dlg(IconPtr icon, String name)
{
  if (confirm_deldir && !force_deldir) {
    sprintf(s, emptydir(name)?CONFIRM_DELETE_EMPTYDIR:CONFIRM_DELETE_DIR,
	    name);
    return msg_dlg("confirm_delete_dir_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

int confirm_delete_file_dlg(IconPtr icon, String name)
{
  if (confirm_delfile && !force_delfile) {
    sprintf(s, CONFIRM_DELETE_FILE, name);
    return msg_dlg("confirm_delete_file_dlg", icon, s, WARNING,
		   Yes|No|Doall|Cancel, Yes|No|Doall|Cancel,
		   CANCEL);
  } else
    return YES;
}

int confirm_quit_dlg(IconPtr icon)
{
  if (confirm_quit)
    return msg_dlg("confirm_quit_dlg", icon, CONFIRM_QUIT, QUESTION,
		   Ok|Cancel, Ok|Cancel,
		   CANCEL);
  else
    return OK;
}

int confirm_nomatch_type_dlg(IconPtr icon, String patterns, String name)
{
  sprintf(s, CONFIRM_NOMATCH_TYPE, patterns, name);
  return msg_dlg("confirm_nomatch_type_dlg", icon, s, WARNING,
		 Ok|Cancel, Ok|Cancel, CANCEL);
}

int confirm_delete_type_dlg(IconPtr icon, int type, String patterns)
{
  sprintf(s, CONFIRM_DELETE_TYPE, type, patterns);
  return msg_dlg("confirm_delete_type_dlg", icon, s, WARNING,
		 Ok|Cancel, Ok|Cancel, CANCEL);
}

int scheme_dlg(IconPtr icon, String s, int maxlen)
{
  scheme_fld_desc[0].value = s;
  return input_dlg("scheme_dlg", icon, SCHEME_MSG,
		   scheme_fld_desc, XtNumber(scheme_fld_desc), NONE, maxlen,
		   Ok|Clear|Cancel, Ok|Clear|Cancel, CANCEL);
}

static int mount_dlg1(IconPtr icon, String name)
{
  sprintf(s, MOUNT_RETRY, name);
  return msg_dlg("mount_dlg", icon, s, QUESTION,
		 Ok|Ignore|Cancel, Ok|Ignore|Cancel,
		 CANCEL);
}

int mount_dlg(IconPtr icon, String name, Boolean verify)
{
  if (!check || !verify)
    return devmount(name);
  else {
    int d;
    while ((d = devmount(name)) == NONE)
      switch (mount_dlg1(icon, name)) {
      case OK:
	break;
      case IGNORE:
	d = devmount_ignore(name);
      case CANCEL:
	return d;
      }
    return d;
  }
}

static int umount_dlg1(IconPtr icon, int d)
{
  sprintf(s, UMOUNT_RETRY, devnam(d));
  return msg_dlg("umount_dlg", icon, s, QUESTION,
		 Ok|Ignore, Ok|Ignore,
		 IGNORE);
}

void umount_dlg(IconPtr icon, int d)
{
  while (!devumount(d))
    switch (umount_dlg1(icon, d)) {
    case OK:
      break;
    case IGNORE:
      devumount_ignore(d);
      return;
    }
  return;
}

int mkdir_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  mkdir_fld_desc[0].directory = dir;
  mkdir_fld_desc[0].pattern = pat;
  mkdir_fld_desc[0].value = s;
  return input_dlg("mkdir_dlg", icon, MKDIR_MSG,
		   mkdir_fld_desc, XtNumber(mkdir_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

int chdir_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  chdir_fld_desc[0].directory = dir;
  chdir_fld_desc[0].pattern = pat;
  chdir_fld_desc[0].value = s;
  return input_dlg("chdir_dlg", icon, CHDIR_MSG,
		   chdir_fld_desc, XtNumber(chdir_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

int rename_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  rename_fld_desc[0].directory = dir;
  rename_fld_desc[0].pattern = pat;
  rename_fld_desc[0].value = s;
  return input_dlg("rename_dlg", icon, RENAME_MSG,
		   rename_fld_desc, XtNumber(rename_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

int move_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  move_fld_desc[0].directory = dir;
  move_fld_desc[0].pattern = pat;
  move_fld_desc[0].value = s;
  return input_dlg("move_dlg", icon, MOVE_MSG,
		   move_fld_desc, XtNumber(move_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

int copy_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  copy_fld_desc[0].directory = dir;
  copy_fld_desc[0].pattern = pat;
  copy_fld_desc[0].value = s;
  return input_dlg("copy_dlg", icon, COPY_MSG,
		   copy_fld_desc, XtNumber(copy_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

int link_dlg(IconPtr icon, String d, String s, int maxlen)
{
  char dir[MAXPATHLEN+1] = "", pat[MAXPATHLEN+1] = "*";
  if (d) strcpy(dir, d);
  link_fld_desc[0].directory = dir;
  link_fld_desc[0].pattern = pat;
  link_fld_desc[0].value = s;
  return input_dlg("link_dlg", icon, LINK_MSG,
		   link_fld_desc, XtNumber(link_fld_desc), NONE, maxlen,
		   Ok|Cancel, Ok|Cancel, CANCEL);
}

/* unfortunately, the rwx permission flags in the stat structure are just the
   wrong way round; they are reversed with the following routine */

static unsigned long perm_flags(unsigned long flags)
{
  return ((flags & 1) << 2) | (flags & 2) | ((flags & 4) >> 2);
}

#define S_ISPECIAL (S_ISUID|S_ISGID|S_ISVTX)

int props_dlg(IconPtr icon, int nFiles, int nBytes,
	      String dirname, String filename, String linkname,
	      String desc, String comment,
	      String last_acc, String last_mod, String last_chg,
	      String user, String group, int maxlen,
	      mode_t *mode, mode_t *mask)
{
  int ret;
  mode_t mode1;
  char *summary, size[10], num[10];

  sprintf(size, "%d", nBytes);
  sprintf(num, "%d", nFiles);
  summary = alloca(strlen(PROPS_SUMMARY)+strlen(size)+strlen(num)+1);
  sprintf(summary, PROPS_SUMMARY, size, (nBytes==1)?"":"s", num,
	  (nFiles==1)?"":"s");
  props_fld_desc[3].value = dirname;
  props_fld_desc[4].value = filename;
  props_fld_desc[5].value = summary;
  props_fld_desc[6].value = desc;
  props_fld_desc[7].value = linkname;
  props_fld_desc[8].value = comment;
  props_fld_desc[9].value = last_acc;
  props_fld_desc[10].value = last_mod;
  props_fld_desc[11].value = last_chg;
  props_fld_desc[12].value = user;
  props_fld_desc[13].value = group;
  props_fld_desc[17].flags = perm_flags((((unsigned long)*mode) &
					S_IRWXU)>>6) |
    ((((unsigned long)*mode & S_ISUID)>>11)<<3);
  props_fld_desc[18].flags = perm_flags((((unsigned long)*mode) &
					 S_IRWXG)>>3) |
    ((((unsigned long)*mode & S_ISGID)>>10)<<3);
  props_fld_desc[19].flags = perm_flags((((unsigned long)*mode) &
					 S_IRWXO)>>0) |
    ((((unsigned long)*mode & S_ISVTX)>>9)<<3);
  props_fld_desc[17].mask = perm_flags((((unsigned long)*mask) &
				       S_IRWXU)>>6) |
    ((((unsigned long)*mask & S_ISUID)>>11)<<3);
  props_fld_desc[18].mask = perm_flags((((unsigned long)*mask) &
					S_IRWXG)>>3) |
    ((((unsigned long)*mask & S_ISGID)>>10)<<3);
  props_fld_desc[19].mask = perm_flags((((unsigned long)*mask) &
					S_IRWXO)>>0) |
    ((((unsigned long)*mask & S_ISVTX)>>9)<<3);
  ret = input_dlg("props_dlg", icon, PROPS_MSG,
		  props_fld_desc, XtNumber(props_fld_desc), NONE, maxlen,
		  Ok|Cancel, Ok|Cancel, CANCEL);
  mode1 = (mode_t) ((perm_flags(props_fld_desc[17].flags)<<6) |
		     (perm_flags(props_fld_desc[18].flags)<<3) |
		     (perm_flags(props_fld_desc[19].flags)<<0) |
		     (((props_fld_desc[17].flags & 8)>>3)<<11) |
		     (((props_fld_desc[18].flags & 8)>>3)<<10) |
		     (((props_fld_desc[19].flags & 8)>>3)<<9));
  *mask = (mode_t) ((perm_flags(props_fld_desc[17].mask)<<6) |
		     (perm_flags(props_fld_desc[18].mask)<<3) |
		     (perm_flags(props_fld_desc[19].mask)<<0) |
		     (((props_fld_desc[17].mask & 8)>>3)<<11) |
		     (((props_fld_desc[18].mask & 8)>>3)<<10) |
		     (((props_fld_desc[19].mask & 8)>>3)<<9));
  *mode = (*mode & ~(*mask)) | (mode1 & (*mask));
  return ret;
}

int bind_dlg(IconPtr icon, int type, int errline, String patterns,
	     String deflt, Boolean *use_default, Boolean *magic,
	     String comment, String large_icon,
	     String small_icon, String actions,
	     String *types, int ntypes, int *pos,
	     int maxlen,
	     Boolean *edited)
{
  int ret, mask;
  char type_info[100];

  if (type != NONE)
    sprintf(type_info, BIND_TYPE_INFO, type,
	    type == 0 ? "default type" :
	    tmp_flag(type) ? "user-defined" : "predefined");
  else
    sprintf(type_info, BIND_TYPE_NONE);
  bind_fld_desc[1].label = type_info;
  if (!*use_default) {
    bind_use_default = False;
    bind_fld_desc[3].editable = True;
    bind_fld_desc[3].value = patterns;
    bind_fld_desc[4].flags = (*magic)?1:0;
    bind_other_pattern = deflt;
  } else {
    bind_use_default = True;
    bind_fld_desc[3].editable = False;
    bind_fld_desc[3].value = deflt;
    bind_fld_desc[4].flags = 2|((*magic)?1:0);
    bind_other_pattern = patterns;
  }
  bind_fld_desc[5].value = comment;
  bind_fld_desc[6].args = types;
  bind_fld_desc[6].n = ntypes;
  bind_fld_desc[6].i = *pos;
  bind_fld_desc[10].value = large_icon;
  bind_fld_desc[11].value = small_icon;
  bind_fld_desc[15].value = actions;
  bind_fld_desc[15].line = errline;
  bind_edited = False;
  bind_maxlen = maxlen;
  mask = Add2|Cancel;
  if (tmp_flag(type)) mask |= Replace2|Delete;
  if (*edited) mask &= ~Delete;
  ret = input_dlg("bind_dlg", icon, BIND_MSG,
		  bind_fld_desc, XtNumber(bind_fld_desc), 
		  errline?15:NONE, maxlen, Add2|Replace2|Delete|Cancel,
		  mask, CANCEL);
  if (bind_fld_desc[4].flags)
    *magic = True;
  else
    *magic = False;
  if (!bind_edited) bind_edited = *pos != bind_fld_desc[6].i;
  *pos = bind_fld_desc[6].i;
  *edited = bind_edited;
  *use_default = bind_use_default;
  return ret;
}

int filter_dlg(IconPtr icon, String s, int maxlen)
{
  filter_fld_desc[0].value = s;
  return input_dlg("filter_dlg", icon, FILTER_MSG,
		   filter_fld_desc, XtNumber(filter_fld_desc), NONE, maxlen,
		   Ok|Clear|Cancel, Ok|Clear|Cancel, CANCEL);
}

int select_dlg(IconPtr icon, String s, int maxlen)
{
  select_fld_desc[0].value = s;
  return input_dlg("select_dlg", icon, SELECT_MSG,
		   select_fld_desc, XtNumber(select_fld_desc), NONE, maxlen,
		   Replace|Add|Remove|Cancel, Replace|Add|Remove|Cancel,
		   CANCEL);
}

/* this comes from xfm/moxfm -- thanks to Brian King for his default value
   patch */

#define kDefaultValueMarker "--" /* Marker to denote default value */
#define kDefaultValue ""         /* Default Value to use if none specified */

int param_dlg(IconPtr icon, String msg, String action, String buf, int bufsz)
{
  char *act1 = (char *)alloca(strlen(action)+1), *s, *t;
  char *str = (char *)alloca(strlen(action)+1);
  char **acts = NULL, **vars = NULL;
  int n_acts = 0, n_vars = 0;
  char *def_val;
  char **vals = NULL;

  if (!action) return CANCEL;

  strcpy(act1, action);

  for (s = split2(act1, '%'); s; s = split2(NULL, '%')) {
    acts = (char **)REALLOC(acts, (n_acts+1)*sizeof(char *));
    acts[n_acts++] = XtNewString(unquote(str, s));
    if ((t = split2(NULL, '%'))) {
      vars = (char **)REALLOC(vars, (n_vars+1)*sizeof(char *));
      vals = (char **)REALLOC(vals, (n_vars+1)*sizeof(char *));
      if (!strcmp(t, "--")) {
	/* separator */
	vars[n_vars] = NULL;
	vals[n_vars++] = NULL;
      } else if (*t == '-') {
	/* label */
	vars[n_vars] = XtNewString(unquote(str, t+1));
	vals[n_vars++] = NULL;
      } else {
	vars[n_vars] = XtNewString(unquote(str, t));
	vals[n_vars] = (char *)MALLOC(bufsz+1);
	/* check string for default value character */
	if ((def_val = strstr(vars[n_vars], kDefaultValueMarker)) == NULL) {
	  strcpy(vals[n_vars++], kDefaultValue);
	} else {
	  def_val[0] = '\0'; /* Separate label and default value */
	  strcpy(vals[n_vars++], def_val + strlen(kDefaultValueMarker));
	}
      }
    } else
      break;
  }

  if (n_vars) {
    FieldDescRec *param_fld_desc =
      (FieldDescRec*) MALLOC(n_vars * sizeof(FieldDescRec));
    int i, ret;

    for (i = 0; i < n_vars; i++) {
      param_fld_desc[i].id = vars[i]?(vals[i]?ID_EDIT_FIELD:ID_LABEL):
	ID_SEPARATOR;
      param_fld_desc[i].label = vars[i];
      param_fld_desc[i].value = vals[i];
      param_fld_desc[i].editable = vars[i] != NULL;
      param_fld_desc[i].line = 0;
      param_fld_desc[i].buttons = NULL;
      param_fld_desc[i].flags = param_fld_desc[i].mask = 0L;
      param_fld_desc[i].n = 0;
      param_fld_desc[i].value_changed_cb = NULL;
    }
    ret  = input_dlg("param_dlg", icon, msg?msg:PARAM_MSG, param_fld_desc,
		     n_vars, NONE, bufsz, Ok|Cancel, Ok|Cancel, CANCEL);
    FREE(param_fld_desc);
    if (ret == OK) {
      int i, l;
      for (l = i = 0; i < n_acts; i++) {
	int l1 = strlen(acts[i]), l2 = (i<n_vars && vals[i])?strlen(vals[i]):0;
	if (l+l1+l2 >= bufsz) {
	  ret = ERROR;
	  break;
	}
	strcpy(buf+l, acts[i]);
	if (l2) strcpy(buf+l+l1, vals[i]);
	l += l1+l2;
      }
      for (i = 0; i < n_acts; i++)
	FREE(acts[i]);
      for (i = 0; i < n_vars; i++)
	if (vars[i]) FREE(vars[i]);
      for (i = 0; i < n_vars; i++)
	if (vals[i]) FREE(vals[i]);
      FREE(acts); FREE(vars); FREE(vals);
    }
    return ret;
  } else if (strlen(action) > bufsz)
    return ERROR;
  else {
    strcpy(buf, action);
    return OK;
  }
}

int about_dlg(IconPtr icon)
{
  if (icon && icon->pixmap) {
    static Widget popup = None, dlg = None, label = None;
    Position x, y;
    Dimension app_w, app_h, dlg_w, dlg_h;
    
    if (popup == None) {
      /* create the dialog */
      popup = XmCreateDialogShell(app_shell, "about_dlg_popup", NULL, 0);
      dlg = XmCreateBulletinBoard(popup, "about_dlg", NULL, 0);
      XtVaSetValues(dlg,
		    XmNmarginWidth, 0,
		    XmNmarginHeight, 0,
		    NULL);
      label = XmCreateLabelGadget(dlg, "Symbol", NULL, 0);
      XtVaSetValues(label,
		    XmNlabelType, XmPIXMAP,
		    XmNlabelPixmap, icon->pixmap,
		    XmNtraversalOn, False,
		    NULL);
      XtManageChild(label);
    }
    if (!XtIsManaged(dlg)) {
      /* center the dialog in the xplore window */
      XtVaGetValues(app_shell, XmNwidth, &app_w, XmNheight, &app_h, NULL);
      XtVaGetValues(dlg, XmNwidth, &dlg_w, XmNheight, &dlg_h, NULL);
      XtTranslateCoords(app_shell, 0, 0, &x, &y);
      x += (app_w - dlg_w)/2;
      y += (app_h - dlg_h)/2;
      if (x < 0) x = 0;
      if (y < 0) y = 0;
      XtVaSetValues(popup, XmNx, x, XmNy, y, NULL);
      /* show it */
      XtManageChild(dlg);
      return OK;
    }
  } else {
    static String msg = "This is xplore version %s\n\n"
"Copyright (c) 1996, 1997 by Albert Graef\n\n"
"Parts of this work are derived from xfm 1.3.2 and moxfm 0.99,\n\n"
"Copyright (c) 1990-1996 by Simon Marlow, Albert Graef, Oliver Mai\n"
"Copyright (c) 1994 by Robert Vogelgesang (shell autodetection)\n"
"Copyright (c) 1995 by Juan D. Martin (magic headers)\n\n"
"and from xfsm 1.89,\n\n"
"Copyright (c) 1993-1996 by Robert Gasch\n\n"
"Copying policy: GNU General Public License\n\n"
"For further documentation please refer to the xplore(1) manual page.";

    sprintf(s, msg, version);
    return msg_dlg("about_dlg", NULL, s, INFO, Ok, Ok, OK);
  }
}

static Widget splash_popup = None;

void splash_on(IconPtr icon)
{
  if (icon) {
    Position x, y;
    Dimension disp_w, disp_h, label_w, label_h;
    Widget popup, dlg, label;

    splash_popup = popup =
      XtVaCreatePopupShell("splash_popup",
			   overrideShellWidgetClass,
			   app_shell,
			   NULL);
    dlg = XmCreateBulletinBoard(popup, "splash_dlg", NULL, 0);
    XtVaSetValues(dlg,
		  XmNmarginWidth, 0,
		  XmNmarginHeight, 0,
		  NULL);
    label = XmCreateLabelGadget(dlg, "Symbol", NULL, 0);
    XtVaSetValues(label,
		  XmNlabelType, XmPIXMAP,
		  XmNlabelPixmap, icon->pixmap,
		  XmNtraversalOn, False,
		  NULL);
    XtManageChild(label);
    XtManageChild(dlg);
    disp_w = DisplayWidth(display, screen_no);
    disp_h = DisplayHeight(display, screen_no);
    XtVaGetValues(label, XmNwidth, &label_w, XmNheight, &label_h, NULL);
    x = (disp_w - label_w)/2;
    y = (disp_h - label_h)/2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    XtVaSetValues(popup,
		  XmNx, x, XmNy, y,
		  XmNwidth, label_w,
		  XmNheight, label_h,
		  NULL);
    XtPopup(popup, XtGrabNone);
    XDefineCursor(display, XtWindow(popup), wait_cursor);
    XmUpdateDisplay(popup);
  }
}

void splash_off(void)
{
  if (splash_popup != None)
    XtDestroyWidget(splash_popup);
}

int setup_dlg(IconPtr icon)
{
  static String msg = "Welcome to xplore version %s!\n\n"
"It appears that this is the first time you run the xplore file manager.\n"
"Xplore will now run a setup script which creates your personal shelf\n"
"directory and performs other site-specific initializations necessary\n"
"to ensure proper operation of this program. Proceed?";

    sprintf(s, msg, version);
    return msg_dlg("setup_dlg", NULL, s, WARNING, Ok|Cancel, Ok|Cancel,
		   CANCEL);
}

int error_dlg(IconPtr icon, String msg)
{
  return msg_dlg("error_dlg", icon, msg, ERRORMSG, Ok, Ok, OK);
}

int warn_dlg(IconPtr icon, String msg)
{
  return msg_dlg("warn_dlg", icon, msg, WARNING, Ok, Ok, OK);
}
