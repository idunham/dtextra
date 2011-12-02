
#include "xplore.h"
#include "interface.h"

#include "callbacks.h"
#include "curdir.h"
#include "devmount.h"
#include "dialogs.h"
#include "error.h"
#include "fileops.h"
#include "ftype.h"
#include "icons.h"
#include "interface.h"
#include "menus.h"
#include "parse.h"
#include "signals.h"
#include "util.h"

#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <Xm/TransferP.h>
#include <Tabs.h>

#if (XtSpecificationRelease > 5)

/* support for X11R6 session management */
#define USE_SM

#include <X11/SM/SMlib.h>
#include <X11/ICE/ICElib.h>

static int use_sm = 1;

#endif

#if (XtSpecificationRelease > 4)
#ifndef _NO_XMU

/* support for editres protocol */
#include <X11/Xmu/Editres.h>

#endif
#endif

/* global data: *************************************************************/

/* application context, resource db, shell, display and screen: */

XtAppContext app;
XrmDatabase db, db0;
Widget app_shell;
Display *display;
int screen_no;

/* translations and actions */

static char tree_trans_s[] = "#override <Btn2Down>: TreeStartDrag()";
static char file_trans_s[] = "#override <Btn2Down>: FileStartDrag()";

static XtTranslations tree_trans, file_trans;

static void TreeStartDrag(Widget, XEvent*);
static void FileStartDrag(Widget, XEvent*);

static XtActionsRec drag_actions[] = {
  { "TreeStartDrag", (XtActionProc)TreeStartDrag },
  { "FileStartDrag", (XtActionProc)FileStartDrag },
};

/* widgets */

Widget main_window, menubar, form, shelf_tabs, frame, work_area, file_view,
file_pane, file_scroll, tree_pane, tree_scroll, shelf_pane, shelf_scroll,
log_pane, log_scroll, status_line, message_frame, message, indicators_frame,
indicators, curgadget;

Widget file_pulldown, directory_pulldown, shelf_pulldown, options_pulldown,
view_pulldown, preferences_pulldown, help_pulldown, command_pulldown,
file_popup, log_popup;

Widget file_button, directory_button, shelf_button, options_button,
view_button, preferences_button, help_button, command_button;

Widget open_button, rename_button, move_button, copy_button, link_button,
delete_button, properties_button, bindings_button, select_button,
select_all_button, invert_all_button, filter_button, quit_button;

Widget mkdir_button, chdir_button, parent_button, home_button;

Widget shelf_open_button, shelf_rename_button, shelf_move_button,
shelf_copy_button, shelf_link_button, shelf_delete_button,
shelf_properties_button, shelf_bindings_button, shelf_select_button,
shelf_select_all_button, shelf_invert_all_button;

Widget update_button, reread_button, unmount_button, unmount_all_button,
clear_log_button, scheme_button, save_defaults_button, save_session_button,
clear_session_button, reload_config_button;

Widget manual_button, faq_button, website_button, about_button;

Widget locate_button;

Widget *command_menu_button = NULL;
int command_menu_size = 0;

Widget large_icons_button, small_icons_button, detail_button,
tree_pane_button, file_pane_button, shelf_pane_button, log_pane_button,
sort_by_name_button, sort_by_size_button, sort_by_date_button, reverse_button,
dirs_first_button, show_hidden_button, show_parent_dir_button,
show_files_button, show_dirs_button, rescan_files_button, magic_headers_button;

Widget absolute_paths_button, target_dir_button, push_dir_button,
echo_commands_button, check_mounts_button, backups_button, show_splash_button,
autosave_defaults_button, autosave_session_button, autopopup_button,
confirm_drop_button, confirm_move_button,
confirm_copy_button, confirm_link_button, confirm_delete_button,
confirm_deldir_button, confirm_delfile_button, confirm_overwrite_button,
confirm_quit_button, auto_updates_button;

Cursor wait_cursor;

/* resource values: **********************************************************/

String progname;
char configdir[MAXPATHLEN+1], sessionfile[MAXPATHLEN+1];
char magicfile[MAXPATHLEN+1], configfile[MAXPATHLEN+1], startup[MAXPATHLEN+1],
  resfile[MAXPATHLEN+1];
String curshelf;
String cpp_options;
String iconpath, color_scheme;
String sh_list, shell;
String usertypes;

unsigned char keyboardFocusPolicy;

char rootdirname[MAXPATHLEN+1], curdirname[MAXPATHLEN+1],
shelfdirname[MAXPATHLEN+1];

Boolean show_getwd;

int savelines;

int update_time;
Boolean updates;

ViewType viewtype;
Boolean filepane, treepane, shelfpane, logpane;

Boolean absolute, dirtarget, pushdir;

Boolean
confirm_drop,
confirm_move,
confirm_copy,
confirm_link,
confirm_delete,
confirm_deldir,
confirm_delfile,
confirm_overwrt,
confirm_quit;

Boolean autosave, autosession, autopopup;
Boolean showhelp, showver, splash;
Boolean console;

/* default options for directories */

static int stdoptions =
CHECK_FILES|SORT_BY_NAME|INCLUDE_UPDIR|INCLUDE_FILES|INCLUDE_DIRS|DIRS_FIRST;

static int shelfoptions =
CHECK_FILES|SORT_BY_NAME|INCLUDE_HIDDEN|INCLUDE_FILES|INCLUDE_DIRS|DIRS_FIRST;

/* resource data */

typedef enum _SortType {
    ByName, BySize, ByDate,
} SortType;

typedef struct _ResData {
  String rootdir, curdir, shelfdir, curshelf;
  String iconpath, scheme;
  String configdir, clone, session;
  String cppoptions;
  String bourneShells, shell;
  String metachars;
  String types;
  Boolean getwd;
  Boolean updates;
  int update, multiclick, savelines;
  Boolean file, tree, shelf, log;
  Boolean fileonly, treeonly, shelfonly, logonly;
  ViewType view;
  Boolean updir, hidden, files, dirs;
  SortType sort;
  Boolean reverse, dirsfirst;
  Boolean rescan, magic;
  Boolean check;
  Boolean absolute, dirtarget, pushdir, echo, backups;
  Boolean nosm;
  Boolean autosave, autosession, autopopup, showhelp, showver, splash, console;
  Boolean drop, move, copy, link, delete, deldir, delfile, overwrt, quit;
} ResData;

static ResData resdata;

/* resource converters */

static void StringToViewType(XrmValue *args, Cardinal *n_args,
			     XrmValue *fromVal, XrmValue *toVal)
{
  static ViewType t;

  if (!(strcmp(fromVal->addr, "LargeIcons")))
    t = LargeIconView;
  else if (!(strcmp(fromVal->addr, "SmallIcons")))
    t = SmallIconView;
  else if (!(strcmp(fromVal->addr, "Detail")))
    t = DetailView;
  else {
    XtStringConversionWarning(fromVal->addr, "ViewType");
    return;
  }
  
  toVal->addr = (caddr_t) &t;
  toVal->size = sizeof(ViewType);
}

static void StringToSortType(XrmValue *args, Cardinal *n_args,
			     XrmValue *fromVal, XrmValue *toVal)
{
  static SortType t;

  if (!(strcmp(fromVal->addr, "ByName")))
    t = ByName;
  else if (!(strcmp(fromVal->addr, "BySize")))
    t = BySize;
  else if (!(strcmp(fromVal->addr, "ByDate")))
    t = ByDate;
  else {
    XtStringConversionWarning(fromVal->addr, "SortType");
    return;
  }
  
  toVal->addr = (caddr_t) &t;
  toVal->size = sizeof(ViewType);
}

/* command line options */

static XrmOptionDescRec options[] = {
  { "-C", ".console", XrmoptionNoArg, "True" },
  { "-help", ".showhelp", XrmoptionNoArg, "True" },
  { "-nosm", ".nosm", XrmoptionNoArg, "True" },
  { "-o", ".cppoptions", XrmoptionSepArg, NULL },
  { "-p", ".curdir", XrmoptionSepArg, NULL },
  { "-s", ".curshelf", XrmoptionSepArg, NULL },
  { "-tree", ".treeonly", XrmoptionNoArg, "True" },
  { "-file", ".fileonly", XrmoptionNoArg, "True" },
  { "-shelf", ".shelfonly", XrmoptionNoArg, "True" },
  { "-log", ".logonly", XrmoptionNoArg, "True" },
  { "-version", ".showver", XrmoptionNoArg, "True" },
};

/* this one has to be parsed first so we can get the defaults and session file
   names from the command line */

static XrmOptionDescRec options0[] = {
  { "-c", ".configdir", XrmoptionSepArg, NULL },
  { "-clone", ".clone", XrmoptionSepArg, NULL },
  { "-session", ".session", XrmoptionSepArg, NULL },
};

/* fallback resources */

static String fallback_resources[] = {
#include "defaults.h"
  NULL
};

/* application resources with hard-coded defaults */

static XtResource resources[] = {
  { "iconpath", "Iconpath", XmRString, sizeof(String),
    XtOffsetOf(ResData, iconpath), XmRString,
    "/usr/lib/X11/xplore/icons" },
  { "scheme", "Scheme", XmRString, sizeof(String),
    XtOffsetOf(ResData, scheme), XmRString,
    NULL },
  { "rootdir", "Rootdir", XmRString, sizeof(String),
    XtOffsetOf(ResData, rootdir), XmRString,
    "/" },
  { "curdir", "Curdir", XmRString, sizeof(String),
    XtOffsetOf(ResData, curdir), XmRString,
    "." },
  { "shelfdir", "Shelfdir", XmRString, sizeof(String),
    XtOffsetOf(ResData, shelfdir), XmRString,
    "~/shelf" },
  { "curshelf", "Curshelf", XmRString, sizeof(String),
    XtOffsetOf(ResData, curshelf), XmRString,
    "" },
  { "configdir", "configdir", XmRString, sizeof(String),
    XtOffsetOf(ResData, configdir), XmRString,
    "~/.xplore" },
  { "clone", "Clone", XmRString, sizeof(String),
    XtOffsetOf(ResData, clone), XmRString,
    NULL },
  { "session", "Session", XmRString, sizeof(String),
    XtOffsetOf(ResData, session), XmRString,
    NULL },
  { "cppoptions", "CppOptions", XmRString, sizeof(String),
    XtOffsetOf(ResData, cppoptions), XmRString,
    "-I/usr/lib/X11/xplore" },
  { "bourneShells", "BourneShells", XmRString, sizeof(String),
    XtOffsetOf(ResData, bourneShells), XmRString,
    "AUTO" },
  { "shell", "Shell", XmRString, sizeof(String),
    XtOffsetOf(ResData, shell), XmRString,
    NULL },
  { "metachars", "Metachars", XmRString, sizeof(String),
    XtOffsetOf(ResData, metachars), XmRString,
    NULL },
  { "types", "Types", XmRString, sizeof(String),
    XtOffsetOf(ResData, types), XmRString,
    NULL },
  { "getwd", "Getwd", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, getwd), XmRImmediate,
    (XtPointer) True },
  { "updates", "Updates", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, updates), XmRImmediate,
    (XtPointer) True },
  { "update", "Update", XmRInt, sizeof(int),
    XtOffsetOf(ResData, update), XmRImmediate,
    (XtPointer) 10000 },
  { "multiclick", "Multiclick", XmRInt, sizeof(int),
    XtOffsetOf(ResData, multiclick), XmRImmediate,
    (XtPointer) 300 },
  { "savelines", "Savelines", XmRInt, sizeof(int),
    XtOffsetOf(ResData, savelines), XmRImmediate,
    (XtPointer) 600 },
  { "tree", "Panes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, tree), XmRImmediate,
    (XtPointer) True },
  { "file", "Panes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, file), XmRImmediate,
    (XtPointer) True },
  { "shelf", "Panes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, shelf), XmRImmediate,
    (XtPointer) True },
  { "log", "Panes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, log), XmRImmediate,
    (XtPointer) False },
  { "view", "View", "ViewType", sizeof(ViewType),
    XtOffsetOf(ResData, view), XmRImmediate,
    (XtPointer) LargeIconView },
  { "updir", "Items", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, updir), XmRImmediate,
    (XtPointer) True },
  { "hidden", "Items", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, hidden), XmRImmediate,
    (XtPointer) False },
  { "files", "Items", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, files), XmRImmediate,
    (XtPointer) True },
  { "dirs", "Items", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, dirs), XmRImmediate,
    (XtPointer) True },
  { "sort", "Sort", "SortType", sizeof(SortType),
    XtOffsetOf(ResData, sort), XmRImmediate,
    (XtPointer) ByName },
  { "reverse", "Reverse", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, reverse), XmRImmediate,
    (XtPointer) False },
  { "dirsfirst", "DirsFirst", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, dirsfirst), XmRImmediate,
    (XtPointer) True },
  { "rescan", "Rescan", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, rescan), XmRImmediate,
    (XtPointer) True },
  { "magic", "Magic", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, magic), XmRImmediate,
    (XtPointer) True },
  { "check", "Check", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, check), XmRImmediate,
    (XtPointer) True },
  { "absolute", "Absolute", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, absolute), XmRImmediate,
    (XtPointer) False },
  { "dirtarget", "Dirtarget", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, dirtarget), XmRImmediate,
    (XtPointer) False },
  { "pushdir", "Pushdir", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, pushdir), XmRImmediate,
    (XtPointer) True },
  { "echo", "Echo", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, echo), XmRImmediate,
    (XtPointer) False },
  { "backups", "Backups", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, backups), XmRImmediate,
    (XtPointer) True },
  { "nosm", "NoSM", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, nosm), XmRImmediate,
    (XtPointer) False },
  { "autosave", "Autosave", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, autosave), XmRImmediate,
    (XtPointer) False },
  { "autosession", "Autosession", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, autosession), XmRImmediate,
    (XtPointer) True },
  { "autopopup", "Autopopup", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, autopopup), XmRImmediate,
    (XtPointer) True },
  { "showhelp", "Showhelp", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, showhelp), XmRImmediate,
    (XtPointer) False },
  { "showver", "Showver", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, showver), XmRImmediate,
    (XtPointer) False },
  { "splash", "Splash", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, splash), XmRImmediate,
    (XtPointer) False },
  { "console", "Console", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, console), XmRImmediate,
    (XtPointer) False },
  { "drop", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, drop), XmRImmediate,
    (XtPointer) True },
  { "move", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, move), XmRImmediate,
    (XtPointer) True },
  { "copy", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, copy), XmRImmediate,
    (XtPointer) True },
  { "link", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, link), XmRImmediate,
    (XtPointer) True },
  { "delete", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, delete), XmRImmediate,
    (XtPointer) True },
  { "deldir", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, deldir), XmRImmediate,
    (XtPointer) True },
  { "delfile", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, delfile), XmRImmediate,
    (XtPointer) True },
  { "overwrt", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, overwrt), XmRImmediate,
    (XtPointer) True },
  { "quit", "Confirm", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, quit), XmRImmediate,
    (XtPointer) True },
  /* the following are for processing command line options and should never be
     set from the resources file */
  { "treeonly", "OnlyPanes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, treeonly), XmRImmediate,
    (XtPointer) False },
  { "fileonly", "OnlyPanes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, fileonly), XmRImmediate,
    (XtPointer) False },
  { "shelfonly", "OnlyPanes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, shelfonly), XmRImmediate,
    (XtPointer) False },
  { "logonly", "OnlyPanes", XmRBoolean, sizeof(Boolean),
    XtOffsetOf(ResData, logonly), XmRImmediate,
    (XtPointer) False },
};

/* this is used to initially merge the defaults (defaults and session files) */

static XtResource resources0[] = {
  { "configdir", "configdir", XmRString, sizeof(String),
    XtOffsetOf(ResData, configdir), XmRString,
    "~/.xplore" },
  { "clone", "Clone", XmRString, sizeof(String),
    XtOffsetOf(ResData, clone), XmRString,
    NULL },
  { "session", "Session", XmRString, sizeof(String),
    XtOffsetOf(ResData, session), XmRString,
    NULL },
};

/* this is used to reload the user type definitions from the defaults file */

static XtResource type_resources[] = {
  { "types", "Types", XmRString, sizeof(String),
    XtOffsetOf(ResData, types), XmRString,
    NULL },
};

/* menus: *******************************************************************/

/* menu callbacks: these are used to lock updates when a menu has been
   posted */

static int level = 0;
static Boolean lock_save;

static void menu_map_cb(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (XtIsTransientShell(XtParent(w))) return;
  if (level == 0) {
    lock_save = lock;
    lock = True;
  }
  level++;
}

static void menu_unmap_cb(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (XtIsTransientShell(XtParent(w))) return;
  if (level <= 0)
    return;
  else if (level == 1)
    lock = lock_save;
  level--;
}

/* menu descriptions */

enum {
    MENU_POPUP, MENU_PULLDOWN
};

enum {
    SEPARATOR_ID, PUSH_BUTTON_ID, CHECK_BUTTON_ID, RADIO_BUTTON_ID,
    CASCADE_BUTTON_ID
};

typedef struct _MenuItem {
  int id;
  String label;
  String mnemonic;
  String accelerator;
  String accelerator_text;
  struct _MenuItem *submenu;
  int submenu_size;
  XtCallbackProc callback, unmap_callback;
  XtPointer data;
  String button_name;
  Widget *button;
  String pulldown_name;
  Widget *pulldown;
} MenuItem, MenuDesc[];

#define SEPARATOR \
{ \
    SEPARATOR_ID, NULL, NULL, NULL, NULL, NULL, 0, \
    (XtCallbackProc)NULL, (XtCallbackProc)NULL, NULL, NULL, NULL, NULL, NULL \
}

#define PUSH_BUTTON(lbl,mnem,accel,accel_text,cb,data,but_name,but) \
{ \
    PUSH_BUTTON_ID, lbl, mnem, accel, accel_text, NULL, 0, \
    (XtCallbackProc)cb, (XtCallbackProc)NULL, data, but_name, but, NULL, NULL \
}

#define CHECK_BUTTON(lbl,mnem,accel,accel_text,cb,data,but_name,but) \
{ \
    CHECK_BUTTON_ID, lbl, mnem, accel, accel_text, NULL, 0, \
    (XtCallbackProc)cb, (XtCallbackProc)NULL, data, but_name, but, NULL, NULL \
}

#define RADIO_BUTTON(lbl,mnem,accel,accel_text,cb,data,but_name,but) \
{ \
    RADIO_BUTTON_ID, lbl, mnem, accel, accel_text, NULL, 0, \
    (XtCallbackProc)cb, (XtCallbackProc)NULL, data, but_name, but, NULL, NULL \
}

#define CASCADE_BUTTON(lbl,mnem,accel,accel_text,subm,subm_sz,map_cb,unmap_cb,\
but_name,but,pdn_name,pdn) \
{ \
    CASCADE_BUTTON_ID, lbl, mnem, accel, accel_text, subm, subm_sz, \
    (XtCallbackProc)map_cb, (XtCallbackProc)unmap_cb, \
    NULL, but_name, but, pdn_name, pdn \
}

#define COMMAND 2

static MenuDesc file_menu_desc = {
  PUSH_BUTTON("Open", NULL, NULL, NULL, OpenCB, NULL,
	      "Open", &open_button),
  SEPARATOR,
  CASCADE_BUTTON("Command", NULL, NULL, NULL, NULL, 0,
		 menu_map_cb, menu_unmap_cb,
		 "CommandButton", &command_button,
		 "CommandPulldown", &command_pulldown),
  SEPARATOR,
  PUSH_BUTTON("Rename...", NULL, NULL, NULL, RenameCB, NULL,
	      "Rename", &rename_button),
  PUSH_BUTTON("Move...", NULL, NULL, NULL, MoveCB, NULL,
	      "Move", &move_button),
  PUSH_BUTTON("Copy...", NULL, NULL, NULL, CopyCB, NULL,
	      "Copy", &copy_button),
  PUSH_BUTTON("Link...", NULL, NULL, NULL, LinkCB, NULL,
	      "Link", &link_button),
  PUSH_BUTTON("Delete", NULL, NULL, NULL, DeleteCB, NULL,
	      "Delete", &delete_button),
  SEPARATOR,
  PUSH_BUTTON("Properties...", NULL, NULL, NULL, PropertiesCB, NULL,
	      "Properties", &properties_button),
  PUSH_BUTTON("File type...", NULL, NULL, NULL, BindingsCB, NULL,
	      "File_type", &bindings_button),
  SEPARATOR,
  PUSH_BUTTON("Select...", NULL, NULL, NULL, SelectCB, NULL,
	      "Select", &select_button),
  PUSH_BUTTON("Select all", NULL, NULL, NULL, SelectAllCB, NULL,
	      "Select_all", &select_all_button),
  PUSH_BUTTON("Invert all", NULL, NULL, NULL, InvertAllCB, NULL,
	      "Invert_all", &invert_all_button),
  SEPARATOR,
  PUSH_BUTTON("Filter...", NULL, NULL, NULL, FilterCB, NULL,
	      "Filter", &filter_button),
  SEPARATOR,
  PUSH_BUTTON("Quit", NULL, NULL, NULL, QuitCB, NULL,
	      "Quit", &quit_button),
};

static MenuItem *command_menu_desc = NULL;

static MenuDesc directory_menu_desc = {
  PUSH_BUTTON("Make directory...", NULL, NULL, NULL, MkdirCB, NULL,
	      "Make_directory", &mkdir_button),
  PUSH_BUTTON("Change directory...", NULL, NULL, NULL, ChdirCB, NULL,
	      "Change_directory", &chdir_button),
  PUSH_BUTTON("Parent", NULL, NULL, NULL, ParentCB, NULL,
	      "Parent", &parent_button),
  PUSH_BUTTON("Home", NULL, NULL, NULL, HomeCB, NULL,
	      "Home", &home_button),
};

static MenuDesc shelf_menu_desc = {
  PUSH_BUTTON("Open", NULL, NULL, NULL, ShelfOpenCB, NULL,
	      "Open", &shelf_open_button),
  SEPARATOR,
  PUSH_BUTTON("Rename...", NULL, NULL, NULL, ShelfRenameCB, NULL,
	      "Rename", &shelf_rename_button),
  PUSH_BUTTON("Move...", NULL, NULL, NULL, ShelfMoveCB, NULL,
	      "Move", &shelf_move_button),
  PUSH_BUTTON("Copy...", NULL, NULL, NULL, ShelfCopyCB, NULL,
	      "Copy", &shelf_copy_button),
  PUSH_BUTTON("Link...", NULL, NULL, NULL, ShelfLinkCB, NULL,
	      "Link", &shelf_link_button),
  PUSH_BUTTON("Delete", NULL, NULL, NULL, ShelfDeleteCB, NULL,
	      "Delete", &shelf_delete_button),
  SEPARATOR,
  PUSH_BUTTON("Properties...", NULL, NULL, NULL, ShelfPropertiesCB, NULL,
	      "Properties", &shelf_properties_button),
  PUSH_BUTTON("File type...", NULL, NULL, NULL, ShelfBindingsCB, NULL,
	      "File_type", &shelf_bindings_button),
  SEPARATOR,
  PUSH_BUTTON("Select...", NULL, NULL, NULL, ShelfSelectCB, NULL,
	      "Select", &shelf_select_button),
  PUSH_BUTTON("Select all", NULL, NULL, NULL, ShelfSelectAllCB, NULL,
	      "Select_all", &shelf_select_all_button),
  PUSH_BUTTON("Invert all", NULL, NULL, NULL, ShelfInvertAllCB, NULL,
	      "Invert_all", &shelf_invert_all_button),
};

static MenuDesc view_menu_desc = {
  RADIO_BUTTON("Large icons", NULL, NULL, NULL, LargeIconsCB, NULL,
	       "Large_icons", &large_icons_button),
  RADIO_BUTTON("Small icons", NULL, NULL, NULL, SmallIconsCB, NULL,
	       "Small_icons", &small_icons_button),
  RADIO_BUTTON("Detail", NULL, NULL, NULL, DetailCB, NULL,
	       "Detail", &detail_button),
  SEPARATOR,
  CHECK_BUTTON("Shelf pane", NULL, NULL, NULL, ShelfPaneCB, NULL,
	       "Shelf_pane", &shelf_pane_button),
  CHECK_BUTTON("Tree pane", NULL, NULL, NULL, TreePaneCB, NULL,
	       "Tree_pane", &tree_pane_button),
  CHECK_BUTTON("File pane", NULL, NULL, NULL, FilePaneCB, NULL,
	       "File_pane", &file_pane_button),
  CHECK_BUTTON("Log pane", NULL, NULL, NULL, LogPaneCB, NULL,
	       "Log_pane", &log_pane_button),
  SEPARATOR,
  RADIO_BUTTON("Sort by name", NULL, NULL, NULL, SortByNameCB, NULL,
	       "Sort_by_name", &sort_by_name_button),
  RADIO_BUTTON("Sort by size", NULL, NULL, NULL, SortBySizeCB, NULL,
	       "Sort_by_size", &sort_by_size_button),
  RADIO_BUTTON("Sort by date", NULL, NULL, NULL, SortByDateCB, NULL,
	       "Sort_by_date", &sort_by_date_button),
  SEPARATOR,
  CHECK_BUTTON("Reverse order", NULL, NULL, NULL, ReverseCB, NULL,
	       "Reverse_order", &reverse_button),
  CHECK_BUTTON("Directories first", NULL, NULL, NULL, DirsFirstCB, NULL,
	       "Directories_first", &dirs_first_button),
  SEPARATOR,
  CHECK_BUTTON("Show hidden files", NULL, NULL, NULL,
	       ShowHiddenFilesCB, NULL,
	       "Show_hidden_files", &show_hidden_button),
  CHECK_BUTTON("Show files", NULL, NULL, NULL, ShowFilesCB, NULL,
	       "Show_files", &show_files_button),
  CHECK_BUTTON("Show directories", NULL, NULL, NULL, ShowDirsCB, NULL,
	       "Show_directories", &show_dirs_button),
  CHECK_BUTTON("Show parent dir", NULL, NULL, NULL, ShowParentDirCB, NULL,
	       "Show_parent_dir", &show_parent_dir_button),
  SEPARATOR,
  CHECK_BUTTON("Rescan files", NULL, NULL, NULL, RescanFilesCB, NULL,
	       "Rescan_files", &rescan_files_button),
  CHECK_BUTTON("Magic headers", NULL, NULL, NULL, MagicHeadersCB, NULL,
	       "Magic_headers", &magic_headers_button),
};

static MenuDesc preferences_menu_desc = {
  CHECK_BUTTON("Absolute paths", NULL, NULL, NULL, AbsolutePathsCB, NULL,
	       "Absolute_paths", &absolute_paths_button),
  CHECK_BUTTON("Push in current dir", NULL, NULL, NULL, PushDirCB, NULL,
	       "Push_in_current_dir", &push_dir_button),
  CHECK_BUTTON("Drop in target dir", NULL, NULL, NULL, TargetDirCB, NULL,
	       "Drop_in_target_dir", &target_dir_button),
  CHECK_BUTTON("Echo commands", NULL, NULL, NULL, EchoCommandsCB, NULL,
	       "Echo_commands", &echo_commands_button),
  CHECK_BUTTON("Check mounts", NULL, NULL, NULL, CheckMountsCB, NULL,
	       "Check_mounts", &check_mounts_button),
  CHECK_BUTTON("Backups", NULL, NULL, NULL, BackupsCB, NULL,
	       "Backups", &backups_button),
  SEPARATOR,
  CHECK_BUTTON("Show splash screen", NULL, NULL, NULL, ShowSplashCB, NULL,
	       "Show_splash_screen", &show_splash_button),
  CHECK_BUTTON("Autopopup log", NULL, NULL, NULL, AutopopupCB, NULL,
	       "Autopopup_log", &autopopup_button),
  CHECK_BUTTON("Autosave defaults", NULL, NULL, NULL, AutosaveDefaultsCB, NULL,
	       "Autosave_defaults", &autosave_defaults_button),
  CHECK_BUTTON("Autosave session", NULL, NULL, NULL, AutosaveSessionCB, NULL,
	       "Autosave_session", &autosave_session_button),
  SEPARATOR,
  CHECK_BUTTON("Confirm drop", NULL, NULL, NULL, ConfirmDropCB, NULL,
	       "Confirm_drop", &confirm_drop_button),
  CHECK_BUTTON("Confirm move", NULL, NULL, NULL, ConfirmMoveCB, NULL,
	       "Confirm_move", &confirm_move_button),
  CHECK_BUTTON("Confirm copy", NULL, NULL, NULL, ConfirmCopyCB, NULL,
	       "Confirm_copy", &confirm_copy_button),
  CHECK_BUTTON("Confirm link", NULL, NULL, NULL, ConfirmLinkCB, NULL,
	       "Confirm_link", &confirm_link_button),
  CHECK_BUTTON("Confirm delete", NULL, NULL, NULL, ConfirmDeleteCB, NULL,
	       "Confirm_delete", &confirm_delete_button),
  CHECK_BUTTON("Confirm delete dir", NULL, NULL, NULL, ConfirmDelDirCB, NULL,
	       "Confirm_deldir", &confirm_deldir_button),
  CHECK_BUTTON("Confirm delete file", NULL, NULL, NULL, ConfirmDelFileCB, NULL,
	       "Confirm_delfile", &confirm_delfile_button),
  CHECK_BUTTON("Confirm overwrite", NULL, NULL, NULL, ConfirmOverwriteCB,
	       NULL,
	       "Confirm_overwrite", &confirm_overwrite_button),
  CHECK_BUTTON("Confirm quit", NULL, NULL, NULL, ConfirmQuitCB, NULL,
	       "Confirm_quit", &confirm_quit_button),
  SEPARATOR,
  CHECK_BUTTON("Automatic updates", NULL, NULL, NULL, AutoUpdatesCB, NULL,
	       "Automatic_updates", &auto_updates_button),
};

static MenuDesc options_menu_desc = {
  CASCADE_BUTTON("View", NULL, NULL, NULL,
		 view_menu_desc, XtNumber(view_menu_desc),
		 menu_map_cb, menu_unmap_cb,
		 "View", &view_button,
		 "ViewPulldown", &view_pulldown),
  CASCADE_BUTTON("Preferences", NULL, NULL, NULL,
		 preferences_menu_desc, XtNumber(preferences_menu_desc),
		 menu_map_cb, menu_unmap_cb,
		 "Preferences", &preferences_button,
		 "PreferencesPulldown", &preferences_pulldown),
  SEPARATOR,
  PUSH_BUTTON("Update", NULL, NULL, NULL, UpdateCB, NULL,
	      "Update", &update_button),
  PUSH_BUTTON("Reread", NULL, NULL, NULL, RereadCB, NULL,
	      "Reread", &reread_button),
  PUSH_BUTTON("Unmount", NULL, NULL, NULL, UnmountCB, NULL,
	      "Unmount", &unmount_button),
  PUSH_BUTTON("Unmount all", NULL, NULL, NULL, UnmountAllCB, NULL,
	      "Unmount_all", &unmount_all_button),
  SEPARATOR,
  PUSH_BUTTON("Clear log", NULL, NULL, NULL, ClearLogCB, NULL,
	      "Clear_log", &clear_log_button),
  SEPARATOR,
  PUSH_BUTTON("Color scheme...", NULL, NULL, NULL, SchemeCB, NULL,
	      "Color_scheme", &scheme_button),
  PUSH_BUTTON("Save defaults", NULL, NULL, NULL, SaveDefaultsCB, NULL,
	      "Save_defaults", &save_defaults_button),
  PUSH_BUTTON("Save session", NULL, NULL, NULL, SaveSessionCB, NULL,
	      "Save_session", &save_session_button),
  PUSH_BUTTON("Clear session", NULL, NULL, NULL, ClearSessionCB, NULL,
	      "Clear_session", &clear_session_button),
  PUSH_BUTTON("Reload configuration file", NULL, NULL, NULL, ReloadConfigCB,
	      NULL,
	      "Reload_configuration_file", &reload_config_button),
};

static char manual[MAXPATHLEN], faq[MAXPATHLEN],
  website[] = "http://www.musikwissenschaft.uni-mainz.de/~ag/xplore";

static MenuDesc help_menu_desc = {
  PUSH_BUTTON("Xplore manual", NULL, NULL, NULL, HelpCB, manual,
	      "Xplore_manual", &manual_button),
  PUSH_BUTTON("Xplore FAQ", NULL, NULL, NULL, HelpCB, faq,
	      "Xplore_FAQ", &faq_button),
  PUSH_BUTTON("Xplore WWW page", NULL, NULL, NULL, HelpCB, website,
	      "Xplore_WWW_page", &website_button),
  SEPARATOR,
  PUSH_BUTTON("About xplore...", NULL, NULL, NULL, AboutCB, NULL,
	      "About_xplore", &about_button),
};

static MenuItem *file_popup_desc = NULL;
static Widget *file_popup_button = NULL;
static int file_popup_size = 0;

static MenuDesc file_popup_desc0 = {
  PUSH_BUTTON("Open", NULL, NULL, NULL, OpenCB, NULL,
	      "Open", NULL),
  SEPARATOR,
  PUSH_BUTTON("Rename...", NULL, NULL, NULL, RenameCB, NULL,
	      "Rename", NULL),
  PUSH_BUTTON("Move...", NULL, NULL, NULL, MoveCB, NULL,
	      "Move", NULL),
  PUSH_BUTTON("Copy...", NULL, NULL, NULL, CopyCB, NULL,
	      "Copy", NULL),
  PUSH_BUTTON("Link...", NULL, NULL, NULL, LinkCB, NULL,
	      "Link", NULL),
  PUSH_BUTTON("Delete", NULL, NULL, NULL, DeleteCB, NULL,
	      "Delete", NULL),
  SEPARATOR,
  PUSH_BUTTON("Properties...", NULL, NULL, NULL, PropertiesCB, NULL,
	      "Properties", NULL),
  PUSH_BUTTON("File type...", NULL, NULL, NULL, BindingsCB, NULL,
	      "File_type", NULL),
  SEPARATOR,
};

static MenuDesc shelf_popup_desc0 = {
  PUSH_BUTTON("Open", NULL, NULL, NULL, ShelfOpenCB, NULL,
	      "Open", NULL),
  SEPARATOR,
  PUSH_BUTTON("Rename...", NULL, NULL, NULL, ShelfRenameCB, NULL,
	      "Rename", NULL),
  PUSH_BUTTON("Move...", NULL, NULL, NULL, ShelfMoveCB, NULL,
	      "Move", NULL),
  PUSH_BUTTON("Copy...", NULL, NULL, NULL, ShelfCopyCB, NULL,
	      "Copy", NULL),
  PUSH_BUTTON("Link...", NULL, NULL, NULL, ShelfLinkCB, NULL,
	      "Link", NULL),
  PUSH_BUTTON("Delete", NULL, NULL, NULL, ShelfDeleteCB, NULL,
	      "Delete", NULL),
  SEPARATOR,
  PUSH_BUTTON("Properties...", NULL, NULL, NULL, ShelfPropertiesCB, NULL,
	      "Properties", NULL),
  PUSH_BUTTON("File type...", NULL, NULL, NULL, ShelfBindingsCB, NULL,
	      "File_type", NULL),
  SEPARATOR,
};

static MenuDesc log_popup_desc = {
  PUSH_BUTTON("Locate", NULL, NULL, NULL, LocateCB, NULL,
	      "Locate", &locate_button)
};

/* create the description of the command menu */

static void CreateCommandMenuDesc(void)
{
  int i;
  
  for (i = 0; i < command_menu_size; i++)
    FREE(command_menu_desc[i].button_name);
  FREE(command_menu_desc); FREE(command_menu_button);
  command_menu_size = n_cmd_actions();
  command_menu_desc = (MenuItem*) MALLOC(command_menu_size*sizeof(MenuItem));
  command_menu_button = (Widget*) MALLOC(command_menu_size*sizeof(Widget));
  for (i = 0; i < n_cmd_actions(); i++) 
    if (cmd_label(i)) {
      String button_name = alloca(strlen(cmd_label(i))+1);

      resname(button_name, cmd_label(i));
      command_menu_desc[i].id = PUSH_BUTTON_ID;
      command_menu_desc[i].label = cmd_label(i);
      command_menu_desc[i].mnemonic = command_menu_desc[i].accelerator =
	command_menu_desc[i].accelerator_text = NULL;
      command_menu_desc[i].submenu = NULL;
      command_menu_desc[i].submenu_size = 0;
      command_menu_desc[i].callback = (XtCallbackProc) CommandActionCB;
      command_menu_desc[i].unmap_callback = NULL;
      command_menu_desc[i].data = (XtPointer) i;
      command_menu_desc[i].button = &command_menu_button[i];
      command_menu_desc[i].button_name = NEWSTRING(button_name);
      command_menu_desc[i].pulldown = NULL;
      command_menu_desc[i].pulldown_name = NULL;
    } else {
      command_menu_desc[i].id = SEPARATOR_ID;
      command_menu_desc[i].label = NULL;
      command_menu_desc[i].mnemonic = command_menu_desc[i].accelerator =
	command_menu_desc[i].accelerator_text = NULL;
      command_menu_desc[i].submenu = NULL;
      command_menu_desc[i].submenu_size = 0;
      command_menu_desc[i].callback = NULL;
      command_menu_desc[i].unmap_callback = NULL;
      command_menu_desc[i].data = NULL;
      command_menu_desc[i].button = NULL;
      command_menu_desc[i].button_name = NULL;
      command_menu_desc[i].pulldown = NULL;
      command_menu_desc[i].pulldown_name = NULL;
    }
}

/* create the description of the file popup */

static void CreateFilePopupDesc(DirPtr dir, int item)
{
  FilePtr file = dirFile(dir, item);
  int i, n = XtNumber(file_popup_desc0), type = fileType(file);
  
  for (i = n; i < file_popup_size; i++)
    FREE(file_popup_desc[i].button_name);
  FREE(file_popup_desc); FREE(file_popup_button);
  if (!n_menu_actions(type)) n--;
  file_popup_size = n+n_menu_actions(type);
  file_popup_desc = (MenuItem*) MALLOC(file_popup_size*sizeof(MenuItem));
  file_popup_button = (Widget*) MALLOC(file_popup_size*sizeof(Widget));
  for (i = 0; i < n; i++) {
    file_popup_desc[i] = (dir == curdir) ? file_popup_desc0[i] :
      shelf_popup_desc0[i];
    file_popup_desc[i].button = &file_popup_button[i];
  }
  for (i = 0; i < n_menu_actions(type); i++)
    if (menu_label(type, i)) {
      String button_name = alloca(strlen(menu_label(type, i))+1);

      resname(button_name, menu_label(type, i));
      file_popup_desc[i+n].id = PUSH_BUTTON_ID;
      file_popup_desc[i+n].label = menu_label(type, i);
      file_popup_desc[i+n].mnemonic = file_popup_desc[i+n].accelerator =
	file_popup_desc[i+n].accelerator_text = NULL;
      file_popup_desc[i+n].submenu = NULL;
      file_popup_desc[i+n].submenu_size = 0;
      file_popup_desc[i+n].callback = (XtCallbackProc)
	((dir == curdir) ? MenuActionCB : ShelfMenuActionCB);
      file_popup_desc[i+n].unmap_callback = NULL;
      file_popup_desc[i+n].data = (XtPointer) i;
      file_popup_desc[i+n].button = &file_popup_button[i+n];
      file_popup_desc[i+n].button_name = NEWSTRING(button_name);
      file_popup_desc[i+n].pulldown = NULL;
      file_popup_desc[i+n].pulldown_name = NULL;
    } else {
      file_popup_desc[i+n].id = SEPARATOR_ID;
      file_popup_desc[i+n].label = NULL;
      file_popup_desc[i+n].mnemonic = file_popup_desc[i+n].accelerator =
	file_popup_desc[i+n].accelerator_text = NULL;
      file_popup_desc[i+n].submenu = NULL;
      file_popup_desc[i+n].submenu_size = 0;
      file_popup_desc[i+n].callback = NULL;
      file_popup_desc[i+n].unmap_callback = NULL;
      file_popup_desc[i+n].data = NULL;
      file_popup_desc[i+n].button = NULL;
      file_popup_desc[i+n].button_name = NULL;
      file_popup_desc[i+n].pulldown = NULL;
      file_popup_desc[i+n].pulldown_name = NULL;
    }
}

/* convenience functions to create popup and pulldown menus from
   descriptions */

static void CreateMenuButtons(Widget menu, MenuDesc desc, int size);

static void CreateMenu(Widget parent,
		       int type,
		       String name,
		       String mnemonic,
		       String accelerator, String accelerator_text,
		       MenuDesc desc, int size,
		       XtCallbackProc map_callback,
		       XtCallbackProc unmap_callback,
		       String menu_name, Widget *menu,
		       String button_name, Widget *button)
{
  Widget pdn, but;
  XmString label;
  int nargs = 0;
  Arg args[3];
  
  if (type == MENU_PULLDOWN)
    *menu = XmCreatePulldownMenu(parent, menu_name, NULL, 0);
  else
    *menu = XmCreatePopupMenu(parent, menu_name, NULL, 0);
  if (map_callback)
    XtAddCallback(*menu,
		  XmNmapCallback, map_callback,
		  NULL);
  if (unmap_callback)
    XtAddCallback(*menu,
		  XmNunmapCallback, unmap_callback,
		  NULL);
  if (mnemonic) {
    XtSetArg(args[0],
	     XmNmnemonic, XStringToKeysym(mnemonic));
    nargs++;
  }
  if (accelerator) {
    XtSetArg(args[nargs],
	     XmNaccelerator, accelerator);
    nargs++;
  }
  if (accelerator_text) {
    label = XmStringCreateLocalized(accelerator_text);
    XtSetArg(args[nargs], XmNacceleratorText, label);
    nargs++;
  }
  if (accelerator_text)
    XmStringFree(label);
  if (type == MENU_PULLDOWN) {
    *button = XmCreateCascadeButton(parent, button_name, args, nargs);
    label = XmStringCreateLocalized(name);
    XtVaSetValues(*button,
		  XmNlabelString, label,
		  XmNsubMenuId, *menu,
		  NULL);
    XmStringFree(label);
    XtManageChild(*button);
  }
  CreateMenuButtons(*menu, desc, size);
}

static void CreateMenuButtons(Widget menu, MenuDesc desc, int size)
{
  XmString label;
  int i, sepcount = 0;
  int nargs;
  Arg args[3];
  
  for (i = 0; i < size; i++) {
    if (desc[i].id == SEPARATOR_ID)
      XtManageChild(XmCreateSeparator(menu, "Separator", NULL, 0));
    else {
      nargs = 0;
      if (desc[i].mnemonic) {
	XtSetArg(args[nargs],
		 XmNmnemonic, XStringToKeysym(desc[i].mnemonic));
	nargs++;
      }
      if (desc[i].accelerator) {
	XtSetArg(args[nargs],
		 XmNaccelerator, desc[i].accelerator);
	nargs++;
      }
      if (desc[i].accelerator_text) {
	label = XmStringCreateLocalized(desc[i].accelerator_text);
	XtSetArg(args[nargs],
		 XmNacceleratorText, label);
	nargs++;
      }
      switch (desc[i].id) {
      case PUSH_BUTTON_ID:
	*desc[i].button =
	  XmCreatePushButton(menu, desc[i].button_name,
			     args, nargs);
	break;
      case CHECK_BUTTON_ID:
      case RADIO_BUTTON_ID:
	*desc[i].button =
	  XmCreateToggleButton(menu, desc[i].button_name,
			       args, nargs);
	XtVaSetValues(*desc[i].button,
		      XmNindicatorType,
		      desc[i].id == CHECK_BUTTON_ID?XmN_OF_MANY:
		      XmONE_OF_MANY,
		      NULL);
	break;
      case CASCADE_BUTTON_ID:
	CreateMenu(menu, MENU_PULLDOWN,
		   desc[i].label,
		   desc[i].mnemonic,
		   desc[i].accelerator,
		   desc[i].accelerator_text,
		   desc[i].submenu, desc[i].submenu_size,
		   desc[i].callback,
		   desc[i].unmap_callback,
		   desc[i].pulldown_name, desc[i].pulldown,
		   desc[i].button_name, desc[i].button);
	break;
      default:
	/* shouldn't happen */
	fatal("invalid menu description");
      }
      if (desc[i].accelerator_text)
	XmStringFree(label);
      label = XmStringCreateLocalized(desc[i].label);
      XtVaSetValues(*desc[i].button,
		    XmNlabelString, label,
		    NULL);
      XmStringFree(label);
      if (desc[i].callback)
	switch(desc[i].id) {
	case PUSH_BUTTON_ID:
	  XtAddCallback(*desc[i].button,
			XmNactivateCallback, desc[i].callback,
			(XtPointer)desc[i].data);
	  break;
	case CHECK_BUTTON_ID:
	case RADIO_BUTTON_ID:
	  XtAddCallback(*desc[i].button,
			XmNvalueChangedCallback, desc[i].callback,
			(XtPointer)desc[i].data);
	  break;
	}
      XtManageChild(*desc[i].button);
    }
  }
}

/* window title: *************************************************************/

static void UpdateTitle(void)
{
  char title[4*MAXPATHLEN+7], icon_name[MAXPATHLEN+1];

  *title = *icon_name = '\0';
  if (!treepane && !filepane && shelfpane) {
    strcpy(title, shelf_label(shelf_no));
    strcpy(icon_name, shelf_label(shelf_no));
  }
  if (*title) strcat(title, " - ");
  if (show_getwd && dirGetwd(curdir) &&
      strcmp(dirName(curdir), dirGetwd(curdir)))
    sprintf(title+strlen(title), "%s [%s]", dirName(curdir), dirGetwd(curdir));
  else
    strcat(title, dirName(curdir));
  if (!*icon_name) strcpy(icon_name, dirName(curdir));
  if (!*title) strcpy(title, progname);
  if (!*icon_name) strcpy(icon_name, progname);
  XtVaSetValues(app_shell, XmNtitle, title, NULL);
  XtVaSetValues(app_shell, XmNiconName, icon_name, NULL);
}

/* file/directory views: ****************************************************/

/* adjust the cell size of a container */

static void AdjustCellSize(Widget cont)
{
  unsigned char type;
  int count, i, n;
  WidgetList gadgets;
  Dimension cell_wd, cell_ht;

  XtVaGetValues(cont, XmNentryViewType, &type, NULL);
  if (type == XmLARGE_ICON)
    XtVaGetValues(cont, XmNlargeCellWidth, &cell_wd, XmNlargeCellHeight,
		  &cell_ht, NULL);
  else
    XtVaGetValues(cont, XmNsmallCellWidth, &cell_wd, XmNsmallCellHeight,
		  &cell_ht, NULL);
  XtVaGetValues(cont, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (count = i = 0; i < n; i++)
    if (XtIsManaged(gadgets[i]) &&
	XtClass(gadgets[i]) == xmIconGadgetClass) {
      Dimension gadget_wd, gadget_ht, border_wd;
      
      XtVaGetValues(gadgets[i], XmNwidth, &gadget_wd, XmNheight, &gadget_ht,
		    XmNborderWidth, &border_wd, NULL);
      if (gadget_wd + 2 * border_wd > cell_wd)
	cell_wd = gadget_wd + 2 * border_wd;
      if (gadget_ht + 2 * border_wd > cell_ht)
	cell_ht = gadget_ht + 2 * border_wd;
      count++;
    }
  if (type == XmLARGE_ICON)
    XtVaSetValues(cont, XmNlargeCellWidth, cell_wd, XmNlargeCellHeight,
		  cell_ht, NULL);
  else
    XtVaSetValues(cont, XmNsmallCellWidth, cell_wd, XmNsmallCellHeight,
		  cell_ht, NULL);
}

/* calculate the number of icon gadget children and the cell size of a
   container with spatial layout type */

static void GetCellParams(Widget cont, Dimension *w, Dimension *h, int *count)
{
  unsigned char type;
  Cardinal numChildren;
  WidgetList children;
  int i;

  XtVaGetValues(cont, XmNentryViewType, &type, NULL);
  if (type == XmLARGE_ICON)
    XtVaGetValues(cont, XmNlargeCellWidth, w, XmNlargeCellHeight,
		  h, NULL);
  else
    XtVaGetValues(cont, XmNsmallCellWidth, w, XmNsmallCellHeight,
		  h, NULL);
  XtVaGetValues(cont,
		XmNnumChildren, &numChildren,
		XmNchildren, &children,
		NULL);
  for (*count = i = 0; i < numChildren; i++)
    if (XtIsManaged(children[i]) &&
	XtClass(children[i]) == xmIconGadgetClass)
      (*count)++;
}

/* calculate the height of a container in outline or detail mode */

static int ContHeight(Widget cont)
{
  Cardinal numChildren;
  WidgetList children;
  int i;
  Dimension ht, wd, cont_ht;

  XtVaGetValues(cont,
		XmNnumChildren, &numChildren,
		XmNchildren, &children,
		NULL);
  if (cont == file_scroll) {
    for (cont_ht = 0, i = 0; i < numChildren; i++)
      if (XtIsManaged(children[i]) &&
	  XtClass(children[i]) == xmIconGadgetClass) {
	XtVaGetValues(children[i], XmNheight, &ht, XmNborderWidth, &wd, NULL);
	cont_ht += ht + 2 * wd;
      }
  } else {
    /* same as above, but ignore placeholder gadgets in tree container */
    for (cont_ht = 0, i = 0; i < numChildren; i++)
      if (XtIsManaged(children[i]) &&
	  XtClass(children[i]) == xmIconGadgetClass) {
	DirPtr dir;

	XtVaGetValues(children[i], XmNuserData, &dir,
		      XmNheight, &ht, XmNborderWidth, &wd, NULL);
	if (dir)
	  cont_ht += ht + 2 * wd;
      }
  }
  return cont_ht;
}

/* resize a container according to the current size of its clip window */

static void Resize(Widget scroll)
{
  unsigned char type;
  Dimension clipw, cliph, scrollw, scrollh, margw, margh, cellw, cellh;
  int n, ncols, nlines;

  if (scroll == None) return;

  /* determine the layout type of the container */

  XtVaGetValues(scroll, XmNlayoutType, &type, NULL);

  /* get the current geometry of clip window and scroll */

  XtVaGetValues(XtParent(scroll),
		XmNwidth, &clipw,
		XmNheight, &cliph,
		NULL);
  XtVaGetValues(scroll,
		XmNwidth, &scrollw,
		XmNheight, &scrollh,
		XmNmarginWidth, &margw,
		XmNmarginHeight, &margh,
		NULL);

  /* Setting the new width or height of the scroll to the exact value of
     clipw or cliph sometimes creates a scrollbar (Motif bug?). Decrementing
     these values by 2 seems to work though. */

  clipw -= 2;
  cliph -= 2;

  if (type == XmSPATIAL) {

    /* calculate the new number of columns and lines to display */

    GetCellParams(scroll, &cellw, &cellh, &n);
    if (cellw)
      ncols = (clipw - 2 * margw) / cellw;
    else
      ncols = 1;
    if (ncols * cellw + 2 * margw > clipw)
      ncols--;
    if (!ncols) ncols = 1;
    nlines = n / ncols;
    if (nlines * ncols < n) nlines++;
  
    /* set the new width and height of the scroll */
  
    scrollh = nlines * cellh + 2 * margh;
    XtVaSetValues(scroll,
		  XmNwidth, clipw,
		  XmNheight, (scrollh > cliph) ? scrollh : cliph,
		  NULL);

  } else {
    scrollh = ContHeight(scroll);
    XtVaSetValues(scroll,
		  XmNheight, (scrollh > cliph) ? scrollh : cliph,
		  NULL);
  }
  
}

static Boolean ResizeWorkProc(Widget scroll)
{
  /* make sure that the scroll is still valid */
  if (scroll != None &&
      (scroll == tree_scroll || scroll == file_scroll ||
       scroll == shelf_scroll))
    Resize(scroll);
  return True;
}

static void ResizeCB(Widget clip, Widget scroll, XtPointer widget_data)
{
  if (scroll != None && XtIsRealized(scroll)) {
    /* be sure to invoke this as a background procedure, otherwise scrolled
       window widget is not updated properly */
    XtAppAddWorkProc(app, (XtWorkProc)ResizeWorkProc, scroll);
  }
}

/* traverse to obscured widgets in scrolled windows */

static void TravObscuredCB(Widget w, XtPointer app_data,
		    XmTraverseObscuredCallbackStruct *tocs)
{
  XmScrollVisible(w, tocs->traversal_destination, 0, 0);
}

/* Terrible kludge to force setting of a pane's geometry during update
   or remapping of a pane. This is achieved by setting XmNpane{Min,Max}imum
   accordingly. */

static void FreezePaneGeom(Widget pane, Dimension *save_min,
			   Dimension *save_max)
{
  unsigned char orientation;
  Dimension wd, ht;

  XtVaGetValues(XtParent(pane), XmNorientation, &orientation, NULL);
  XtVaGetValues(pane,
		XmNpaneMinimum, save_min,
		XmNpaneMaximum, save_max,
		XmNwidth, &wd,
		XmNheight, &ht,
		NULL);
  if (orientation == XmVERTICAL)
    XtVaSetValues(pane,
		  XmNpaneMinimum, ht,
		  XmNpaneMaximum, ht,
		  NULL);
  else
    XtVaSetValues(pane,
		  XmNpaneMinimum, wd,
		  XmNpaneMaximum, wd,
		  NULL);
}

/* revert to previous XmNpane{Min,Max}imum settings */

static void UnfreezePaneGeom(Widget pane, Dimension save_min,
			     Dimension save_max)
{
  XtVaSetValues(pane,
		XmNpaneMinimum, save_min,
		XmNpaneMaximum, save_max,
		NULL);
}

/* save and restore the slider positions of the scrollbars during update
   of a pane */

static void SaveScrollBarValue(Widget pane, Boolean vert, Dimension *save_pos)
{
  Widget scroll, scrollbar;
  Dimension scroll_sz;
  int sb_min, sb_max, sb_val;

  XtVaGetValues(pane, XmNworkWindow, &scroll,
		vert?XmNverticalScrollBar:XmNhorizontalScrollBar,
		&scrollbar, NULL);
  if (scroll != None)
    XtVaGetValues(scroll, vert?XmNheight:XmNwidth, &scroll_sz, NULL);
  else
    scroll_sz = 0;
  if (scrollbar != None)
    XtVaGetValues(scrollbar, XmNminimum, &sb_min, XmNmaximum, &sb_max,
		  XmNvalue, &sb_val, NULL);
  else
    sb_min = 0, sb_max = scroll_sz, sb_val = 0;
  /* translate the slider position to a position in the scroll */
  if (sb_max > sb_min)
    *save_pos = sb_val * scroll_sz / (sb_max - sb_min);
  else
    *save_pos = 0;
}

static void SaveScrollBarValues(Widget pane, Dimension *vert_pos,
				Dimension *horiz_pos)
{
  SaveScrollBarValue(pane, True, vert_pos);
  SaveScrollBarValue(pane, False, horiz_pos);
}

static void RestoreScrollBarValue(Widget pane, Boolean vert,
				  Dimension save_pos)
{
  Widget scroll, scrollbar;
  Dimension scroll_sz;
  int sb_min, sb_max, sb_val, sb_sld;

  XtVaGetValues(pane, XmNworkWindow, &scroll,
		vert?XmNverticalScrollBar:XmNhorizontalScrollBar,
		&scrollbar, NULL);
  if (scroll != None)
    XtVaGetValues(scroll, vert?XmNheight:XmNwidth, &scroll_sz, NULL);
  else
    scroll_sz = 0;
  if (scroll_sz > 0 && scrollbar != None) {
    XtVaGetValues(scrollbar, XmNminimum, &sb_min, XmNmaximum, &sb_max,
		  XmNsliderSize, &sb_sld, NULL);
    /* translate the saved position value to a slider position in the
       new scrollbar */
    sb_val = save_pos * (sb_max - sb_min) / scroll_sz;
    if (sb_val > sb_max - sb_sld)
      sb_val = sb_max - sb_sld;
    /* establish the new slider position; this requires that we reset the
       slider position first (Motif bug?) */
    XmScrollBarSetValues(scrollbar, sb_min, 0, 0, 0, False);
    XmScrollBarSetValues(scrollbar, sb_val, 0, 0, 0, True);
  }
}

static void RestoreScrollBarValues(Widget pane, Dimension vert_pos,
				   Dimension horiz_pos)
{
  RestoreScrollBarValue(pane, True, vert_pos);
  RestoreScrollBarValue(pane, False, horiz_pos);
}

/* reset the slider position to the minimum value */

static void ResetScrollBarValue(Widget pane, Boolean vert)
{
  Widget scrollbar;
  int sb_min;

  XtVaGetValues(pane, vert?XmNverticalScrollBar:XmNhorizontalScrollBar,
		&scrollbar, NULL);
  if (scrollbar != None) {
    XtVaGetValues(scrollbar, XmNminimum, &sb_min, NULL);
    XmScrollBarSetValues(scrollbar, sb_min, 0, 0, 0, True);
  }
}

static void ResetScrollBarValues(Widget pane)
{
  ResetScrollBarValue(pane, True);
  ResetScrollBarValue(pane, False);
}

/* file popup handler */

static void FilePopupDestroyCB(Widget w, XtPointer app_data,
			       XtPointer widget_data)
{
  file_popup = NULL;
}

static void UnmapCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  lock = False;
}

static void FilePopupH(Widget w, XtPointer client_data, XEvent *event)
{
  XButtonPressedEvent *button_event = (XButtonPressedEvent *) event;
  Widget gadget = XmObjectAtPoint(w, button_event->x, button_event->y);
  DirPtr dir = (w == file_scroll)?curdir:shelfdir;

  if (button_event->button == 3 && gadget != None) {
    int item, i, n;
    WidgetList gadgets;

    XtVaGetValues(gadget, XmNuserData, &item, NULL);
    if (item == NONE) return;
    if (file_popup)
      XtDestroyWidget(file_popup);
    XmProcessTraversal(gadget, XmTRAVERSE_CURRENT);
    lock = True;
    XtVaGetValues(w, XmNselectedObjectCount, &n,
		  XmNselectedObjects, &gadgets, NULL);
    for (i = 0; i < n; i++)
      XtVaSetValues(gadgets[i], XmNvisualEmphasis, XmNOT_SELECTED, NULL);
    XtVaSetValues(gadget, XmNvisualEmphasis, XmSELECTED, NULL);
    XmUpdateDisplay(w);
    dirDeselect(dir, ALL, NULL);
    dirSelect(dir, item, NULL);
    UpdateStatusLine();
    UpdateMenus();
    CreateFilePopupDesc(dir, item);
    CreateMenu(w, MENU_POPUP, NULL, NULL, NULL, NULL,
	       file_popup_desc, file_popup_size,
	       NULL, UnmapCB,
	       "file_popup", &file_popup,
	       NULL, NULL);
    XtAddCallback(file_popup, XmNdestroyCallback, FilePopupDestroyCB, NULL);
    XmMenuPosition(file_popup, button_event);
    XtManageChild(file_popup);
  }
}

/* incremental search in the file and shelf pane */

static Widget is_w = None;
static WidgetList is_gadgets = NULL;
static int is_num = 0, is_act = 0, is_count = 0;
static char is_str[MAXPATHLEN+1];
static DirPtr is_dir;

static void is_init(Widget w)
{
  is_count = 0;
  *is_str = '\0';
  if (w == file_scroll || w == shelf_scroll)
    is_w = w;
  else
    is_w = None;
  if (w) {
    XtVaGetValues(w, XmNnumChildren, &is_num, XmNchildren, &is_gadgets,
		  NULL);
    is_dir = w == file_scroll ? curdir :
      w == shelf_scroll ? shelfdir : NULL;
  } else {
    is_num = 0;
    is_gadgets = NULL;
    is_dir = NULL;
  }
  is_act = 0;
  RestoreStatusLine();
}

static void is_next(void)
{
  int is_act0 = is_act;
  if (!is_w || !is_gadgets || !is_dir) return;
  while (is_act < is_num) {
    Widget gadget = is_gadgets[is_act];
    if (XtIsManaged(gadget) &&
	XtClass(gadget) == xmIconGadgetClass) {
      int i;
      XtVaGetValues(gadget, XmNuserData, &i, NULL);
      if (i == NONE || !prefix(is_str, fileName(dirFile(is_dir, i))))
	is_act++;
      else {
	int l = strlen(is_str) + 10;
	String msg = alloca((l+1) * sizeof(char));
	sprintf(msg, "I-search: %s", is_str);
	XmProcessTraversal(gadget, XmTRAVERSE_CURRENT);
	StatusMessage(msg);
	return;
      }
    } else
      is_act++;
  }
  is_act = is_act0;
  if (is_count)
    is_str[--is_count] = '\0';
  XBell(display, 100);
}

static void IsearchH(Widget w, XtPointer client_data, XEvent *event)
{
  if (w != file_scroll && w != shelf_scroll) return;
  if (event->type == KeyPress) {
    KeySym keysym;
    unsigned char c;
    int res = XLookupString(&(event->xkey), &c, 1, &keysym, NULL);

    if (!res || c <= ' ' ||
	(event->xkey.state & (Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))) {
      /* special keys */
      if (!IsModifierKey(keysym)) is_init(w);
      return;
    }
    /* ordinary key */
    if (w != is_w) is_init(w);
    if (is_count >= MAXPATHLEN)
      XBell(display, 100);
    else {
      is_str[is_count++] = c;
      is_str[is_count] = '\0';
      is_next();
    }
  } else
    is_init(w);
}

/* keep track of the input focus */

static void FocusChangeH(Widget w, XtPointer client_data, XEvent *event)
{
  DirPtr dir; int i;
  XFocusChangeEvent *focus_event = (XFocusChangeEvent *) event;
  Widget gadget = XmGetFocusWidget(app_shell);
  if (gadget != None && XtIsManaged(gadget) &&
      XtClass(gadget) == xmIconGadgetClass &&
      XtParent(gadget) == w)
    if (focus_event->type == FocusIn) {
      XtVaSetValues(w, XmNinitialFocus, gadget, NULL);
      if (is_count && is_w)
	if (is_w != XtParent(gadget) || is_act >= is_num ||
	    gadget != is_gadgets[is_act])
	  is_init(None);
    }
}

/* wheel mouse support (assumes the wheel is mapped to buttons 4 and 5) */

static void ScrollH(Widget w, XtPointer client_data, XEvent *event)
{
  String action, params[1];
  XButtonPressedEvent *button_event = (XButtonPressedEvent *) event;
  Widget pane = (Widget)client_data, scrollbar = NULL;
  if (button_event->button != 4 && button_event->button != 5) return;
  XtVaGetValues(pane, XmNverticalScrollBar, &scrollbar, NULL);
  if (!scrollbar) return;
  if (button_event->button == 4) {
    action = "IncrementUpOrLeft"; params[0] = "Up";
  } else {
    action = "IncrementDownOrRight"; params[0] = "Down";
  }
  XtCallActionProc(scrollbar, action, event, params, XtNumber(params)); 
}

/* destroy callbacks (remove event handlers from the scrolls) */

static void DestroyCB(Widget w, XtPointer app_data,
		      XtPointer widget_data)
{
  XtRemoveEventHandler(w, ButtonPressMask, False,
		       (XtEventHandler)FilePopupH, NULL);
  if (keyboardFocusPolicy == XmEXPLICIT)
    XtRemoveEventHandler(w, KeyPressMask|ButtonPressMask, False,
			 (XtEventHandler)IsearchH, NULL);
  XtRemoveEventHandler(w, FocusChangeMask, False,
		       (XtEventHandler)FocusChangeH, NULL);
  XtRemoveEventHandler(w, ButtonReleaseMask, False,
		       (XtEventHandler)ScrollH, NULL);
}

static void TreeDestroyCB(Widget w, XtPointer app_data,
			  XtPointer widget_data)
{
  XtRemoveEventHandler(w, FocusChangeMask, False,
		       (XtEventHandler)FocusChangeH, NULL);
  XtRemoveEventHandler(w, ButtonReleaseMask, False,
		       (XtEventHandler)ScrollH, NULL);
}

/* drag and drop */

/* finish a drag and drop transaction */

static void DragDropFinishCB(Widget w, Widget drag_icon, XtPointer widget_data)
{
  if (drag_icon)
    XtDestroyWidget(drag_icon);
}

/* start a drag */

static void FileStartDrag(Widget w, XEvent *event)
{
  XButtonPressedEvent *button_event = (XButtonPressedEvent *) event;
  Widget gadget = XmObjectAtPoint(w, button_event->x, button_event->y);
  DirPtr dir = (w == file_scroll)?curdir:shelfdir;

  if (gadget) {
    int i, n;
    WidgetList gadgets;
    Arg args[10];
    FilePtr file;
    IconPtr icon;
    Widget drag_context, drag_icon;

    XtVaGetValues(gadget, XmNuserData, &i, NULL);
    if (i == NONE) return;
    /* make sure we don't get an update during dragging */
    lock = True;
    /* set focus to the dragged icon gadget and select it if necessary */
    XmProcessTraversal(gadget, XmTRAVERSE_CURRENT);
    file = dirFile(dir, i);
    if (!fileSelected(file)) {
      int j, m;

      XtVaGetValues(w, XmNselectedObjectCount, &m,
		    XmNselectedObjects, &gadgets, NULL);
      for (j = 0; j < m; j++)
	XtVaSetValues(gadgets[j], XmNvisualEmphasis, XmNOT_SELECTED, NULL);
      XtVaSetValues(gadget, XmNvisualEmphasis, XmSELECTED, NULL);
      dirDeselect(dir, ALL, NULL);
      dirSelect(dir, i, NULL);
      UpdateStatusLine();
      UpdateMenus();
    }
    XmUpdateDisplay(w);
    /* create the drag icon */
    if (dirNFilesSelected(dir) > 1)
      icon = &std_icons[ICON_FILES];
    else
      icon = fileLargeIcon(file);
    n = 0;
    XtSetArg(args[n], XmNpixmap, icon->pixmap); n++;
    XtSetArg(args[n], XmNdepth,
	     DefaultDepth(display, screen_no)); n++;
    if (icon->mask != None) {
      XtSetArg(args[n], XmNmask, icon->mask); n++;
    }
    XtSetArg(args[n], XmNwidth, icon->width); n++;
    XtSetArg(args[n], XmNheight, icon->height); n++;
    drag_icon = XmCreateDragIcon(w, "drag_icon", args, n);
    /* create the drag context */
    n = 0;
    XtSetArg(args[n], XmNdragOperations,
	     XmDROP_MOVE|XmDROP_COPY|XmDROP_LINK); n++;
    XtSetArg(args[n], XmNsourcePixmapIcon, drag_icon); n++;
    drag_context = XmeDragSource(w, NULL, event, args, n);
    XtAddCallback(drag_context, XmNdragDropFinishCallback,
		  (XtCallbackProc)DragDropFinishCB, drag_icon);
    XtAddCallback(drag_context, XmNdropFinishCallback,
		  (XtCallbackProc)DropFinishCB, NULL);
  }
}

DirPtr tree_drag_dir;

static void TreeStartDrag(Widget w, XEvent *event)
{
  XButtonPressedEvent *button_event = (XButtonPressedEvent *) event;
  Widget gadget = XmObjectAtPoint(w, button_event->x, button_event->y);

  if (gadget) {
    int n;
    WidgetList gadgets;
    Arg args[10];
    DirPtr dir;
    IconPtr icon;
    Widget drag_context, drag_icon;

    XtVaGetValues(gadget, XmNuserData, &dir, NULL);
    if (!dir || !strcmp(dirName(dir), "/")) return;
    XmProcessTraversal(gadget, XmTRAVERSE_CURRENT);
    /* make sure we don't get an update during dragging */
    lock = True;
    /* create the drag icon */
    icon = dirLargeIcon(dir);
    n = 0;
    XtSetArg(args[n], XmNpixmap, icon->pixmap); n++;
    XtSetArg(args[n], XmNdepth,
	     DefaultDepth(display, screen_no)); n++;
    if (icon->mask != None) {
      XtSetArg(args[n], XmNmask, icon->mask); n++;
    }
    XtSetArg(args[n], XmNwidth, icon->width); n++;
    XtSetArg(args[n], XmNheight, icon->height); n++;
    drag_icon = XmCreateDragIcon(w, "drag_icon", args, n);
    /* create the drag context */
    tree_drag_dir = dir;
    n = 0;
    XtSetArg(args[n], XmNdragOperations,
	     XmDROP_MOVE|XmDROP_COPY|XmDROP_LINK); n++;
    XtSetArg(args[n], XmNsourcePixmapIcon, drag_icon); n++;
    drag_context = XmeDragSource(w, dir, event, args, n);
    XtAddCallback(drag_context, XmNdragDropFinishCallback,
		  (XtCallbackProc)DragDropFinishCB, drag_icon);
    XtAddCallback(drag_context, XmNdropFinishCallback,
		  (XtCallbackProc)DropFinishCB, NULL);
  }
}

/* register drop sites */

static void DropRegister(Widget w)
{
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom FILE = XInternAtom(display, "FILE", False);
  Atom targets[2];
  int n = 0;
  Arg args[10];

  targets[0] = FILES;
  targets[1] = FILE;
  XtSetArg(args[n], XmNdropSiteOperations,
	   XmDROP_MOVE|XmDROP_COPY|XmDROP_LINK); n++;
  XtSetArg(args[n], XmNimportTargets, targets); n++;
  XtSetArg(args[n], XmNnumImportTargets, 2); n++;
  XtSetArg(args[n], XmNdragProc, DragProcCB); n++;
  XtSetArg(args[n], XmNanimationStyle, XmDRAG_UNDER_NONE); n++;
  if (XmDropSiteRegistered(w))
    XmDropSiteUnregister(w);
  XmeDropSink(w, args, n);
}

/* icon gadget creation */

#define ICON_FILE_LINK_BAD_NAME	"file_link_bad_icon"
#define ICON_DIR_LINK_NAME	"dir_link_icon"
#define ICON_DIR_UP_NAME	"dir_up_icon"
#define ICON_DIR_NAME		"dir_icon"
#define ICON_EXEC_LINK_NAME	"exec_link_icon"
#define ICON_EXEC_NAME		"exec_icon"
#define ICON_FILE_LINK_NAME	"file_link_icon"
#define ICON_FILE_NAME		"file_icon"

static String icon_name(String name, StatPtr lstats, StatPtr stats)
{
  if (S_ISLNK(stats->st_mode))
    return ICON_FILE_LINK_BAD_NAME;
  else if (S_ISDIR(stats->st_mode))
    if (S_ISLNK(lstats->st_mode))
      return ICON_DIR_LINK_NAME;
    else if (!(strcmp(name, "..")))
      return ICON_DIR_UP_NAME;
    else
      return ICON_DIR_NAME;
  else if (stats->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
    if (S_ISLNK(lstats->st_mode))
      return ICON_EXEC_LINK_NAME;
    else
      return ICON_EXEC_NAME;
  else if (S_ISLNK(lstats->st_mode))
    return ICON_FILE_LINK_NAME;
  else
    return ICON_FILE_NAME;
}

static Widget CreateDummyGadget(Widget parent, String name, XtPointer data)
{
  Widget icon_gadget =
    XtVaCreateManagedWidget(name, xmIconGadgetClass, parent, NULL);
  Pixel background;
  
  /* The following kludge removes shadows from the label. Setting the
     shadow thickness to zero does not work here since the pixmap may
     be clipped when the label string is shorter than the pixmap's width
     and the highlight thickness is greater than the shadow thickness
     (Motif bug?). */
  XtVaGetValues(icon_gadget, XmNbackground, &background, NULL);
  XtVaSetValues(icon_gadget,
		XmNtopShadowColor, background,
		XmNbottomShadowColor, background,
		XmNuserData, data,
		NULL);
  return icon_gadget;
}

static Widget CreateSmallIconGadget(Widget parent, DirPtr dir, int i)
{
  FilePtr file = dirFile(dir, i);
  XmString label = XmStringCreateLocalized(fileName(file));
  String name = icon_name(fileName(file), fileLstats(file), fileStats(file));
  Widget icon_gadget =
    XtVaCreateManagedWidget(name, xmIconGadgetClass, parent,
			    XmNlabelString, label,
			    XmNsmallIconPixmap,
			    fileSmallIcon(file)->pixmap,
			    XmNsmallIconMask,
			    fileSmallIcon(file)->mask,
			    NULL);
  Pixel background;
  
  XtVaGetValues(icon_gadget, XmNbackground, &background, NULL);
  XtVaSetValues(icon_gadget,
		XmNtopShadowColor, background,
		XmNbottomShadowColor, background,
		XmNvisualEmphasis,
		fileSelected(file)?XmSELECTED:XmNOT_SELECTED,
		XmNuserData, i,
		NULL);
  XmStringFree(label);
  return icon_gadget;
}

static Widget CreateLargeIconGadget(Widget parent, DirPtr dir, int i)
{
  FilePtr file = dirFile(dir, i);
  XmString label = XmStringCreateLocalized(fileName(file));
  String name = icon_name(fileName(file), fileLstats(file), fileStats(file));
  Widget icon_gadget =
    XtVaCreateManagedWidget(name, xmIconGadgetClass, parent,
			    XmNlabelString, label,
			    XmNlargeIconPixmap,
			    fileLargeIcon(file)->pixmap,
			    XmNlargeIconMask,
			    fileLargeIcon(file)->mask,
			    NULL);
  Pixel background;
  
  XtVaGetValues(icon_gadget, XmNbackground, &background, NULL);
  XtVaSetValues(icon_gadget,
		XmNtopShadowColor, background,
		XmNbottomShadowColor, background,
		XmNvisualEmphasis,
		fileSelected(file)?XmSELECTED:XmNOT_SELECTED,
		XmNuserData, i,
		NULL);
  XmStringFree(label);
  return icon_gadget;
}

static Widget CreateDetailGadget(Widget parent, DirPtr dir, int i)
{
  FilePtr file = dirFile(dir, i);
  XmString label = XmStringCreateLocalized(fileName(file));
  String name = icon_name(fileName(file), fileLstats(file), fileStats(file));
  Widget icon_gadget =
    XtVaCreateManagedWidget(name, xmIconGadgetClass, parent,
			    XmNlabelString, label,
			    XmNsmallIconPixmap,
			    fileSmallIcon(file)->pixmap,
			    XmNsmallIconMask,
			    fileSmallIcon(file)->mask,
			    NULL);
  Pixel background;
  XmString details[5];
  int j, n = 0;
  struct passwd *pw;
  struct group *gr;
  char s[100];

  /* size */
  sprintf(s, "%lu", (unsigned long) fileLstats(file)->st_size);
  details[n++] = XmStringCreateLocalized(s);

  /* owner */
  if ((pw = getpwuid(fileLstats(file)->st_uid)) == NULL) {
    sprintf(s, "%lu", (unsigned long) fileLstats(file)->st_uid);
    details[n++] = XmStringCreateLocalized(s);
  } else
    details[n++] = XmStringCreateLocalized(pw->pw_name);

  /* group */
  if ((gr = getgrgid(fileLstats(file)->st_gid)) == NULL) {
    sprintf(s, "%lu", (unsigned long) fileLstats(file)->st_gid);
    details[n++] = XmStringCreateLocalized(s);
  } else
    details[n++] = XmStringCreateLocalized(gr->gr_name);

  /* permissions */
  sprintf(s, "%c%c%c%c%c%c%c%c%c%c",
	  S_ISLNK(fileLstats(file)->st_mode) ? 'l' :
	  S_ISDIR(fileLstats(file)->st_mode) ? 'd' : '-',
	  (S_IRUSR & fileLstats(file)->st_mode) ? 'r' : '-',
	  (S_IWUSR & fileLstats(file)->st_mode) ? 'w' : '-',
	  (S_IXUSR & fileLstats(file)->st_mode) ? 'x' : '-',
	  (S_IRGRP & fileLstats(file)->st_mode) ? 'r' : '-',
	  (S_IWGRP & fileLstats(file)->st_mode) ? 'w' : '-',
	  (S_IXGRP & fileLstats(file)->st_mode) ? 'x' : '-',
	  (S_IROTH & fileLstats(file)->st_mode) ? 'r' : '-',
	  (S_IWOTH & fileLstats(file)->st_mode) ? 'w' : '-',
	  (S_IXOTH & fileLstats(file)->st_mode) ? 'x' : '-');
  if (S_ISUID & fileLstats(file)->st_mode)
    s[3] = (s[3] == 'x')?'s':'S';
  if (S_ISGID & fileLstats(file)->st_mode)
    s[6] = (s[6] == 'x')?'s':'S';
  if (S_ISVTX & fileLstats(file)->st_mode)
    s[9] = (s[9] == 'x')?'t':'T';
  details[n++] = XmStringCreateLocalized(s); 

  /* date */
  strcpy(s, ctime(&(fileLstats(file)->st_mtime)));
  if (*s)
    s[strlen(s)-1] = '\0';
  details[n++] = XmStringCreateLocalized(s);

  XtVaGetValues(icon_gadget, XmNbackground, &background, NULL);
  XtVaSetValues(icon_gadget,
		XmNtopShadowColor, background,
		XmNbottomShadowColor, background,
		XmNdetail, details,
		XmNdetailCount, n,
		XmNvisualEmphasis,
		fileSelected(file)?XmSELECTED:XmNOT_SELECTED,
		XmNuserData, i,
		NULL);
  XmStringFree(label);
  for (j = 0; j < n; j++)
    XmStringFree(details[j]);
  return icon_gadget;
}

static void CreateFileGadgets(Widget parent, DirPtr dir, ViewType viewtype)
{
  int i;
  
  /* make sure we have at least a dummy gadget in the container; otherwise
     Motif breaks (bug?) */
  if (!dirIsReadable(dir) || !dirHasFileInfo(dir))
    CreateDummyGadget(parent, "unreadable", (XtPointer)NONE);
  else if (dirNFiles(dir) == 0)
    CreateDummyGadget(parent, "empty", (XtPointer)NONE);
  else if (viewtype == LargeIconView)
    for (i = 0; i < dirNFiles(dir); i++)
      CreateLargeIconGadget(parent, dir, i);
  else if (viewtype == SmallIconView)
    for (i = 0; i < dirNFiles(dir); i++)
      CreateSmallIconGadget(parent, dir, i);
  else
    for (i = 0; i < dirNFiles(dir); i++)
      CreateDetailGadget(parent, dir, i);
}

static Widget CreateTreeGadget(Widget parent, Widget parent_entry,
			       DirPtr dir)
{
  char dirname[MAXPATHLEN+1];
  XmString label;
  String name;
  Widget icon_gadget;
  Pixel background;
  
  filepart(dirname, dirName(dir));
  label = XmStringCreateLocalized((dir != rootdir)?dirname:dirName(dir));
  name = icon_name(dirname, dirLstats(dir), dirStats(dir));
  icon_gadget =
    XtVaCreateManagedWidget(name, xmIconGadgetClass, parent,
			    XmNlabelString, label,
			    XmNsmallIconPixmap,
			    dirSmallIcon(dir)->pixmap,
			    XmNsmallIconMask,
			    dirSmallIcon(dir)->mask,
			    XmNentryParent, parent_entry,
			    XmNoutlineState,
			    dirHasSubdirInfo(dir)?XmEXPANDED:XmCOLLAPSED,
			    NULL);
  XtVaGetValues(icon_gadget, XmNbackground, &background, NULL);
  XtVaSetValues(icon_gadget,
		XmNtopShadowColor, background,
		XmNbottomShadowColor, background,
		XmNuserData, dir,
		NULL);
  XmStringFree(label);
  if (dir == curdir)
    curgadget = icon_gadget;
  return icon_gadget;
}

static void CreateTreeGadgets(Widget parent, Widget parent_entry, DirPtr dir)
{
  int i;
  Widget entry = CreateTreeGadget(parent, parent_entry, dir);
  
  if (dirHasSubdirInfo(dir))
    for (i = 0; i < dirNSubdirs(dir); i++)
      CreateTreeGadgets(parent, entry, dirSubdir(dir, i));
  else if (dirHasSubdirs(dir)) {
    /* create a placeholder entry */
    Widget subdir_entry = CreateDummyGadget(parent, "subdir", NULL);
    XtVaSetValues(subdir_entry, XmNentryParent, entry, NULL);
  }
}

/* default dimensions of the panes */

static Dimension view_wd, view_ht, tree_wd, tree_ht, file_wd, file_ht, 
  shelf_wd, shelf_ht, log_wd, log_ht;

/* create scrolls and the corresponding panes */

static void UpdateFileScroll(Widget parent)
{
  unsigned char sel;
  Pixel pix;
  if (!filepane) return;
  if (file_scroll != None) {
    if (is_w == file_scroll) is_init(None);
    XtDestroyWidget(file_scroll);
  }
  if (viewtype == LargeIconView)
    file_scroll =
      XtVaCreateWidget("file_scroll_large_icons",
		       xmContainerWidgetClass,
		       parent,
		       XmNentryViewType, XmLARGE_ICON,
		       XmNlayoutType, XmSPATIAL,
		       XmNspatialStyle, XmGRID,
		       XmNspatialResizeModel, XmGROW_MINOR,
		       XmNspatialSnapModel, XmCENTER,
		       XmNautomaticSelection, XmNO_AUTO_SELECT,
		       XmNprimaryOwnership, XmOWN_NEVER,
		       XmNtranslations, file_trans,
		       NULL);
  else if (viewtype == SmallIconView)
    file_scroll =
      XtVaCreateWidget("file_scroll_small_icons",
		       xmContainerWidgetClass,
		       parent,
		       XmNentryViewType, XmSMALL_ICON,
		       XmNlayoutType, XmSPATIAL,
		       XmNspatialStyle, XmGRID,
		       XmNspatialResizeModel, XmGROW_MINOR,
		       XmNspatialSnapModel, XmCENTER,
		       XmNautomaticSelection, XmNO_AUTO_SELECT,
		       XmNprimaryOwnership, XmOWN_NEVER,
		       XmNtranslations, file_trans,
		       NULL);
  else
    file_scroll =
      XtVaCreateWidget("file_scroll_detail",
		       xmContainerWidgetClass,
		       parent,
		       XmNentryViewType, XmSMALL_ICON,
		       XmNlayoutType, XmDETAIL,
		       XmNautomaticSelection, XmNO_AUTO_SELECT,
		       XmNoutlineButtonPolicy, XmOUTLINE_BUTTON_ABSENT,
		       XmNprimaryOwnership, XmOWN_NEVER,
		       XmNtranslations, file_trans,
		       NULL);
  XtVaGetValues(file_scroll, XmNbackground, &pix, NULL);
  XtVaSetValues(XtParent(file_scroll), XmNbackground, pix, NULL);
  XtVaGetValues(file_scroll, XmNselectionPolicy, &sel, NULL);
  if (sel != XmMULTIPLE_SELECT && sel != XmEXTENDED_SELECT)
    XtVaSetValues(file_scroll, XmNselectionPolicy, XmEXTENDED_SELECT,
		  NULL);
  CreateFileGadgets(file_scroll, curdir, viewtype);
  if (viewtype != DetailView)
    AdjustCellSize(file_scroll);
  DropRegister(file_scroll);
  XtAddCallback(file_scroll, XmNdefaultActionCallback,
		(XtCallbackProc) FileActionCB, NULL);
  XtAddCallback(file_scroll, XmNselectionCallback,
		(XtCallbackProc) FileSelectionCB, NULL);
  XtAddCallback(file_scroll, XmNconvertCallback,
		(XtCallbackProc) FileConvertCB, NULL);
  XtAddCallback(file_scroll, XmNdestinationCallback,
		(XtCallbackProc) FileDestinationCB, NULL);
  XtRemoveAllCallbacks(XtParent(file_scroll), XmNresizeCallback);
  XtAddCallback(XtParent(file_scroll), XmNresizeCallback,
		(XtCallbackProc) ResizeCB, file_scroll);
  XtAddEventHandler(file_scroll, ButtonPressMask, False,
		    (XtEventHandler)FilePopupH, NULL);
  if (keyboardFocusPolicy == XmEXPLICIT)
    XtAddEventHandler(file_scroll, KeyPressMask|ButtonPressMask, False,
		      (XtEventHandler)IsearchH, NULL);
  XtAddEventHandler(file_scroll, FocusChangeMask, False,
		    (XtEventHandler)FocusChangeH, NULL);
  XtAddEventHandler(file_scroll, ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddEventHandler(XtParent(file_scroll), ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddCallback(file_scroll, XmNdestroyCallback,
		(XtCallbackProc) DestroyCB, NULL);
}

static void CreateFileScroll(Widget parent)
{
  UpdateFileScroll(parent);
  if (file_scroll != None)
    XtManageChild(file_scroll);
  UpdateTitle();
}

static void CreateFilePane(Widget parent)
{
  file_pane = XtVaCreateWidget("file_scrollSW",
			       xmScrolledWindowWidgetClass,
			       parent,
			       XmNscrollingPolicy, XmAUTOMATIC,
			       NULL);
  XtVaGetValues(file_pane, XmNwidth, &file_wd, XmNheight, &file_ht, NULL);
  XtAddCallback(file_pane, XmNtraverseObscuredCallback,
		(XtCallbackProc) TravObscuredCB, NULL);
  CreateFileScroll(file_pane);
  if (filepane)
    XtManageChild(file_pane);
}

static void UpdateShelfScroll(Widget parent)
{
  unsigned char sel;
  Pixel pix;
  if (!shelfpane) return;
  if (shelf_scroll != None) {
    if (is_w == shelf_scroll) is_init(None);
    XtDestroyWidget(shelf_scroll);
  }
  shelf_scroll =
    XtVaCreateWidget("shelf_scroll",
		     xmContainerWidgetClass,
		     parent,
		     XmNentryViewType, XmLARGE_ICON,
		     XmNlayoutType, XmSPATIAL,
		     XmNspatialStyle, XmGRID,
		     XmNspatialResizeModel, XmGROW_MINOR,
		     XmNspatialSnapModel, XmCENTER,
		     XmNautomaticSelection, XmNO_AUTO_SELECT,
		     XmNprimaryOwnership, XmOWN_NEVER,
		     XmNtranslations, file_trans,
		     NULL);
  XtVaGetValues(shelf_scroll, XmNbackground, &pix, NULL);
  XtVaSetValues(XtParent(shelf_scroll), XmNbackground, pix, NULL);
  XtVaGetValues(shelf_scroll, XmNselectionPolicy, &sel, NULL);
  if (sel != XmMULTIPLE_SELECT && sel != XmEXTENDED_SELECT)
    XtVaSetValues(shelf_scroll, XmNselectionPolicy, XmEXTENDED_SELECT,
		  NULL);
  CreateFileGadgets(shelf_scroll, shelfdir, LargeIconView);
  AdjustCellSize(shelf_scroll);
  DropRegister(shelf_scroll);
  XtAddCallback(shelf_scroll, XmNdefaultActionCallback,
		(XtCallbackProc) ShelfActionCB, NULL);
  XtAddCallback(shelf_scroll, XmNselectionCallback,
		(XtCallbackProc) ShelfSelectionCB, NULL);
  XtAddCallback(shelf_scroll, XmNconvertCallback,
		(XtCallbackProc) ShelfConvertCB, NULL);
  XtAddCallback(shelf_scroll, XmNdestinationCallback,
		(XtCallbackProc) ShelfDestinationCB, NULL);
  XtRemoveAllCallbacks(XtParent(shelf_scroll), XmNresizeCallback);
  XtAddCallback(XtParent(shelf_scroll), XmNresizeCallback,
		(XtCallbackProc) ResizeCB, shelf_scroll);
  XtAddEventHandler(shelf_scroll, ButtonPressMask, False,
		    (XtEventHandler)FilePopupH, NULL);
  if (keyboardFocusPolicy == XmEXPLICIT)
    XtAddEventHandler(shelf_scroll, KeyPressMask|ButtonPressMask, False,
		      (XtEventHandler)IsearchH, NULL);
  XtAddEventHandler(shelf_scroll, FocusChangeMask, False,
		    (XtEventHandler)FocusChangeH, NULL);
  XtAddEventHandler(shelf_scroll, ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddEventHandler(XtParent(shelf_scroll), ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddCallback(shelf_scroll, XmNdestroyCallback,
		(XtCallbackProc) DestroyCB, NULL);
}

static void CreateShelfScroll(Widget parent)
{
  UpdateShelfScroll(parent);
  if (shelf_scroll != None)
    XtManageChild(shelf_scroll);
  UpdateTitle();
}

static void CreateShelfPane(Widget parent)
{
  shelf_pane = XtVaCreateWidget("shelf_scrollSW",
			       xmScrolledWindowWidgetClass,
			       parent,
			       XmNscrollingPolicy, XmAUTOMATIC,
			       NULL);
  XtVaGetValues(shelf_pane, XmNwidth, &shelf_wd, XmNheight, &shelf_ht, NULL);
  XtAddCallback(shelf_pane, XmNtraverseObscuredCallback,
		(XtCallbackProc) TravObscuredCB, NULL);
  CreateShelfScroll(shelf_pane);
  if (shelfpane)
    XtManageChild(shelf_pane);
}

static void UpdateTreeScroll(Widget parent)
{
  unsigned char sel;
  Pixel pix;
  if (!treepane) return;
  if (tree_scroll != None)
    XtDestroyWidget(tree_scroll);
  tree_scroll =
    XtVaCreateWidget("tree_scroll",
		     xmContainerWidgetClass,
		     parent,
		     XmNentryViewType, XmSMALL_ICON,
		     XmNlayoutType, XmOUTLINE,
		     XmNautomaticSelection, XmAUTO_SELECT,
		     XmNprimaryOwnership, XmOWN_NEVER,
		     XmNtranslations, tree_trans,
		     NULL);
  XtVaGetValues(tree_scroll, XmNbackground, &pix, NULL);
  XtVaSetValues(XtParent(tree_scroll), XmNbackground, pix, NULL);
  XtVaGetValues(tree_scroll, XmNselectionPolicy, &sel, NULL);
  if (sel != XmSINGLE_SELECT && sel != XmBROWSE_SELECT)
    XtVaSetValues(tree_scroll, XmNselectionPolicy, XmSINGLE_SELECT,
		  NULL);
  CreateTreeGadgets(tree_scroll, None, rootdir);
  XtVaSetValues(curgadget, XmNvisualEmphasis, XmSELECTED, NULL);
  DropRegister(tree_scroll);
  XtAddCallback(tree_scroll, XmNoutlineChangedCallback,
		(XtCallbackProc) TreeOutlineCB, NULL);
  XtAddCallback(tree_scroll, XmNselectionCallback,
		(XtCallbackProc) TreeSelectionCB, NULL);
  XtAddCallback(tree_scroll, XmNconvertCallback,
		(XtCallbackProc) TreeConvertCB, NULL);
  XtAddCallback(tree_scroll, XmNdestinationCallback,
		(XtCallbackProc) TreeDestinationCB, NULL);
  XtRemoveAllCallbacks(XtParent(tree_scroll), XmNresizeCallback);
  XtAddCallback(XtParent(tree_scroll), XmNresizeCallback,
		(XtCallbackProc) ResizeCB, tree_scroll);
  XtAddEventHandler(tree_scroll, FocusChangeMask, False,
		    (XtEventHandler)FocusChangeH, NULL);
  XtAddEventHandler(tree_scroll, ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddEventHandler(XtParent(tree_scroll), ButtonReleaseMask, False,
		    (XtEventHandler)ScrollH, parent);
  XtAddCallback(tree_scroll, XmNdestroyCallback,
		(XtCallbackProc) TreeDestroyCB, NULL);
}

static void CreateTreeScroll(Widget parent)
{
  UpdateTreeScroll(parent);
  if (tree_scroll != None)
    XtManageChild(tree_scroll);
  UpdateTitle();
}

static void CreateTreePane(Widget parent)
{
  tree_pane = XtVaCreateWidget("tree_scrollSW",
			       xmScrolledWindowWidgetClass,
			       parent,
			       XmNscrollingPolicy, XmAUTOMATIC,
			       NULL);
  XtVaGetValues(tree_pane, XmNwidth, &tree_wd, XmNheight, &tree_ht, NULL);
  XtAddCallback(tree_pane, XmNtraverseObscuredCallback,
		(XtCallbackProc) TravObscuredCB, NULL);
  CreateTreeScroll(tree_pane);
  if (treepane)
    XtManageChild(tree_pane);
}

static void CreateLogPane(Widget parent)
{
  log_scroll = XmCreateScrolledText(parent, "log_scroll", NULL, 0);
  XtVaSetValues(log_scroll, XmNeditable, False,	NULL);
  XtAddCallback(log_scroll, XmNpopupHandlerCallback, LogPopupCB, NULL);
  XtManageChild(log_scroll);
  CreateMenu(log_scroll, MENU_POPUP, NULL, NULL, NULL, NULL,
	     log_popup_desc, XtNumber(log_popup_desc),
	     NULL, NULL,
	     "log_popup", &log_popup,
	     NULL, NULL);
  XtVaSetValues(log_popup, XmNpopupEnabled, XmPOPUP_AUTOMATIC, NULL);
  log_pane = XtParent(log_scroll);
  XtVaGetValues(log_pane, XmNwidth, &log_wd, XmNheight, &log_ht, NULL);
  if (!logpane)
    XtUnmanageChild(log_pane);
}

/* create and update the shelf tabs */

static void CreateShelfTabs(Widget parent)
{
  String *labels = (String*)alloca(n_shelves()*sizeof(String));
  int i;
  for (i = 0; i < n_shelves(); i++)
    labels[i] = shelf_label(i)?shelf_label(i):"";
  shelf_tabs = XtVaCreateWidget("shelf_tabs",
				xgTabsWidgetClass,
				parent,
				XgNtabLabels, labels,
				XgNcurrentTab, shelf_no,
				XgNfocusTab, shelf_no,
				XgNtabCount, n_shelves(),
				NULL);
  XtAddCallback(shelf_tabs, XmNactivateCallback, (XtCallbackProc) ShelfCB,
		NULL);
  if (shelfpane)
    XtManageChild(shelf_tabs);
}

static void UpdateShelfTabs()
{
  String *labels = (String*)alloca(n_shelves()*sizeof(String));
  int i;
  for (i = 0; i < n_shelves(); i++)
    labels[i] = shelf_label(i)?shelf_label(i):"";
  XtVaSetValues(shelf_tabs,
		XgNtabLabels, labels,
		XgNcurrentTab, shelf_no,
		XgNfocusTab, shelf_no,
		XgNtabCount, n_shelves(),
		NULL);
}

/* create the file view */

static void CreateFileView(Widget parent)
{
  file_view = XtVaCreateWidget("file_view",
			       xmPanedWindowWidgetClass,
			       parent,
			       NULL);
  XtVaGetValues(file_view, XmNwidth, &view_wd, XmNheight, &view_ht, NULL);
  CreateTreePane(file_view);
  CreateFilePane(file_view);
  if (treepane || filepane)
    XtManageChild(file_view);
}

/* create the work area */

static void CreateWorkArea(Widget parent)
{
  frame = XtVaCreateManagedWidget("frame",
				  xmFrameWidgetClass,
				  parent,
				  NULL);
  work_area = XtVaCreateManagedWidget("work_area",
				      xmPanedWindowWidgetClass,
				      frame,
				      NULL);
  CreateShelfPane(work_area);
  CreateFileView(work_area);
  CreateLogPane(work_area);
}

/* create attachments for the main form */

static void CreateAttachments(void)
{
  if (shelfpane) {
    int loc;
    XtVaGetValues(shelf_tabs, XgNtabLocation, &loc, NULL);
    switch (loc) {
    case XgTOP:
      XtVaSetValues(shelf_tabs,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_NONE,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    NULL);
      XtVaSetValues(frame,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, shelf_tabs,
		    XmNbottomAttachment, XmATTACH_FORM,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    NULL);
      break;
    case XgBOTTOM:
      XtVaSetValues(shelf_tabs,
		    XmNbottomAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_NONE,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    NULL);
      XtVaSetValues(frame,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, shelf_tabs,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    NULL);
      break;
    case XgLEFT:
      XtVaSetValues(shelf_tabs,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_NONE,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    NULL);
      XtVaSetValues(frame,
		    XmNleftAttachment, XmATTACH_WIDGET,
		    XmNleftWidget, shelf_tabs,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    NULL);
      break;
    case XgRIGHT:
      XtVaSetValues(shelf_tabs,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNleftAttachment, XmATTACH_NONE,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    NULL);
      XtVaSetValues(frame,
		    XmNrightAttachment, XmATTACH_WIDGET,
		    XmNrightWidget, shelf_tabs,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    NULL);
      break;
    }
  } else
    XtVaSetValues(frame,
		  XmNtopAttachment, XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XmNleftAttachment, XmATTACH_FORM,
		  XmNrightAttachment, XmATTACH_FORM,
		  NULL);
}

/* create the main form */

static void CreateForm(Widget parent)
{
  form = XtVaCreateManagedWidget("form",
				 xmFormWidgetClass,
				 parent,
				 NULL);
  CreateShelfTabs(form);
  CreateWorkArea(form);
  CreateAttachments();
}

/* status line: *************************************************************/

static char status[1000];

static void update_status(void)
{
  int i;
  long bsize, ksize, free, free_blocks;
  long n_bytes, n_files, n_bytes_sel, n_files_sel;
  int res;
#ifdef USE_STATVFS
  struct statvfs stat;
#else
  struct statfs stat;
#endif

  status[0] = 0;

  /*
  if (!filepane && !treepane)
    return;
  */

  if (filepane && dirFilter(curdir))
    sprintf(status, "[%s] ", dirFilter(curdir));

  n_files = dirNFiles(curdir);
  n_bytes = dirNBytes(curdir);
  n_files_sel = dirNFilesSelected(curdir);
  n_bytes_sel = dirNBytesSelected(curdir);

  sprintf(status+strlen(status),
	  "%ld byte%s in %d item%s",
	  n_bytes, n_bytes == 1 ? "" : "s",
	  n_files, n_files == 1 ? "" : "s");
  if (filepane && n_files_sel)
    if (n_files_sel == 1) {
      int i;
      for (i = 0; i < dirNFiles(curdir); i++)
	if (fileSelected(dirFile(curdir, i))) {
	  String name = fileName(dirFile(curdir, i));
	  String desc = comment(fileType(dirFile(curdir, i)));
	  sprintf(status+strlen(status), ", %s", name);
	  if (desc)
	    sprintf(status+strlen(status), " (%s)", desc);
	  sprintf(status+strlen(status), ": %ld byte%s",
	      n_bytes_sel, n_bytes_sel == 1 ? "" : "s");
	  break;
	}
    } else
      sprintf(status+strlen(status), ", %ld byte%s in %d selected item%s",
	      n_bytes_sel, n_bytes_sel == 1 ? "" : "s",
	      n_files_sel, n_files_sel == 1 ? "" : "s");

  /* the following has been pilfered from moxfm -- thanks to Oliver Mai */

#ifdef USE_STATVFS
  res = statvfs(dirName(curdir), &stat);
#else
  res = statfs(dirName(curdir), &stat);
#endif
#ifdef OSF1
  bsize = (long) stat.f_fsize / 1024L;
  if (!bsize)  ksize = 1024L / (long) stat.f_fsize;
#else
  bsize = (long) stat.f_bsize / 1024L;
  if (!bsize)  ksize = 1024L / (long) stat.f_bsize;
#endif

  if (!res) {
    if (geteuid())
      free_blocks = stat.f_bavail;
    else
      free_blocks = stat.f_bfree;

    if (bsize) free = free_blocks * bsize;
    else  free = free_blocks / ksize;

    sprintf(status+strlen(status), ", %ld KB available", free);
  }
}

static int n_dev_indicators = 0;
static Widget *dev_indicators = NULL;
static int *devs = NULL;

static Pixel foreground, highlightColor;

static void UpdateDeviceIndicators(void)
{
  int i;
  if (check_mnt_table())
    get_mnt_table();
  for (i = 0; i < n_dev_indicators; i++) {
    XtSetSensitive(dev_indicators[i], True);
    if (premounted(devs[i]))
      XtVaSetValues(dev_indicators[i], XmNforeground, highlightColor, NULL);
    else
      XtVaSetValues(dev_indicators[i], XmNforeground, foreground, NULL);
    if (devcount(devs[i]))
      XtSetSensitive(dev_indicators[i], True);
    else
      XtSetSensitive(dev_indicators[i], False);
  }
}

static void CreateDeviceIndicators(Widget parent)
{
  int i, d;
  if (dev_indicators) {
    for (i = 0; i < n_dev_indicators; i++)
      XtDestroyWidget(dev_indicators[i]);
    FREE(dev_indicators);
    FREE(devs);
  }
  n_dev_indicators = 0;
  for (d = 0; d < ndevs(); d++)
    if (devlabel(d))
      n_dev_indicators++;
  if (n_dev_indicators) {
    XtManageChild(indicators_frame);
    XtVaSetValues(message_frame,
		  XmNrightAttachment, XmATTACH_WIDGET,
		  XmNrightWidget, indicators_frame,
		  XmNrightOffset, 3,
		  NULL);
    dev_indicators = (Widget*)MALLOC(n_dev_indicators*sizeof(Widget));
    devs = (int*)MALLOC(n_dev_indicators*sizeof(int));
  } else {
    XtUnmanageChild(indicators_frame);
    XtVaSetValues(message_frame,
		  XmNrightAttachment, XmATTACH_FORM,
		  XmNrightOffset, 0,
		  NULL);
    dev_indicators = NULL;
    devs = NULL;
  }
  for (i = d = 0; d < ndevs(); d++)
    if (devlabel(d)) {
      XmString labelstr = XmStringCreateLocalized(devlabel(d));
      Pixel background;
      if (i > 0)
	dev_indicators[i] =
	  XtVaCreateManagedWidget("device_indicator", xmIconGadgetClass,
				  parent,
				  XmNlabelString, labelstr,
				  XmNviewType, XmSMALL_ICON,
				  XmNleftAttachment, XmATTACH_WIDGET,
				  XmNleftWidget, dev_indicators[i-1],
				  XmNleftOffset, 5,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNtopAttachment, XmATTACH_NONE,
				  XmNbottomAttachment, XmATTACH_NONE,
				  NULL);
      else
	dev_indicators[i] =
	  XtVaCreateManagedWidget("device_indicator", xmIconGadgetClass,
				  parent,
				  XmNlabelString, labelstr,
				  XmNviewType, XmSMALL_ICON,
				  XmNtraversalOn, False,
				  XmNleftAttachment, XmATTACH_FORM,
				  XmNrightAttachment, XmATTACH_NONE,
				  XmNtopAttachment, XmATTACH_NONE,
				  XmNbottomAttachment, XmATTACH_NONE,
				  NULL);
      if (devicon(d))
	XtVaSetValues(dev_indicators[i],
		      XmNsmallIconPixmap, devicon(d)->pixmap,
		      XmNsmallIconMask, devicon(d)->mask,
		      NULL);
      XtVaGetValues(dev_indicators[i], XmNbackground, &background, NULL);
      XtVaSetValues(dev_indicators[i],
		    XmNtopShadowColor, background,
		    XmNbottomShadowColor, background,
		    NULL);
      XmStringFree(labelstr);
      devs[i++] = d;
    }
  XtVaGetValues(indicators, XmNhighlightColor, &highlightColor, NULL);
  XtVaGetValues(indicators, XmNforeground, &foreground, NULL);
  UpdateDeviceIndicators();
}

static void CreateStatusLine(Widget parent)
{
  status_line =
    XtVaCreateManagedWidget("status_line", xmFormWidgetClass, parent, NULL);
  XtVaSetValues(parent, XmNmessageWindow, status_line, NULL);
  message_frame =
    XtVaCreateManagedWidget("message_frame", xmFrameWidgetClass, status_line,
			    XmNleftAttachment, XmATTACH_FORM,
			    XmNtopAttachment, XmATTACH_FORM,
			    XmNbottomAttachment, XmATTACH_FORM,
			    NULL);
  message =
    XtVaCreateManagedWidget("message", xmLabelGadgetClass, message_frame,
			    XmNlabelType, XmSTRING,
			    XmNframeChildType, XmFRAME_WORKAREA_CHILD,
			    XmNalignment, XmALIGNMENT_BEGINNING,
			    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
			    NULL);
  indicators_frame =
    XtVaCreateWidget("indicators_frame", xmFrameWidgetClass,
		     status_line,
		     XmNleftAttachment, XmATTACH_NONE,
		     XmNrightAttachment, XmATTACH_FORM,
		     XmNtopAttachment, XmATTACH_FORM,
		     XmNbottomAttachment, XmATTACH_FORM,
		     NULL);
  indicators =
    XtVaCreateManagedWidget("indicators", xmFormWidgetClass, indicators_frame,
			    XmNframeChildType, XmFRAME_WORKAREA_CHILD,
			    XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
			    NULL);
  CreateDeviceIndicators(indicators);
}

/* window size: *************************************************************/

/* roughly estimate the minimum window size required and enforce it */

/* I went through lot of pain here to find some sane minimum values for the
   work area, since the Motif 2.0 paned windows break when the main window
   becomes too small. The following looks a bit like magic, but seems to work
   reasonably well. You might have to change things if you use an exotic pane
   layout, though. */

/* NOTE: Not required for Motif 2.1 (?), so I disabled this because it breaks
   session management under Solaris CDE. 09-08-01 AG */

static void set_min_size(Boolean init)
{
#if XmREVISION == 0
  Dimension min_w, min_h, menubar_h, status_line_h, tabs_w, tabs_h,
    file_view_w, file_view_h, work_area_w, work_area_h;
  Dimension file_pane_min, tree_pane_min, shelf_pane_min, file_view_min,
    log_pane_min;
  unsigned char work_area_o, file_view_o;
  int tabs_loc;

  XtVaGetValues(work_area, XmNorientation, &work_area_o, NULL);
  XtVaGetValues(file_view, XmNorientation, &file_view_o, NULL);
  if (shelfpane)
    XtVaGetValues(shelf_tabs, XgNtabLocation, &tabs_loc,
		  XmNwidth, &tabs_w, XmNheight, &tabs_h, NULL);
  else {
    tabs_loc = XgTOP;
    tabs_w = tabs_h = 0;
  }
  XtVaGetValues(menubar, XmNheight, &menubar_h, NULL);
  if (treepane || filepane)
    XtVaGetValues(status_line, XmNheight, &status_line_h, NULL);
  else
    status_line_h = 0;
  if (treepane)
    XtVaGetValues(tree_pane, XmNpaneMinimum, &tree_pane_min, NULL);
  else
    tree_pane_min = 0;
  if (filepane)
    XtVaGetValues(file_pane, XmNpaneMinimum, &file_pane_min, NULL);
  else
    file_pane_min = 0;
  if (shelfpane)
    XtVaGetValues(shelf_pane, XmNpaneMinimum, &shelf_pane_min, NULL);
  else
    shelf_pane_min = 0;
  if (logpane)
    XtVaGetValues(log_pane, XmNpaneMinimum, &log_pane_min, NULL);
  else
    log_pane_min = 0;
  if (filepane || treepane)
    XtVaGetValues(file_view, XmNpaneMinimum, &file_view_min, NULL);
  else
    file_view_min = 0;
  if (file_view_o == XmVERTICAL) {
    file_view_h = tree_pane_min + file_pane_min;
    file_view_w = 0;
  } else {
    file_view_w = tree_pane_min + file_pane_min;
    file_view_h = 0;
  }
  if (work_area_o == XmVERTICAL) {
    if (file_view_h > file_view_min) file_view_min = file_view_h;
    work_area_h = shelf_pane_min + file_view_min + log_pane_min;
    work_area_w = file_view_w;
  } else {
    if (file_view_w > file_view_min) file_view_min = file_view_w;
    work_area_w = shelf_pane_min + file_view_min + log_pane_min;
    work_area_h = file_view_h;
  }
  if (work_area_w < 300) work_area_w = 300;
  min_w = work_area_w;
  min_h = menubar_h + work_area_h + status_line_h;
  if (tabs_loc == XgTOP || tabs_loc == XgBOTTOM)
    min_h += tabs_h;
  else
    min_w += tabs_w;
  min_h += 20;
  XtVaSetValues(app_shell, XmNminWidth, min_w, XmNminHeight, min_h, NULL);
  if (!init) {
    Dimension w, h;
    XtVaGetValues(app_shell, XmNwidth, &w, XmNheight, &h, NULL);
    if (w < min_w)
      XtVaSetValues(app_shell, XmNwidth, min_w, NULL);
    if (h < min_h)
      XtVaSetValues(app_shell, XmNheight, min_h, NULL);
  }
#endif
}

/* session management: ******************************************************/

static char *sessionid = NULL;

static void init_session(char *id)
{
  char hostname[0x100], file[MAXPATHLEN+1];
  if (id && *id)
    sessionid = id;
  else {
    sessionid = MALLOC(100);
    sprintf(sessionid, "%d%d", getpid(), time(NULL));
    sessionid = REALLOC(sessionid, strlen(sessionid)+1);
  }
  if (gethostname(hostname, 0xff))
    sprintf(file, "%s", sessionid);
  else
    sprintf(file, "%s-%s", hostname, sessionid);
  pathname(sessionfile, configdir, file);
}

static void clear_session(void)
{
  if (sessionid) {
    if (!resdata.session || !*resdata.session)
      resdata.session = sessionid;
    sessionid = NULL;
    *sessionfile = 0;
  }
}

/* handle the WM_COMMAND property */

static int argc = 0, argc0 = 0;
static char **argv = NULL, **argv0 = NULL;
static char geomstr[60];
static int geom_i = 0, session_i = 0, shelf_i = 0, curdir_i = 0;

static void InitCommand()
{
  int argc1 = 0;
  char **argv1 = NULL;
  int i, j;

  /* record a copy of the command line (without any -clone options) for later
     use */
  XGetCommand(display, XtWindow(app_shell), &argv1, &argc1);
  argc = argc1;
  argv = (char**)MALLOC(argc*sizeof(char*));
  for (j = i = 0; i < argc1; i++)
    if (strcmp(argv1[i], "-clone"))
      argv[j++] = argv1[i];
    else {
      i++; argc -= 2;
    }
  FREE(argv1);

  /* make a copy of the command line without any -session options (used to
     revert to the "old" command line when a session is deleted) */
  argc0 = argc;
  argv0 = (char**)MALLOC(argc0*sizeof(char*));
  for (j = i = 0; i < argc; i++)
    if (strcmp(argv[i], "-session"))
      argv0[j++] = NEWSTRING(argv[i]);
    else {
      i++; argc0 -= 2;
    }

  /* make sure we get rid of WM_COMMAND when running under X11R6 SM since it
     interferes with the properties set with the smc_save_yourself callback
     function below */
  if (use_sm)
    XSetCommand(display, XtWindow(app_shell), NULL, 0);
}

/* get the geometry of the main window (this code is shamelessly pilfered
   from the X11 xwininfo program ;-) */

static void get_geometry(char *geomstr)
{
  Display *dpy = display;
  int screen = screen_no;
  Window window = XtWindow(app_shell);

  XWindowAttributes win_attributes;
  XSizeHints hints;
  int dw = DisplayWidth (dpy, screen), dh = DisplayHeight (dpy, screen);
  int rx, ry, xright, ybelow;
  int showright = 0, showbelow = 0;
  Status status;
  Window wmframe;
  long longjunk;

  *geomstr = 0;

  if (!XGetWindowAttributes(dpy, window, &win_attributes))
    return;

  /* compute size in appropriate units */
  status = XGetWMNormalHints(dpy, window, &hints, &longjunk);
  if (status  &&  hints.flags & PResizeInc  &&
      hints.width_inc != 0  &&  hints.height_inc != 0) {
    if (hints.flags & (PBaseSize|PMinSize)) {
      if (hints.flags & PBaseSize) {
	win_attributes.width -= hints.base_width;
	win_attributes.height -= hints.base_height;
      } else {
	/* ICCCM says MinSize is default for BaseSize */
	win_attributes.width -= hints.min_width;
	win_attributes.height -= hints.min_height;
      }
    }
    sprintf(geomstr, "%dx%d", win_attributes.width/hints.width_inc,
	   win_attributes.height/hints.height_inc);
  } else
    sprintf(geomstr, "%dx%d", win_attributes.width, win_attributes.height);

  geomstr += strlen(geomstr);

  if (!(hints.flags&PWinGravity))
    hints.win_gravity = NorthWestGravity; /* per ICCCM */
  /* find our window manager frame, if any */
  wmframe = window;
  while (True) {
    Window root, parent;
    Window *childlist;
    unsigned int ujunk;
    
    status = XQueryTree(dpy, wmframe, &root, &parent, &childlist, &ujunk);
    if (parent == root || !parent || !status)
      break;
    wmframe = parent;
    if (status && childlist)
      XFree((char *)childlist);
  }
  if (wmframe != window) {
    /* WM reparented, so find edges of the frame */
    /* Only works for ICCCM-compliant WMs, and then only if the
       window has corner gravity.  We would need to know the original width
       of the window to correctly handle the other gravities. */
    
    XWindowAttributes frame_attr;
    
    if (!XGetWindowAttributes(dpy, wmframe, &frame_attr))
      goto skip;

    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case WestGravity:
      rx = frame_attr.x;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case EastGravity:
      xright = dw - frame_attr.x - frame_attr.width -
	2*frame_attr.border_width;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case NorthGravity:
      ry = frame_attr.y;
    }
    switch (hints.win_gravity) {
    case NorthWestGravity: case SouthWestGravity:
    case NorthEastGravity: case SouthEastGravity:
    case SouthGravity:
      ybelow = dh - frame_attr.y - frame_attr.height -
	2*frame_attr.border_width;
    }
  }

 skip:

  /* If edge gravity, offer a corner on that edge (because the application
     programmer cares about that edge), otherwise offer upper left unless
     some other corner is close to an edge of the screen.
     (For corner gravity, assume gravity was set by XWMGeometry.
     For CenterGravity, it doesn't matter.) */
  if (hints.win_gravity == EastGravity  ||
      (abs(xright) <= 100  &&  abs(xright) < abs(rx)
       &&  hints.win_gravity != WestGravity))
    showright = 1;
  if (hints.win_gravity == SouthGravity  ||
      (abs(ybelow) <= 100  &&  abs(ybelow) < abs(ry)
       &&  hints.win_gravity != NorthGravity))
    showbelow = 1;
  
  if (showright)
    sprintf(geomstr, "-%d", xright);
  else
    sprintf(geomstr, "+%d", rx);
  geomstr += strlen(geomstr);
  if (showbelow)
    sprintf(geomstr, "-%d", ybelow);
  else
    sprintf(geomstr, "+%d", ry);
}

/* besides adding the -session option, we also have to record the geometry
   values in the command line, s.t. they can override the default resource
   values which are set at the time the shell is created; and we have to
   update a directory and shelf given on the command line, since these will
   override values in the resource file */

static void UpdateCommand()
{
  if (XtIsRealized(app_shell) && sessionid && *sessionid) {
    int i;
    Window root;
    Boolean have_geom = False;
    Boolean have_session = False;

    get_geometry(geomstr);
    /* update parameters */
    if (session_i)
      argv[session_i] = sessionid;
    else {
      for (i = 0; i < argc; i++)
	if (!strcmp(argv[i], "-session") && i+1 < argc) {
	  session_i = i+1;
	  argv[i+1] = sessionid;
	  have_session = True;
	}
      if (!have_session) {
	session_i = argc+1;
	argv = (char**)REALLOC(argv, (argc+2)*sizeof(char*));
	argv[argc++] = "-session";
	argv[argc++] = sessionid;
      }
    }
    if (*geomstr)
      if (geom_i)
	argv[geom_i] = geomstr;
      else {
	for (i = 0; i < argc; i++)
	  if ((!strcmp(argv[i], "-geometry") ||
	       !strcmp(argv[i], "-geom")) && i+1 < argc) {
	    geom_i = i+1;
	    argv[i+1] = geomstr;
	    have_geom = True;
	  }
	if (!have_geom) {
	  geom_i = argc+1;
	  argv = (char**)REALLOC(argv, (argc+2)*sizeof(char*));
	  argv[argc++] = "-geometry";
	  argv[argc++] = geomstr;
	}
      }
    if (shelf_i == -1)
      ;
    else if (shelf_i)
      argv[shelf_i] = shelf_label(shelf_no);
    else {
      shelf_i = -1;
      for (i = 0; i < argc; i++)
	if (!strcmp(argv[i], "-s") && i+1 < argc) {
	  shelf_i = i+1;
	  argv[i+1] = shelf_label(shelf_no);
	}
    }
    if (curdir_i == -1)
      ;
    else if (curdir_i)
      argv[curdir_i] = dirName(curdir);
    else {
      curdir_i = -1;
      for (i = 0; i < argc; i++)
	if (!strcmp(argv[i], "-p") && i+1 < argc) {
	  curdir_i = i+1;
	  argv[i+1] = dirName(curdir);
	}
    }
    /* only update WM_COMMAND when doing old style session management */
    if (!use_sm)
      XSetCommand(display, XtWindow(app_shell), argv, argc);
  }
}

/* revert command line to initial state */

static void RevertCommand()
{
  XSetCommand(display, XtWindow(app_shell), argv0, argc0);
  if (argv) FREE(argv);
  XGetCommand(display, XtWindow(app_shell), &argv, &argc);
  geom_i = 0, session_i = 0, shelf_i = 0, curdir_i = 0;
  if (use_sm)
    XSetCommand(display, XtWindow(app_shell), NULL, 0);
}

/* save the current session */

void SaveSession(Bool full_save)
{
  FILE *dest;
  Dimension height, width;

  if (!XtIsRealized(app_shell)) return;
  if (!sessionid || !*sessionid) init_session(resdata.session);

  UpdateCommand();

  if (!full_save) return;
  if (!(dest = fopen(sessionfile, "w"))) return;

  if (color_scheme)
    fprintf(dest, "xplore.scheme: %s\n", color_scheme);
  fprintf(dest, "! geometry\n");
  XtVaGetValues(tree_pane, XmNheight, &height, XmNwidth, &width, NULL);
  fprintf(dest, "xplore*tree_scrollSW.width: %d\n", width);
  fprintf(dest, "xplore*tree_scrollSW.height: %d\n", height);
  XtVaGetValues(file_pane, XmNheight, &height, XmNwidth, &width, NULL);
  fprintf(dest, "xplore*file_scrollSW.width: %d\n", width);
  fprintf(dest, "xplore*file_scrollSW.height: %d\n", height);
  XtVaGetValues(shelf_pane, XmNheight, &height, XmNwidth, &width, NULL);
  fprintf(dest, "xplore*shelf_scrollSW.width: %d\n", width);
  fprintf(dest, "xplore*shelf_scrollSW.height: %d\n", height);
  XtVaGetValues(log_pane, XmNheight, &height, XmNwidth, &width, NULL);
  fprintf(dest, "xplore*log_scrollSW.width: %d\n", width);
  fprintf(dest, "xplore*log_scrollSW.height: %d\n", height);
  fprintf(dest, "! current directory and shelf\n");
  fprintf(dest, "xplore.curdir: %s\n", dirName(curdir));
  fprintf(dest, "xplore.curshelf: %s\n", shelf_label(shelf_no));
  fprintf(dest, "! view options\n");
  fprintf(dest, "xplore.updir: %s\n", (dirOptions(curdir) & INCLUDE_UPDIR)?
	  "True":"False");
  fprintf(dest, "xplore.hidden: %s\n", (dirOptions(curdir) & INCLUDE_HIDDEN)?
	  "True":"False");
  fprintf(dest, "xplore.files: %s\n", (dirOptions(curdir) & INCLUDE_FILES)?
	  "True":"False");
  fprintf(dest, "xplore.dirs: %s\n", (dirOptions(curdir) & INCLUDE_DIRS)?
	  "True":"False");
  fprintf(dest, "xplore.sort: %s\n",
	  (dirOptions(curdir) & SORT_BY_SIZE)?"BySize":
	  ((dirOptions(curdir) & SORT_BY_DATE)?"ByDate":
	   "ByName"));
  fprintf(dest, "xplore.reverse: %s\n", (dirOptions(curdir) & REVERSE)?
	  "True":"False");
  fprintf(dest, "xplore.dirsfirst: %s\n", (dirOptions(curdir) & DIRS_FIRST)?
	  "True":"False");
  fprintf(dest, "xplore.rescan: %s\n", (dirOptions(curdir) & CHECK_FILES)?
	  "True":"False");
  fprintf(dest, "xplore.magic: %s\n", (dirOptions(curdir) & NOMAGIC)?
	  "False":"True");
  fprintf(dest, "! global options\n");
  fprintf(dest, "xplore.updates: %s\n", updates?"True":"False");
  fprintf(dest, "xplore.view: %s\n", (viewtype == LargeIconView)?
	  "LargeIcons":(viewtype == SmallIconView)?"SmallIcons":"Detail");
  fprintf(dest, "xplore.tree: %s\n", treepane?"True":"False");
  fprintf(dest, "xplore.file: %s\n", filepane?"True":"False");
  fprintf(dest, "xplore.shelf: %s\n", shelfpane?"True":"False");
  fprintf(dest, "xplore.log: %s\n", logpane?"True":"False");
  fprintf(dest, "xplore.absolute: %s\n", absolute?"True":"False");
  fprintf(dest, "xplore.pushdir: %s\n", pushdir?"True":"False");
  fprintf(dest, "xplore.dirtarget: %s\n", dirtarget?"True":"False");
  fprintf(dest, "xplore.check: %s\n", check?"True":"False");
  fprintf(dest, "xplore.echo: %s\n", echo?"True":"False");
  fprintf(dest, "xplore.backups: %s\n", backups?"True":"False");
  fprintf(dest, "xplore.splash: %s\n", splash?"True":"False");
  fprintf(dest, "xplore.autosave: %s\n", autosave?"True":"False");
  fprintf(dest, "xplore.autosession: %s\n", autosession?"True":"False");
  fprintf(dest, "xplore.autopopup: %s\n", autopopup?"True":"False");
  fprintf(dest, "! confirmation options\n");
  fprintf(dest, "xplore.drop: %s\n", confirm_drop?"True":"False");
  fprintf(dest, "xplore.move: %s\n", confirm_move?"True":"False");
  fprintf(dest, "xplore.copy: %s\n", confirm_copy?"True":"False");
  fprintf(dest, "xplore.link: %s\n", confirm_link?"True":"False");
  fprintf(dest, "xplore.delete: %s\n", confirm_delete?"True":"False");
  fprintf(dest, "xplore.deldir: %s\n", confirm_deldir?"True":"False");
  fprintf(dest, "xplore.delfile: %s\n", confirm_delfile?"True":"False");
  fprintf(dest, "xplore.overwrt: %s\n", confirm_overwrt?"True":"False");
  fprintf(dest, "xplore.quit: %s\n", confirm_quit?"True":"False");

  fclose(dest);
}

/* clear the current session */

Bool save_in_progress = False;

void ClearSession(void)
{
  if (!save_in_progress && *sessionfile) {
    remove(sessionfile);
    clear_session();
    RevertCommand();
  }
}

/* X11R6 session management support */

#ifdef USE_SM

static SmcConn smc_conn = NULL;
static Bool smc_saved = False;

/* session management callbacks */

static void smc_save_yourself(SmcConn conn, SmPointer data,
			      int save_type, Bool shutdown,
			      int interact_style, Bool fast)
{
  if (save_type != SmSaveGlobal) {
    /* this stuff mostly pilfered from an xemacs patch by Kondara, a Japanese
       GNU/Linux distribution; thanks to the anonymous authors :) */
    int i, n;
    char uidstr[256], discard[MAXPATHLEN+10];
    SmPropValue prop_program, prop_userid, prop_discard,
      *prop_clone, *prop_restart;
    /* FIXME: We should be able to tell the session manager how to get rid of
       our session file when the sm decides that it's no longer needed. Is
       there a portable way to do this? SmDiscardCommand seems to work
       differently (or not at all!) for different session managers. :( */
    SmProp prop[] = {
      { SmUserID,           SmARRAY8,       1, &prop_userid  },
      { SmProgram,          SmARRAY8,       1, &prop_program },
      { SmCloneCommand,     SmLISTofARRAY8, 0, NULL          },
      { SmRestartCommand,   SmLISTofARRAY8, 0, NULL          },
      { SmDiscardCommand,   SmLISTofARRAY8, 1, &prop_discard },
    };
    SmProp *props[] = { &prop[0], &prop[1], &prop[2], &prop[3], &prop[4] };

    /* save the session */
    if (autosession) {
      SaveSession(True);
      UpdateMenus();
    }
    /* update properties */
    sprintf(uidstr, "%d", getuid());
    prop_userid.value = uidstr;
    prop_userid.length = strlen(uidstr);
    prop_program.value = argv[0];
    prop_program.length = strlen(argv[0]);
    prop_clone = (SmPropValue*)MALLOC(argc*sizeof(SmPropValue));
    for (i = 0; i < argc; i++) {
      char *arg = (strcmp(argv[i], "-session"))?argv[i]:"-clone";
      prop_clone[i].value = arg;
      prop_clone[i].length = strlen(arg);
    }
    prop[2].num_vals = argc;
    prop[2].vals = prop_clone;
    prop_restart = (SmPropValue*)MALLOC(argc*sizeof(SmPropValue));
    for (i = 0; i < argc; i++) {
      prop_restart[i].value = argv[i];
      prop_restart[i].length = strlen(argv[i]);
    }
    prop[3].num_vals = argc;
    prop[3].vals = prop_restart;
    n = XtNumber(props);
    if (*sessionfile)
      sprintf(discard, "rm %s", sessionfile);
    else {
      *discard = 0;
      n--;
    }
    prop_discard.value = discard;
    prop_discard.length = strlen(discard);
    SmcSetProperties(conn, n, props);
    FREE(prop_clone); FREE(prop_restart);
  }
  save_in_progress = smc_saved = True;
  SmcSaveYourselfDone(conn, True); 
}

static void smc_die(SmcConn conn, SmPointer data)
{
  SmcCloseConnection(conn, 0, NULL);
  if (conn == smc_conn) smc_conn = NULL;
  /* if we have been checkpointed before, assume that the sm wants to restart
     us the next time and inhibit removal of the session file */
  save_in_progress = smc_saved;
  /* bail out */
  quit();
}

static void smc_save_complete(SmcConn conn, SmPointer data)
{
  save_in_progress = False;
}

static void smc_shutdown_cancelled(SmcConn conn, SmPointer data)
{
  save_in_progress = False;
}

/* watch the ICE connection */

static int watch_fd = -1, watch_id = -1;
IceConn ice_conn;

static void ice_input_cb(XtPointer data, int *fd, XtInputId *id)
{
  Bool rep;
  if (IceProcessMessages(ice_conn, NULL, &rep) == IceProcessMessagesIOError) {
    if (smc_conn) {
      ClearSession();
      SmcCloseConnection(smc_conn, 0, NULL);
      smc_conn = NULL;
    }
    ice_conn = NULL;
  }
}

static void ice_watch_fd(IceConn conn, IcePointer data, Bool opening,
			 IcePointer *watch_data)
{
  if (opening) {
    if (watch_fd != -1)
      /* this shouldn't happen but ... */
      error("extra ICE connection ignored");
    else {
      ice_conn = conn;
      watch_fd = IceConnectionNumber(conn);
      watch_id = XtAppAddInput(app, watch_fd, (XtPointer)XtInputReadMask,
			       ice_input_cb, conn);
    }
  } else if (IceConnectionNumber(conn) == watch_fd) {
    XtRemoveInput(watch_id);
    watch_id = watch_fd = -1;
    ice_conn = NULL;
  }
}

#endif

/* initialize session management; this will use X11R6 session management if
   available, and the X11R5 WM_SAVE_YOURSELF protocol otherwise */

void InitSession(void)
{
  Atom WM_SAVE_YOURSELF;

#ifdef USE_SM

  if (!resdata.nosm && getenv("SESSION_MANAGER")) {

    char *new_sessionid, error_msg[1024];
    SmcCallbacks cb;

    /* set up the ice infrastructure */
    if (!IceAddConnectionWatch(&ice_watch_fd, NULL)) {
      error("cannot initialize session management");
      goto default_sm;
    }

    /* initialize the smc connection */
    *error_msg = 0;

    cb.save_yourself.callback = &smc_save_yourself;
    cb.save_yourself.client_data = NULL;
    cb.die.callback = &smc_die;
    cb.die.client_data = NULL;
    cb.save_complete.callback = &smc_save_complete;
    cb.save_complete.client_data = NULL;
    cb.shutdown_cancelled.callback = &smc_shutdown_cancelled;
    cb.shutdown_cancelled.client_data = NULL;

    smc_conn = SmcOpenConnection(NULL, NULL, 0, 0,
				 SmcSaveYourselfProcMask|
				 SmcDieProcMask|
				 SmcSaveCompleteProcMask|
				 SmcShutdownCancelledProcMask,
				 &cb,
				 sessionid,
				 &new_sessionid,
				 sizeof(error_msg),
				 error_msg);

    if (!smc_conn) {
      char msg[1224] = "cannot connect to session manager";
      if (*error_msg)
	sprintf(msg, "cannot connect to session manager -- %s", error_msg);
      error(msg);
      goto default_sm;
    }

    InitCommand();

    /* update the session number */
    if (sessionid) {
      if (*sessionfile) remove(sessionfile);
      clear_session();
      resdata.session = new_sessionid;
      init_session(resdata.session);
    } else
      resdata.session = new_sessionid;

  } else {

  default_sm:
    use_sm = 0;
    InitCommand();
    WM_SAVE_YOURSELF = XmInternAtom(display, "WM_SAVE_YOURSELF", False);
    XmAddWMProtocolCallback(app_shell, WM_SAVE_YOURSELF, SaveYourselfCB, NULL);

  }

#else

  InitCommand();
  WM_SAVE_YOURSELF = XmInternAtom(display, "WM_SAVE_YOURSELF", False);
  XmAddWMProtocolCallback(app_shell, WM_SAVE_YOURSELF, SaveYourselfCB, NULL);

#endif
}

/* interface routines: ******************************************************/

/* hide and show the panes */

void HideTreePane(void)
{
  if (treepane) {
    XtUnmanageChild(tree_pane);
    treepane = False;
    if (!filepane)
      XtUnmanageChild(file_view);
    XtRemoveAllCallbacks(XtParent(tree_scroll), XmNresizeCallback);
    XtDestroyWidget(tree_scroll);
    tree_scroll = None;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
  }
}

static Boolean ShowTreeWorkProc(XtPointer data)
{
  if (treepane) {
    Resize(tree_scroll);
    XmScrollVisible(tree_pane, curgadget, 20, 20);
  }
  return True;
}

void ShowTreePane(void)
{
  if (!treepane) {
    Dimension save_min, save_max;
    Pixel pix;

    treepane = True;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
    if (!filepane) {
      XtVaSetValues(file_view, XmNwidth, view_wd, XmNheight, view_ht, NULL);
      FreezePaneGeom(file_view, &save_min, &save_max);
      XtManageChild(file_view);
      UnfreezePaneGeom(file_view, save_min, save_max);
    }
    XtVaSetValues(tree_pane, XmNwidth, tree_wd, XmNheight, tree_ht, NULL);
    FreezePaneGeom(tree_pane, &save_min, &save_max);
    XtManageChild(tree_pane);
    UnfreezePaneGeom(tree_pane, save_min, save_max);
    CreateTreeScroll(tree_pane);
    XtVaGetValues(tree_scroll, XmNbackground, &pix, NULL);
    XtVaSetValues(XtParent(tree_scroll), XmNbackground, pix, NULL);
    XtAppAddWorkProc(app, (XtWorkProc)ShowTreeWorkProc, NULL);
  }
}

void HideFilePane(void)
{
  if (filepane) {
    XtUnmanageChild(file_pane);
    filepane = False;
    if (!treepane)
      XtUnmanageChild(file_view);
    XtRemoveAllCallbacks(XtParent(file_scroll), XmNresizeCallback);
    if (is_w == file_scroll) is_init(None);
    XtDestroyWidget(file_scroll);
    file_scroll = None;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
  }
}

void ShowFilePane(void)
{
  if (!filepane) {
    Dimension save_min, save_max;
    Pixel pix;

    filepane = True;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
    if (!treepane) {
      XtVaSetValues(file_view, XmNwidth, view_wd, XmNheight, view_ht, NULL);
      FreezePaneGeom(file_view, &save_min, &save_max);
      XtManageChild(file_view);
      UnfreezePaneGeom(file_view, save_min, save_max);
    }
    XtVaSetValues(file_pane, XmNwidth, file_wd, XmNheight, file_ht, NULL);
    FreezePaneGeom(file_pane, &save_min, &save_max);
    XtManageChild(file_pane);
    UnfreezePaneGeom(file_pane, save_min, save_max);
    CreateFileScroll(file_pane);
    XtVaGetValues(file_scroll, XmNbackground, &pix, NULL);
    XtVaSetValues(XtParent(file_scroll), XmNbackground, pix, NULL);
    XtAppAddWorkProc(app, (XtWorkProc)ResizeWorkProc, file_scroll);
  }
}

void HideShelfPane(void)
{
  if (shelfpane) {
    XtUnmanageChild(shelf_tabs);
    XtUnmanageChild(shelf_pane);
    shelfpane = False;
    CreateAttachments();
    XtRemoveAllCallbacks(XtParent(shelf_scroll), XmNresizeCallback);
    if (is_w == shelf_scroll) is_init(None);
    XtDestroyWidget(shelf_scroll);
    shelf_scroll = None;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
  }
}

void ShowShelfPane(void)
{
  if (!shelfpane) {
    Dimension save_min, save_max;
    Pixel pix;

    shelfpane = True;
    UpdateTitle();
    UpdateStatusLine();
    UpdateMenus();
    set_min_size(False);
    XtManageChild(shelf_tabs);
    XtVaSetValues(shelf_pane, XmNwidth, shelf_wd, XmNheight, shelf_ht, NULL);
    FreezePaneGeom(shelf_pane, &save_min, &save_max);
    XtManageChild(shelf_pane);
    UnfreezePaneGeom(shelf_pane, save_min, save_max);
    CreateShelfScroll(shelf_pane);
    XtVaGetValues(shelf_scroll, XmNbackground, &pix, NULL);
    XtVaSetValues(XtParent(shelf_scroll), XmNbackground, pix, NULL);
    CreateAttachments();
    XtAppAddWorkProc(app, (XtWorkProc)ResizeWorkProc, shelf_scroll);
  }
}

void HideLogPane(void)
{
  if (logpane) {
    XtUnmanageChild(log_pane);
    logpane = False;
    UpdateMenus();
    set_min_size(False);
  }
}

void ShowLogPane(void)
{
  if (!logpane) {
    Dimension save_min, save_max;

    logpane = True;
    UpdateMenus();
    set_min_size(False);
    XtVaSetValues(log_pane, XmNwidth, log_wd, XmNheight, log_ht, NULL);
    FreezePaneGeom(log_pane, &save_min, &save_max);
    XtManageChild(log_pane);
    UnfreezePaneGeom(log_pane, save_min, save_max);
  }
}

/* save the current status of the panes and rebuild the display after
   changes */

/* For a sane update, we have to keep track of focus widgets and the position
   of the scrollbar sliders. We have to go a long way here, since the panes
   and the internal data structures may have been rebuilt from scratch. Thus
   we store the relevant status information and try to restore something as
   close as possible to the original state later. */

int act_pane = 0;

char dir_name[MAXPATHLEN+1];

int curdir_num = NONE, shelfdir_num = NONE;

int n_curdir = 0, n_shelfdir = 0;
String *curdir_names = NULL, *shelfdir_names = NULL;

static DirPtr find_dir(Widget scroll, Widget w)
{
  DirPtr dir;
  int i, n;
  WidgetList gadgets;

  XtVaGetValues(scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (i = 0; i < n; i++)
    if (gadgets[i] == w) {
      XtVaGetValues(w, XmNuserData, &dir, NULL);
      return dir;
    }
  return NULL;
}

static int find_file(Widget scroll, Widget w)
{
  int i, n, num;
  WidgetList gadgets;

  XtVaGetValues(scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (i = 0; i < n; i++)
    if (gadgets[i] == w) {
      XtVaGetValues(w, XmNuserData, &num, NULL);
      return num;
    }
  return NONE;
}

static String* get_file_list(DirPtr dir, int *n)
{
  int i, n_files = dirNFiles(dir);
  String *file_list = (String*)MALLOC(n_files*sizeof(String));

  *n = n_files;
  for (i = 0; i < n_files; i++)
    file_list[i] = NEWSTRING(fileName(dirFile(dir, i)));
  return file_list;
}

static void free_file_list(String **fl, int *n)
{
  if (*fl) {
    int i;
    for (i = 0; i < *n; i++)
      FREE((*fl)[i]);
    FREE((*fl));
    *fl = NULL;
  }
  *n = 0;
}

static void SaveFocus(void)
{
  DirPtr dir;
  int i;
  Widget w = XmGetFocusWidget(app_shell);

  free_file_list(&shelfdir_names, &n_shelfdir);
  free_file_list(&curdir_names, &n_curdir);

  if (w == None)
    act_pane = 0;
  else if (XtParent(w) == tree_scroll)
    act_pane = 1;
  else if (XtParent(w) == shelf_scroll)
    act_pane = 2;
  else if (XtParent(w) == file_scroll)
    act_pane = 3;
  else if (w == shelf_tabs)
    act_pane = 4;
  else if (w == log_scroll)
    act_pane = 5;
  else
    act_pane = 0;

  if (tree_scroll != None) {
    XtVaGetValues(tree_scroll, XmNinitialFocus, &w, NULL);
    if ((dir = find_dir(tree_scroll, w)))
      strcpy(dir_name, dirName(dir));
    else
      *dir_name = '0';
  } else
    *dir_name = '0';

  if (shelf_scroll != None) {
    XtVaGetValues(shelf_scroll, XmNinitialFocus, &w, NULL);
    shelfdir_num = find_file(shelf_scroll, w);
    if (shelfdir_num != NONE)
      shelfdir_names = get_file_list(shelfdir, &n_shelfdir);
  }
    
  if (file_scroll != None) {
    XtVaGetValues(file_scroll, XmNinitialFocus, &w, NULL);
    curdir_num = find_file(file_scroll, w);
    if (curdir_num != NONE)
      curdir_names = get_file_list(curdir, &n_curdir);
  }
}

static Widget find_dir_gadget(Widget scroll, String name)
{
  int i, n;
  WidgetList gadgets;

  XtVaGetValues(scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (i = 0; i < n; i++)
    if (XtIsManaged(gadgets[i]) && XtClass(gadgets[i]) == xmIconGadgetClass) {
      DirPtr dir;
      XtVaGetValues(gadgets[i], XmNuserData, &dir, NULL);
      if (dir && !strcmp(dirName(dir), name))
	return gadgets[i];
    }
  return None;
}

static Widget find_first_gadget(Widget scroll)
{
  int i, n;
  WidgetList gadgets;

  XtVaGetValues(scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (i = 0; i < n; i++)
    if (XtIsManaged(gadgets[i]) && XtClass(gadgets[i]) == xmIconGadgetClass) {
      int num;
      XtVaGetValues(gadgets[i], XmNuserData, &num, NULL);
      if (num != NONE)
	return gadgets[i];
    }
  return None;
}

static Widget find_file_gadget(Widget scroll, int num)
{
  int i, n;
  WidgetList gadgets;

  if (num == NONE) return None;
  XtVaGetValues(scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  for (i = 0; i < n; i++)
    if (XtIsManaged(gadgets[i]) && XtClass(gadgets[i]) == xmIconGadgetClass) {
      int num1;
      XtVaGetValues(gadgets[i], XmNuserData, &num1, NULL);
      if (num1 == num)
	return gadgets[i];
    }
  return None;
}

/* find the "nearest" file in the original file list which still exists in the
   given directory; implemented using binary search to speed it up for large
   lists */

static DirPtr _dir;
static String *_fl;
static int *_num;

static int cmp(const int *i, const int *j)
{
  return strcmp(fileName(dirFile(_dir, *i)), fileName(dirFile(_dir, *j)));
}

static int cmp2(const int *i, const int *j)
{
  return strcmp(_fl[*i], fileName(dirFile(_dir, *j)));
}

static int find_next(int num, String *fl, int n, DirPtr dir)
{
  int i, m = dirNFiles(dir), *p;
  if (num == NONE) return NONE;
  _dir = dir; _fl = fl;
  _num = (int*) MALLOC(m*sizeof(int));
  for (i = 0; i < m; i++) _num[i] = i;
  /* sort the files in dir by names */
  qsort(_num, m, sizeof(int), (int (*)(const void *, const void *))cmp);
  /* now look for the nearest item, first to the right, then to the left
     in fl, starting at num */
  for (i = num; i < n; i++)
    if ((p = bsearch(&i, _num, m, sizeof(int),
		     (int (*)(const void *, const void *))cmp2))) {
      int res = *p;
      FREE(_num);
      return res;
    }
  for (i = num-1; i >= 0; i--)
    if ((p = bsearch(&i, _num, m, sizeof(int),
		     (int (*)(const void *, const void *))cmp2))) {
      int res = *p;
      FREE(_num);
      return res;
    }
  FREE(_num);
  return NONE;
}

static Widget locate(int i)
{
  WidgetList gadgets = NULL;
  int num, act = 0;
  if (i == NONE) return None;
  XtVaGetValues(file_scroll, XmNnumChildren, &num, XmNchildren, &gadgets,
		NULL);
  while (act < num) {
    Widget gadget = gadgets[act];
    if (XtIsManaged(gadget) && XtClass(gadget) == xmIconGadgetClass) {
      int j;
      XtVaGetValues(gadget, XmNuserData, &j, NULL);
      if (i != j)
	act++;
      else
	return gadget;
    } else
      act++;
  }
  return None;
}

int loc_num;

static Widget RestoreFocus(Boolean in_shelf, Boolean in_tree, Boolean in_file)
{
  Widget treew = None, shelfw = None, filew = None;

  if (shelf_scroll != None && shelfdir_num != NONE && in_shelf &&
      (shelfw =
       find_file_gadget(shelf_scroll, 
			find_next(shelfdir_num, shelfdir_names, n_shelfdir,
				  shelfdir))) != None)
    XtVaSetValues(shelf_scroll, XmNinitialFocus, shelfw, NULL);
  if (file_scroll != None && loc_num != NONE && in_file &&
      (filew = locate(loc_num)) != None) {
    /* force focus on the file pane */
    act_pane = 3;
    XtVaSetValues(file_scroll, XmNinitialFocus, filew, NULL);
  } else if (file_scroll != None && curdir_num != NONE && in_file &&
      (filew =
       find_file_gadget(file_scroll, 
			find_next(curdir_num, curdir_names, n_curdir,
				  curdir))) != None)
    XtVaSetValues(file_scroll, XmNinitialFocus, filew, NULL);
  free_file_list(&shelfdir_names, &n_shelfdir);
  free_file_list(&curdir_names, &n_curdir);
  if (tree_scroll != None)
    if (*dir_name && in_tree && in_file &&
	(treew = find_dir_gadget(tree_scroll, dir_name)) != None)
      XtVaSetValues(tree_scroll, XmNinitialFocus, treew, NULL);
    else {
      /* this probably means that the current directory has changed */
      XtVaSetValues(tree_scroll, XmNinitialFocus, curgadget, NULL);
      if (act_pane == 3 && filepane &&
	  (loc_num != NONE && filew != None ||
	   (filew = find_first_gadget(file_scroll)) != None)) {
	loc_num = NONE;
	return filew;
      }
    }
  /* return focus widget of the active pane */
  loc_num = NONE;
  switch (act_pane) {
  case 1:
    if (treepane && treew != None)
      return treew;
    else
      return curgadget;
  case 2:
    if (shelfpane && (shelfw != None ||
	(shelfw = find_first_gadget(shelf_scroll)) != None))
      return shelfw;
    else
      return None;
  case 3:
    if (filepane && (filew != None ||
	(filew = find_first_gadget(file_scroll)) != None))
      return filew;
    else
      return None;
  case 4:
    return shelf_tabs;
  case 5:
    return log_scroll;
  default:
    return None;
  }
}

void SavePanes(void)
{
  SaveFocus();
}

struct UpdateInfo {
  Boolean in_progress;
  Boolean do_shelf, do_tree, do_file, save_shelf, save_tree, save_file;
  Boolean set_cur;
  Dimension tree_min, tree_max, tree_vert, tree_horiz;
  Dimension file_min, file_max, file_vert, file_horiz;
  Dimension shelf_min, shelf_max, shelf_vert, shelf_horiz;
  Widget focusw;
} d;

/* some helper routines */

static void SaveScrollBars(void)
{
  if (d.do_shelf && d.save_shelf)
    SaveScrollBarValues(shelf_pane, &d.shelf_vert, &d.shelf_horiz);
  if (d.do_tree && d.save_tree)
    SaveScrollBarValues(tree_pane, &d.tree_vert, &d.tree_horiz);
  if (d.do_file && d.save_file)
    SaveScrollBarValues(file_pane, &d.file_vert, &d.file_horiz);
}

static void FreezePanes(void)
{
  if (d.do_shelf)
    FreezePaneGeom(shelf_pane, &d.shelf_min, &d.shelf_max);
  if (d.do_tree)
    FreezePaneGeom(tree_pane, &d.tree_min, &d.tree_max);
  if (d.do_file)
    FreezePaneGeom(file_pane, &d.file_min, &d.file_max);
}

static void UpdateScrolls(void)
{
  if (d.do_shelf)
    UpdateShelfScroll(shelf_pane);
  if (d.do_tree)
    UpdateTreeScroll(tree_pane);
  if (d.do_file)
    UpdateFileScroll(file_pane);
}

static void RedrawScrolls(void)
{
  if (d.do_shelf && shelf_scroll != None) {
    XtManageChild(shelf_scroll);
    Resize(shelf_scroll);
  }
  if (d.do_tree && tree_scroll != None) {
    XtManageChild(tree_scroll);
    Resize(tree_scroll);
  }
  if (d.do_file && file_scroll != None) {
    XtManageChild(file_scroll);
    Resize(file_scroll);
  }
}

static void RestoreScrollBars(void)
{
  if (d.do_shelf)
    if (d.save_shelf)
      RestoreScrollBarValues(shelf_pane, d.shelf_vert, d.shelf_horiz);
    else
      ResetScrollBarValues(shelf_pane);
  if (d.do_tree)
    if (d.save_tree)
      RestoreScrollBarValues(tree_pane, d.tree_vert, d.tree_horiz);
    else
      ResetScrollBarValues(tree_pane);
  if (d.do_file)
    if (d.save_file)
      RestoreScrollBarValues(file_pane, d.file_vert, d.file_horiz);
    else
      ResetScrollBarValues(file_pane);
}

static void UnfreezePanes(void)
{
  if (d.do_shelf)
    UnfreezePaneGeom(shelf_pane, d.shelf_min, d.shelf_max);
  if (d.do_tree)
    UnfreezePaneGeom(tree_pane, d.tree_min, d.tree_max);
  if (d.do_file)
    UnfreezePaneGeom(file_pane, d.file_min, d.file_max);
}

/* this routine is used to force the keyboard traversal layout to be
   recalculated after the panes have been updated */

static void FixTraversal(Widget scroll)
{
  if (scroll != None) {
    XtVaSetValues(scroll, XmNtraversalOn, False, NULL);
    XtVaSetValues(scroll, XmNtraversalOn, True, NULL);
  }
}

/* make sure to invoke this in the background */

Widget new_focusw;

static Boolean UpdateWorkProc(XtPointer data)
{
  unsigned char sel;
  if (!d.do_shelf && !d.do_tree && !d.do_file) {
    d.in_progress = False;
    return True;
  } else if (lock)
    return False;
  Wait();
  SaveScrollBars();
  FreezePanes();
  UpdateScrolls();
  RedrawScrolls();
  UnfreezePanes();
  d.focusw = RestoreFocus(d.save_shelf, d.save_tree, d.save_file);
  RestoreScrollBars();
  UpdateStatusLine();
  UpdateMenus();
  UpdateTitle();
  if (d.do_shelf && shelf_scroll) {
    Widget w;
    XtVaGetValues(shelf_scroll, XmNinitialFocus, &w, NULL);
    if (w) XmScrollVisible(shelf_pane, w, 0, 0);
  }
  if (d.do_file && file_scroll) {
    Widget w;
    XtVaGetValues(file_scroll, XmNinitialFocus, &w, NULL);
    if (w) XmScrollVisible(file_pane, w, 0, 0);
  }
  if (d.set_cur && treepane)
    XmScrollVisible(tree_pane, curgadget, 20, 20);
  if (d.focusw != None)
    XmProcessTraversal(d.focusw, XmTRAVERSE_CURRENT);
  XmUpdateDisplay(app_shell);
  d.in_progress = False;
  Done();
  FixTraversal(file_scroll); FixTraversal(shelf_scroll);
  return True;
}

void UpdatePanes(Boolean do_shelf, Boolean do_tree, Boolean do_file,
		 Boolean save_shelf, Boolean save_tree, Boolean save_file,
		 Boolean set_cur)
{
  if (d.in_progress) {
    /* if we get here, an update is already "in the queue", so we just
       update the status information */
    d.do_shelf = d.do_shelf || do_shelf;
    d.do_tree = d.do_tree || do_tree;
    d.do_file = d.do_file || do_file;
    d.save_shelf = d.save_shelf && save_shelf;
    d.save_tree = d.save_tree && save_tree;
    d.save_file = d.save_file && save_file;
    d.set_cur = d.set_cur || set_cur;
  } else if (do_shelf || do_tree || do_file) {
    d.do_shelf = do_shelf;
    d.do_tree = do_tree;
    d.do_file = do_file;
    d.save_shelf = save_shelf;
    d.save_tree = save_tree;
    d.save_file = save_file;
    d.set_cur = set_cur;
    d.in_progress = True;
    XtAppAddWorkProc(app, (XtWorkProc)UpdateWorkProc, NULL);
  }
}

/* delete children in the tree container */

int n = 0, count = 0;
WidgetList gadgets = NULL, parents = NULL;

static void EnumChildren(Widget w)
{
  int i;

  n = count = 0;
  XtVaGetValues(tree_scroll, XmNnumChildren, &n, XmNchildren, &gadgets, NULL);
  parents = (WidgetList)MALLOC(n*sizeof(Widget));
  for (i = 0; i < n; i++) {
    Widget gadget = gadgets[i];
    if (gadget != w) {
      if (XtIsManaged(gadget) && XtClass(gadget) == xmIconGadgetClass)
	XtVaGetValues(gadget, XmNentryParent, &gadget, NULL);
      else
	gadget = None;
      while (gadget != None && gadget != w)
	XtVaGetValues(gadget, XmNentryParent, &gadget, NULL);
      if (gadget == w) count++;
      parents[i] = gadget;
    } else
      parents[i] = None;
  }
}

static void DeleteChildren(Widget w)
{
  int i;
  for (i = 0; i < n; i++)
    if (parents[i] == w)
      XtDestroyWidget(gadgets[i]);
}

static void PurgeChildrenInfo(void)
{
  FREE(parents);
  gadgets = parents = NULL;
  n = count = 0;
}

/* change outline state in tree pane */

/* MAXCOUNT is the max number of subdir entries Expand and Collapse handle
   incrementally by adding or removing corresponding child entries in the tree
   container. Note that incremental update is smoother than a complete redraw
   and also keeps the scrollbar position, but gets *very* slow when there are
   a lot of entries to be added or removed. To work around this, if the size
   of the subdirectory goes beyond MAXCOUNT, we assume that simply redrawing
   the view from scratch is more efficient. */

#define MAXCOUNT 100

void Expand(Widget w)
{
  DirPtr dir;
  int i;

  XtVaGetValues(w, XmNuserData, &dir, NULL);
  EnumChildren(w);
  if (count > MAXCOUNT ||
      dirHasSubdirInfo(dir) && dirNSubdirs(dir) > MAXCOUNT) {
    Dimension tree_min, tree_max;
    Widget treew;
    PurgeChildrenInfo();
    /* simply redraw the view from scratch */
    WaitMsg("Updating view, please wait ...");
    FreezePaneGeom(tree_pane, &tree_min, &tree_max);
    UpdateTreeScroll(tree_pane);
    XtManageChild(tree_scroll);
    Resize(tree_scroll);
    ResetScrollBarValues(tree_pane);
    UnfreezePaneGeom(tree_pane, tree_min, tree_max);
    if ((treew = find_dir_gadget(tree_scroll, dirName(dir))) != None)
      XmScrollVisible(tree_pane, treew, 20, 20);
    else
      XmScrollVisible(tree_pane, curgadget, 20, 20);
  } else {
    DeleteChildren(w);
    PurgeChildrenInfo();
    for (i = 0; i < dirNSubdirs(dir); i++)
      CreateTreeGadgets(tree_scroll, w, dirSubdir(dir, i));
    XtVaSetValues(w, XmNoutlineState, XmEXPANDED, NULL);
  }
  UpdateStatusLine();
  UpdateMenus();
  /*
  if (keyboardFocusPolicy == XmEXPLICIT)
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
  */
}

void Collapse(Widget w)
{
  DirPtr dir;

  XtVaGetValues(w, XmNuserData, &dir, NULL);
  EnumChildren(w);
  if (count > MAXCOUNT) {
    Dimension tree_min, tree_max, tree_vert, tree_horiz;
    Widget treew;
    PurgeChildrenInfo();
    WaitMsg("Updating view, please wait ...");
    FreezePaneGeom(tree_pane, &tree_min, &tree_max);
    UpdateTreeScroll(tree_pane);
    XtManageChild(tree_scroll);
    Resize(tree_scroll);
    ResetScrollBarValues(tree_pane);
    UnfreezePaneGeom(tree_pane, tree_min, tree_max);
    if ((treew = find_dir_gadget(tree_scroll, dirName(dir))) != None)
      XmScrollVisible(tree_pane, treew, 20, 20);
    else
      XmScrollVisible(tree_pane, curgadget, 20, 20);
  } else {
    DeleteChildren(w);
    PurgeChildrenInfo();
    if (dirHasSubdirs(dir)) {
      Widget subdir_entry = CreateDummyGadget(tree_scroll, "subdir", NULL);
      XtVaSetValues(subdir_entry, XmNentryParent, w, NULL);
    }
    XtVaSetValues(w, XmNoutlineState, XmCOLLAPSED, NULL);
  }
  UpdateStatusLine();
  UpdateMenus();
  /*
  if (keyboardFocusPolicy == XmEXPLICIT)
    XmProcessTraversal(w, XmTRAVERSE_CURRENT);
  */
}

/* status line */

void UpdateStatusLine(void)
{
  update_status();
  if (*status) {
    XmString labelstr;

    if (!XtIsManaged(status_line))
      XtManageChild(status_line);
    labelstr = XmStringCreateLocalized(status);
    XtVaSetValues(message,
		  XmNlabelString, labelstr,
		  XmNalignment, XmALIGNMENT_BEGINNING,
		  XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		  NULL);
    XmStringFree(labelstr);
    UpdateDeviceIndicators();
  } else {
    if (XtIsManaged(status_line))
      XtUnmanageChild(status_line);
  }
}

void RestoreStatusLine(void)
{
  XmString labelstr;

  labelstr = XmStringCreateLocalized(status?status:"");
  XtVaSetValues(message,
		XmNlabelString, labelstr,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		NULL);
  XmStringFree(labelstr);
}

void StatusMessage(String msg)
{
  XmString labelstr;

  labelstr = XmStringCreateLocalized(msg);
  XtVaSetValues(message,
		XmNlabelString, labelstr,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		NULL);
  XmStringFree(labelstr);
}

/* menus */

void UpdateMenus(void)
{
  /* file/directory */
  if (filepane && dirNFilesSelected(curdir) == 1) {
    XtSetSensitive(open_button, True);
    XtSetSensitive(rename_button, True);
    XtSetSensitive(bindings_button, True);
  } else {
    XtSetSensitive(open_button, False);
    XtSetSensitive(rename_button, False);
    XtSetSensitive(bindings_button, False);
  }
  if (filepane && dirNFilesSelected(curdir) > 0) {
    XtSetSensitive(move_button, True);
    XtSetSensitive(copy_button, True);
    XtSetSensitive(link_button, True);
    XtSetSensitive(delete_button, True);
    XtSetSensitive(properties_button, True);
  } else {
    XtSetSensitive(move_button, False);
    XtSetSensitive(copy_button, False);
    XtSetSensitive(link_button, False);
    XtSetSensitive(delete_button, False);
    XtSetSensitive(properties_button, False);
  }
  if (filepane) {
    XtSetSensitive(command_button, True);
    XtSetSensitive(mkdir_button, True);
    XtSetSensitive(chdir_button, True);
    XtSetSensitive(parent_button, True);
    XtSetSensitive(home_button, True);
    XtSetSensitive(select_button, True);
    XtSetSensitive(select_all_button, True);
    XtSetSensitive(invert_all_button, True);
  } else {
    XtSetSensitive(command_button, False);
    if (treepane) {
      XtSetSensitive(mkdir_button, True);
      XtSetSensitive(chdir_button, True);
      XtSetSensitive(parent_button, True);
      XtSetSensitive(home_button, True);
    } else {
      XtSetSensitive(mkdir_button, False);
      XtSetSensitive(chdir_button, False);
      XtSetSensitive(parent_button, False);
      XtSetSensitive(home_button, False);
    }
    XtSetSensitive(select_button, False);
    XtSetSensitive(select_all_button, False);
    XtSetSensitive(invert_all_button, False);
  }
  /* shelf */
  if (shelfpane && dirNFilesSelected(shelfdir) == 1) {
    XtSetSensitive(shelf_open_button, True);
    XtSetSensitive(shelf_rename_button, True);
    XtSetSensitive(shelf_bindings_button, True);
  } else {
    XtSetSensitive(shelf_open_button, False);
    XtSetSensitive(shelf_rename_button, False);
    XtSetSensitive(shelf_bindings_button, False);
  }
  if (shelfpane && dirNFilesSelected(shelfdir) > 0) {
    XtSetSensitive(shelf_move_button, True);
    XtSetSensitive(shelf_copy_button, True);
    XtSetSensitive(shelf_link_button, True);
    XtSetSensitive(shelf_delete_button, True);
    XtSetSensitive(shelf_properties_button, True);
  } else {
    XtSetSensitive(shelf_move_button, False);
    XtSetSensitive(shelf_copy_button, False);
    XtSetSensitive(shelf_link_button, False);
    XtSetSensitive(shelf_delete_button, False);
    XtSetSensitive(shelf_properties_button, False);
  }
  if (shelfpane) {
    XtSetSensitive(shelf_select_button, True);
    XtSetSensitive(shelf_select_all_button, True);
    XtSetSensitive(shelf_invert_all_button, True);
  } else {
    XtSetSensitive(shelf_select_button, False);
    XtSetSensitive(shelf_select_all_button, False);
    XtSetSensitive(shelf_invert_all_button, False);
  }
  /* options */
  if (filepane)
    XtSetSensitive(filter_button, True);
  else
    XtSetSensitive(filter_button, False);
  if (filepane || treepane || shelfpane) {
    XtSetSensitive(update_button, True);
    XtSetSensitive(reread_button, True);
    XtSetSensitive(unmount_button, True);
    XtSetSensitive(unmount_all_button, True);
  } else {
    XtSetSensitive(update_button, False);
    XtSetSensitive(reread_button, False);
    XtSetSensitive(unmount_button, False);
    XtSetSensitive(unmount_all_button, False);
  }
  if (*sessionfile)
    XtSetSensitive(clear_session_button, True);
  else
    XtSetSensitive(clear_session_button, False);
  /* view */
  if (filepane) {
    XtSetSensitive(large_icons_button, True);
    XtSetSensitive(small_icons_button, True);
    XtSetSensitive(detail_button, True);
    XtSetSensitive(sort_by_name_button, True);
    XtSetSensitive(sort_by_size_button, True);
    XtSetSensitive(sort_by_date_button, True);
    XtSetSensitive(reverse_button, True);
    XtSetSensitive(dirs_first_button, True);
    XtSetSensitive(show_parent_dir_button, True);
    XtSetSensitive(show_files_button, True);
    XtSetSensitive(show_dirs_button, True);
  } else {
    XtSetSensitive(large_icons_button, False);
    XtSetSensitive(small_icons_button, False);
    XtSetSensitive(detail_button, False);
    XtSetSensitive(sort_by_name_button, False);
    XtSetSensitive(sort_by_size_button, False);
    XtSetSensitive(sort_by_date_button, False);
    XtSetSensitive(reverse_button, False);
    XtSetSensitive(dirs_first_button, False);
    XtSetSensitive(show_parent_dir_button, False);
    XtSetSensitive(show_files_button, False);
    XtSetSensitive(show_dirs_button, False);
  }
  if (filepane || treepane) {
    XtSetSensitive(rescan_files_button, True);
    XtSetSensitive(magic_headers_button, True);
  } else {
    XtSetSensitive(rescan_files_button, False);
    XtSetSensitive(magic_headers_button, False);
  }
  if ((filepane || treepane) &&
      (dirOptions(curdir) & (INCLUDE_FILES | INCLUDE_DIRS)))
    XtSetSensitive(show_hidden_button, True);
  else
    XtSetSensitive(show_hidden_button, False);
  if (viewtype == LargeIconView)
    XtVaSetValues(large_icons_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(large_icons_button, XmNset, XmUNSET, NULL);
  if (viewtype == SmallIconView)
    XtVaSetValues(small_icons_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(small_icons_button, XmNset, XmUNSET, NULL);
  if (viewtype == DetailView)
    XtVaSetValues(detail_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(detail_button, XmNset, XmUNSET, NULL);
  if (treepane)
    XtVaSetValues(tree_pane_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(tree_pane_button, XmNset, XmUNSET, NULL);
  if (filepane)
    XtVaSetValues(file_pane_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(file_pane_button, XmNset, XmUNSET, NULL);
  if (shelfpane)
    XtVaSetValues(shelf_pane_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(shelf_pane_button, XmNset, XmUNSET, NULL);
  if (logpane)
    XtVaSetValues(log_pane_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(log_pane_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & SORT_BY_NAME)
    XtVaSetValues(sort_by_name_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(sort_by_name_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & SORT_BY_SIZE)
    XtVaSetValues(sort_by_size_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(sort_by_size_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & SORT_BY_DATE)
    XtVaSetValues(sort_by_date_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(sort_by_date_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & REVERSE)
    XtVaSetValues(reverse_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(reverse_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & DIRS_FIRST)
    XtVaSetValues(dirs_first_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(dirs_first_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & INCLUDE_HIDDEN)
    XtVaSetValues(show_hidden_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(show_hidden_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & INCLUDE_UPDIR)
    XtVaSetValues(show_parent_dir_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(show_parent_dir_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & INCLUDE_FILES)
    XtVaSetValues(show_files_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(show_files_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & INCLUDE_DIRS)
    XtVaSetValues(show_dirs_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(show_dirs_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & CHECK_FILES)
    XtVaSetValues(rescan_files_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(rescan_files_button, XmNset, XmUNSET, NULL);
  if (dirOptions(curdir) & NOMAGIC)
    XtVaSetValues(magic_headers_button, XmNset, XmUNSET, NULL);
  else
    XtVaSetValues(magic_headers_button, XmNset, XmSET, NULL);
  /* preferences */
  if (update_time > 0)
    XtSetSensitive(auto_updates_button, True);
  else
    XtSetSensitive(auto_updates_button, False);
  if (absolute)
    XtVaSetValues(absolute_paths_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(absolute_paths_button, XmNset, XmUNSET, NULL);
  if (dirtarget)
    XtVaSetValues(target_dir_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(target_dir_button, XmNset, XmUNSET, NULL);
  if (pushdir)
    XtVaSetValues(push_dir_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(push_dir_button, XmNset, XmUNSET, NULL);
  if (echo)
    XtVaSetValues(echo_commands_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(echo_commands_button, XmNset, XmUNSET, NULL);
  if (check)
    XtVaSetValues(check_mounts_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(check_mounts_button, XmNset, XmUNSET, NULL);
  if (backups)
    XtVaSetValues(backups_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(backups_button, XmNset, XmUNSET, NULL);
  if (splash)
    XtVaSetValues(show_splash_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(show_splash_button, XmNset, XmUNSET, NULL);
  if (autosave)
    XtVaSetValues(autosave_defaults_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(autosave_defaults_button, XmNset, XmUNSET, NULL);
  if (autosession)
    XtVaSetValues(autosave_session_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(autosave_session_button, XmNset, XmUNSET, NULL);
  if (autopopup)
    XtVaSetValues(autopopup_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(autopopup_button, XmNset, XmUNSET, NULL);
  if (confirm_drop)
    XtVaSetValues(confirm_drop_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_drop_button, XmNset, XmUNSET, NULL);
  if (confirm_move)
    XtVaSetValues(confirm_move_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_move_button, XmNset, XmUNSET, NULL);
  if (confirm_copy)
    XtVaSetValues(confirm_copy_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_copy_button, XmNset, XmUNSET, NULL);
  if (confirm_link)
    XtVaSetValues(confirm_link_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_link_button, XmNset, XmUNSET, NULL);
  if (confirm_delete)
    XtVaSetValues(confirm_delete_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_delete_button, XmNset, XmUNSET, NULL);
  if (confirm_deldir)
    XtVaSetValues(confirm_deldir_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_deldir_button, XmNset, XmUNSET, NULL);
  if (confirm_delfile)
    XtVaSetValues(confirm_delfile_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_delfile_button, XmNset, XmUNSET, NULL);
  if (confirm_overwrt)
    XtVaSetValues(confirm_overwrite_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_overwrite_button, XmNset, XmUNSET, NULL);
  if (confirm_quit)
    XtVaSetValues(confirm_quit_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(confirm_quit_button, XmNset, XmUNSET, NULL);
  if (updates)
    XtVaSetValues(auto_updates_button, XmNset, XmSET, NULL);
  else
    XtVaSetValues(auto_updates_button, XmNset, XmUNSET, NULL);
}

/* initialize the application */

void init_app(int argc, char **argv)
{
  String tmp;
#if OPT_I18N_SUPPORT
  XtSetLanguageProc(NULL, NULL, NULL);
#endif
  app_shell = XtAppInitialize(&app,
			      "Xplore",
			      options0, XtNumber(options0),
			      &argc, argv,
			      fallback_resources,
			      (ArgList) NULL, 0);
  display = XtDisplay(app_shell);
  screen_no = XScreenNumberOfScreen(XtScreen(app_shell));

  /* make sure we can close-on-exec the display connection */
  if (fcntl(ConnectionNumber(display), F_SETFD, 1) == -1)
    fatal("Couldn't mark display connection as close-on-exec");

#if (XtSpecificationRelease > 4)
#ifndef _NO_XMU

  /* add support for editres messages */
  XtAddEventHandler(app_shell, (EventMask)0, True, _XEditResCheckMessages,
		    NULL);

#endif
#endif

  /* handle _XPLORE_UPDATE messages */
  XtAddEventHandler(app_shell, PropertyChangeMask, False,
		    (XtEventHandler)UpdateH, NULL);

  /* register converters for application-specific resources */

  XtAppAddConverter(app, XmRString, "ViewType", StringToViewType,
		    NULL, 0);
  XtAppAddConverter(app, XmRString, "SortType", StringToSortType,
		    NULL, 0);

  /* get the application resources ... */

  XtGetApplicationResources(app_shell, &resdata, resources0,
			    XtNumber(resources0), NULL, 0);
  abspath(configdir, basedir, resdata.configdir);
  /* get the defaults file name */
  pathname(resfile, configdir, "defaults");

  /* ... merge in the defaults from the resfile ... */

  db = XtDatabase(display);
  if (*resfile && exists(resfile))
    if (!XrmCombineFileDatabase(resfile, &db, True))
      error("error reading defaults");

  /* ... if we have a session file, merge it as well ... */

  if (resdata.clone && *resdata.clone) {
    init_session(resdata.clone);
    if (*sessionfile && exists(sessionfile) &&
	!XrmCombineFileDatabase(sessionfile, &db, True))
      error("error reading session");
    sessionid = NULL;
    *sessionfile = 0;
  } else {
    if (resdata.session) init_session(resdata.session);
    if (*sessionfile && exists(sessionfile) &&
	!XrmCombineFileDatabase(sessionfile, &db, True))
      error("error reading session");
  }

  /* ... parse the remaining command line arguments (do this here so they
     can override values in the defaults file) ... */

  XrmParseCommand(&db, options, XtNumber(options), "xplore", &argc, argv);

  /* ... and finally get the full set of application resources */

  XtGetApplicationResources(app_shell, &resdata, resources,
			    XtNumber(resources), NULL, 0);

  /* make a copy of the database s.t. we can later restore the original
     settings if necessary (see SchemeCB) */

  tmp = tmpnam(NULL);
  XrmPutFileDatabase(db, tmp);
  db0 = XrmGetFileDatabase(tmp);
  unlink(tmp);

  /* set the configured color scheme */

  color_scheme = NEWSTRING(resdata.scheme);
  if (color_scheme && *color_scheme && exists(color_scheme) &&
      !XrmCombineFileDatabase(color_scheme, &db, True))
    error("error reading color scheme");

  /* retrieve global settings from resources */

  XtVaGetValues(app_shell, XmNkeyboardFocusPolicy, &keyboardFocusPolicy, NULL);

  abspath(rootdirname, basedir, resdata.rootdir);
  abspath(curdirname, basedir, resdata.curdir);
  abspath(shelfdirname, basedir, resdata.shelfdir);

  pathname(magicfile, configdir, "magic");
  pathname(configfile, configdir, "xplorerc");
  pathname(startup, configdir, "startup");
  curshelf = resdata.curshelf;
  cpp_options = resdata.cppoptions;
  iconpath = resdata.iconpath;
  sh_list = resdata.bourneShells;
  shell = resdata.shell;
  metachars = resdata.metachars;
  usertypes = resdata.types;

  show_getwd = resdata.getwd;

  savelines = resdata.savelines;

  update_time = resdata.update;
  updates = resdata.updates && update_time > 0;

  if (resdata.treeonly || resdata.fileonly || resdata.shelfonly ||
      resdata.logonly) {
    resdata.tree = resdata.treeonly;
    resdata.file = resdata.fileonly;
    resdata.shelf = resdata.shelfonly;
    resdata.log = resdata.logonly;
  }

  viewtype = resdata.view;
  treepane = resdata.tree;
  filepane = resdata.file;
  shelfpane = resdata.shelf;
  logpane = resdata.log;

  stdoptions = 0;

  if (resdata.rescan)
    stdoptions |= CHECK_FILES;
  
  switch (resdata.sort) {
  case BySize:
    stdoptions |= SORT_BY_SIZE;
    break;
  case ByDate:
    stdoptions |= SORT_BY_DATE;
    break;
  default:
    stdoptions |= SORT_BY_NAME;
  }

  if (resdata.updir)
    stdoptions |= INCLUDE_UPDIR;
  if (resdata.hidden)
    stdoptions |= INCLUDE_HIDDEN;
  if (resdata.files)
    stdoptions |= INCLUDE_FILES;
  if (resdata.dirs)
    stdoptions |= INCLUDE_DIRS;

  if (resdata.reverse)
    stdoptions |= REVERSE;
  if (resdata.dirsfirst)
    stdoptions |= DIRS_FIRST;
  if (!resdata.magic)
    stdoptions |= NOMAGIC;

  check = resdata.check;
  absolute = resdata.absolute;
  dirtarget = resdata.dirtarget;
  pushdir = resdata.pushdir;
  echo = resdata.echo;
  backups = resdata.backups;
  autosave = resdata.autosave;
  autosession = resdata.autosession;
  autopopup = resdata.autopopup;
  showhelp = resdata.showhelp;
  showver = resdata.showver;
  splash = resdata.splash;
  console = resdata.console;

  if (console) logpane = False;

  confirm_drop = resdata.drop;
  confirm_move = resdata.move;
  confirm_copy = resdata.copy;
  confirm_link = resdata.link;
  confirm_delete = resdata.delete;
  confirm_deldir = resdata.deldir;
  confirm_delfile = resdata.delfile;
  confirm_overwrt = resdata.overwrt;
  confirm_quit = resdata.quit;

  /* set the multi-click time */

  if (resdata.multiclick)
    XtSetMultiClickTime(display, resdata.multiclick);

  /* initialize wait cursor */
  wait_cursor = XCreateFontCursor(display, XC_watch);

  if (argc)
    progname = argv[0];
  else
    progname = "xplore";

  sprintf(manual, "%s/etc/%s", libdir, "Manual");
  sprintf(faq, "%s/etc/%s", libdir, "FAQ");
}

/* create application widgets */

void create_app(void)
{
  int i;
  DirPtr ret;
  Atom WM_DELETE_WINDOW;

  /* if the shelf and config directory do not exist, presumably xplore is
     invoked for the first time; try to run the xplore setup script */

  if (!exists(shelfdirname) && !exists(configdir) && exists(libsetup))
    if (setup_dlg(NULL) == OK)
      fileExec(libsetup, NULL, NULL);
    else
      fatal("user setup aborted!");

  /* create the directories we need */

  if (!exists(shelfdirname))
    mkdir(shelfdirname, user_umask & 0777);
  if (!chdir(shelfdirname))
    for (i = 0; i < n_shelves(); i++)
      if (shelf_label(i)) {
	String path = shelf_dir(i)?shelf_dir(i):shelf_label(i);
	if (!exists(path))
	  mkdir(path, user_umask & 0777);
      }

  if (!exists(configdir))
    mkdir(configdir, user_umask & 0777);

  /* set current shelf from resource value if possible */

  if (curshelf)
    for (i = 0; i < n_shelves(); i++)
      if (shelf_label(i) && !strcmp(curshelf, shelf_label(i))) {
	default_shelf = i;
	break;
      }

  /* read in initial directory information */

  cur_init(rootdirname, curdirname, stdoptions);
  shelf_init(shelfdirname, shelfoptions);

  /* parse translations and register drag action routines */

  tree_trans = XtParseTranslationTable(tree_trans_s);
  file_trans = XtParseTranslationTable(file_trans_s);
  XtAppAddActions(app, drag_actions, XtNumber(drag_actions));

  /* create the main window */

  main_window = XtVaCreateManagedWidget("main_window",
					xmMainWindowWidgetClass, app_shell,
					NULL);

  /* create the menu bar */
  
  menubar = XmCreateMenuBar(main_window, "menubar", NULL, 0);

  CreateCommandMenuDesc();

  file_menu_desc[COMMAND].submenu = command_menu_desc;
  file_menu_desc[COMMAND].submenu_size = command_menu_size;

  CreateMenu(menubar, MENU_PULLDOWN, "File", NULL, NULL, NULL,
	     file_menu_desc, XtNumber(file_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "FilePulldown", &file_pulldown,
	     "File", &file_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Directory", NULL, NULL, NULL,
	     directory_menu_desc, XtNumber(directory_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "DirectoryPulldown", &directory_pulldown,
	     "Directory", &directory_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Shelf", NULL, NULL, NULL,
	     shelf_menu_desc, XtNumber(shelf_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "ShelfPulldown", &shelf_pulldown,
	     "Shelf", &shelf_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Options", NULL, NULL, NULL,
	     options_menu_desc, XtNumber(options_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "OptionsPulldown", &options_pulldown,
	     "Options", &options_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Help", NULL, NULL, NULL,
	     help_menu_desc, XtNumber(help_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "HelpPulldown", &help_pulldown,
	     "Help", &help_button);

  XtVaSetValues(menubar, XmNmenuHelpWidget, help_button, NULL);

  XtManageChild(menubar);

  /* create the work area */

  CreateForm(main_window);

  /* create the status line */

  CreateStatusLine(main_window);

  /* set the application icon and install WM protocol handlers */

  /* Some wms still have trouble displaying color icons, hence I disabled
     the folder icon. Most wms allow you to set the application icons
     anyhow. -- AG */

  XtVaSetValues(app_shell,
		/*
		XmNiconPixmap, std_icons[ICON_XPLORE].pixmap,
		XmNiconMask, std_icons[ICON_XPLORE].mask,
		*/
		XmNdeleteResponse, XmDO_NOTHING, NULL);

  WM_DELETE_WINDOW = XmInternAtom(display, "WM_DELETE_WINDOW", False);
  XmAddWMProtocolCallback(app_shell, WM_DELETE_WINDOW, DeleteWindowCB, NULL);

}

void recreate_app(void)
{
  Dimension w, h;
  String log = XmTextGetString(log_scroll);
  XtVaGetValues(main_window, XmNwidth, &w, XmNheight, &h, NULL);
  XtDestroyWidget(main_window);
  main_window = XtVaCreateWidget("main_window",
				 xmMainWindowWidgetClass, app_shell,
				 XmNwidth, w, XmNheight, h,
				 NULL);
  menubar = XmCreateMenuBar(main_window, "menubar", NULL, 0);
  CreateMenu(menubar, MENU_PULLDOWN, "File", NULL, NULL, NULL,
	     file_menu_desc, XtNumber(file_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "FilePulldown", &file_pulldown,
	     "File", &file_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Directory", NULL, NULL, NULL,
	     directory_menu_desc, XtNumber(directory_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "DirectoryPulldown", &directory_pulldown,
	     "Directory", &directory_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Shelf", NULL, NULL, NULL,
	     shelf_menu_desc, XtNumber(shelf_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "ShelfPulldown", &shelf_pulldown,
	     "Shelf", &shelf_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Options", NULL, NULL, NULL,
	     options_menu_desc, XtNumber(options_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "OptionsPulldown", &options_pulldown,
	     "Options", &options_button);
  CreateMenu(menubar, MENU_PULLDOWN, "Help", NULL, NULL, NULL,
	     help_menu_desc, XtNumber(help_menu_desc),
	     menu_map_cb, menu_unmap_cb,
	     "HelpPulldown", &help_pulldown,
	     "Help", &help_button);
  XtVaSetValues(menubar, XmNmenuHelpWidget, help_button, NULL);
  XtManageChild(menubar);
  CreateForm(main_window);
  CreateStatusLine(main_window);
  XtManageChild(main_window);
  UpdateStatusLine();
  UpdateMenus();
  UpdatePanes(True, True, True, False, False, False, True);
  XmTextSetString(log_scroll, log);
  FREE(log);
  XmTextSetInsertionPosition(log_scroll, XmTextGetLastPosition(log_scroll));
  XmTextShowPosition(log_scroll, XmTextGetLastPosition(log_scroll));
  set_min_size(False);
}

/* run the application */

static void exec_startup(String startup)
{
  int i, argc1 = (argc<=0)?1:argc;
  char cmd[MAXPATHLEN+4], **argv1 = (char**)MALLOC(argc1*sizeof(char*));
  for (i = 1; i < argc; i++) argv1[i-1] = NEWSTRING(argv[i]);
  argv1[argc1-1] = NULL;
  sprintf(cmd, "%s $*", startup);
  fileExec(cmd, NULL, argv1);
}

XtInputId input_id;
XtIntervalId timer_id;

void run_app(void)
{
  Dimension x, y;
  XSizeHints hints;
  long supplied;

  /* realize the shell */

  /* do not map the window until we have fixed Motif's broken
     WM_NORMAL_HINTS */
  XtSetMappedWhenManaged(app_shell, False);
  XtRealizeWidget(app_shell);
  set_min_size(True);

  /* initialize session management here, to prevent the SM/WM from seeing our
     WM_COMMAND property (set by XtAppInitialize) if we're running under X11R6
     session management; if we do this later some session managers (GNOME!) 
     may get confused  */
  InitSession();

  /* remove Motif's junk PPosition flag (arrgh...) */
  XGetWMNormalHints(display, XtWindow(app_shell), &hints, &supplied);
  hints.flags &= ~PPosition;
  XSetWMNormalHints(display, XtWindow(app_shell), &hints);

  /* run the startup script */
  if (exists(startup))
    exec_startup(startup);
  else if (exists(libstartup))
    exec_startup(libstartup);

  /* now we can show the window */
  XtSetMappedWhenManaged(app_shell, True);
  XMapWindow(display, XtWindow(app_shell));

  /* adjust the width of the shelf and the file scroll */
  if (shelfpane)
    Resize(shelf_scroll);
  if (treepane)
    Resize(tree_scroll);
  if (filepane)
    Resize(file_scroll);

  /* initialize status line and menus */

  UpdateStatusLine();
  UpdateMenus();

  /* watch for input from stdout/err pipe */

  if (pipefd)
    input_id = XtAppAddInput(app, pipefd[0], (XtPointer)XtInputReadMask,
			     InputCB, NULL);

  /* initialize the update cycle */

  if (updates)
    timer_id = XtAppAddTimeOut(app, update_time, TimerCB, NULL);

  /* show the current directory in tree pane */

  if (treepane) {
    XmScrollVisible(tree_pane, curgadget, 20, 20);
    XmProcessTraversal(curgadget, XmTRAVERSE_CURRENT);
  }

  /* enter the event loop */

  XtAppMainLoop(app);

}

/* terminate application */

void exit_app(int val)
{
#ifdef USE_SM
  if (use_sm) {
    IceRemoveConnectionWatch(&ice_watch_fd, NULL);
    if (smc_conn)
      SmcCloseConnection(smc_conn, 0, NULL);
  }
#endif
  XtDestroyApplicationContext(app);
  exit(val);
}

/* reinitialize */

void reload_user_types(void)
{
  db = XtDatabase(display);
  if (*resfile && exists(resfile))
    if (!XrmCombineFileDatabase(resfile, &db, True))
      error("error reading defaults");
  XtGetApplicationResources(app_shell, &resdata, type_resources,
			    XtNumber(type_resources), NULL, 0);
  usertypes = resdata.types;
}

static Boolean InitFocusWorkProc(XtPointer data)
{
  if (treepane) {
    XmScrollVisible(tree_pane, curgadget, 20, 20);
    XmProcessTraversal(curgadget, XmTRAVERSE_CURRENT);
  }
  return True;
}

void reinit(void)
{
  int i, n;

  if (!exists(shelfdirname))
    mkdir(shelfdirname, user_umask & 0777);
  if (!chdir(shelfdirname))
    for (i = 0; i < n_shelves(); i++)
      if (shelf_label(i)) {
	String path = shelf_dir(i)?shelf_dir(i):shelf_label(i);
	if (!exists(path))
	  mkdir(path, user_umask & 0777);
      }
  XtDestroyWidget(command_pulldown);
  command_pulldown = XmCreatePulldownMenu(file_pulldown, "CommandPulldown",
					  NULL, 0);
  XtVaSetValues(command_button, XmNsubMenuId, command_pulldown, NULL);
  CreateCommandMenuDesc();
  CreateMenuButtons(command_pulldown, command_menu_desc,
		    command_menu_size);
  shelf_init(shelfdirname, shelfoptions);
  UpdateShelfTabs(form);
  CreateAttachments();
  CreateShelfScroll(shelf_pane);
  CreateTreeScroll(tree_pane);
  CreateFileScroll(file_pane);
  ResetScrollBarValues(shelf_pane);
  ResetScrollBarValues(tree_pane);
  ResetScrollBarValues(file_pane);
  if (shelfpane)
    Resize(shelf_scroll);
  if (treepane)
    Resize(tree_scroll);
  if (filepane)
    Resize(file_scroll);
  CreateDeviceIndicators(indicators);
  UpdateStatusLine();
  UpdateMenus();
  XtAppAddWorkProc(app, (XtWorkProc)InitFocusWorkProc, NULL);
}
