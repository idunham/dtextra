
/* interface.h: user interface **********************************************/

#ifndef INTERFACE_H
#define INTERFACE_H

#include "xplore.h"
#include "dirs.h"

/* application context, resource db, shell, display and screen: */

extern XtAppContext app;
extern XrmDatabase db, db0;
extern Widget app_shell;
extern Display *display;
extern int screen_no;

/* widgets in the main window */

extern Widget main_window, menubar, form, shelf_tabs, work_area, file_view,
file_pane, file_scroll, tree_pane, tree_scroll, shelf_pane, shelf_scroll,
log_pane, log_scroll, status_line, message_frame, message, indicators_frame,
indicators, curgadget;

/* the menu bar */

extern Widget file_pulldown, directory_pulldown, shelf_pulldown,
options_pulldown, view_pulldown, preferences_pulldown, help_pulldown,
command_pulldown, file_popup, log_popup;

extern Widget file_button, directory_button, shelf_button, options_button,
view_button, preferences_button, help_button, command_button;

extern Widget open_button, rename_button, move_button, copy_button,
link_button, delete_button, properties_button, bindings_button, select_button,
select_all_button, invert_all_button, filter_button, quit_button;

extern Widget mkdir_button, chdir_button, parent_button, home_button;

extern Widget shelf_open_button, shelf_rename_button, shelf_move_button,
shelf_copy_button, shelf_link_button, shelf_delete_button,
shelf_properties_button, shelf_bindings_button, shelf_select_button,
shelf_select_all_button, shelf_invert_all_button;

extern Widget update_button, reread_button, unmount_button,
unmount_all_button, clear_log_button, scheme_button, save_defaults_button,
save_session_button, clear_session_button, reload_config_button;

extern Widget manual_button, faq_button, website_button, about_button;

extern Widget locate_button;

extern Widget *command_menu_button;
extern int command_menu_size;

extern Widget large_icons_button, small_icons_button, detail_button,
tree_pane_button, file_pane_button, shelf_pane_button, log_pane_button,
sort_by_name_button, sort_by_size_button, sort_by_date_button, reverse_button,
dirs_first_button, show_hidden_button, show_parent_dir_button,
show_files_button, show_dirs_button, rescan_files_button, magic_headers_button;

extern Widget absolute_paths_button, target_dir_button, push_dir_button,
echo_commands_button, check_mounts_button, backups_button, show_splash_button,
autosave_defaults_button, autosave_session_button, autopopup_button,
confirm_drop_button, confirm_move_button,
confirm_copy_button, confirm_link_button, confirm_delete_button,
confirm_deldir_button, confirm_delfile_button, confirm_overwrite_button,
confirm_quit_button, auto_updates_button;

/* wait cursor */
extern Cursor wait_cursor;

/* global resource values: */

extern String progname;        /* name of program */

extern char configdir[];       /* name of user's config dir */
extern char magicfile[];       /* name of user's magic file */
extern char configfile[];      /* name of user's config file */
extern char startup[];         /* name of user's startup script */
extern char resfile[];         /* name of X resource file */
extern char sessionfile[];     /* session file name */
extern String curshelf;	       /* default shelf */
extern String cpp_options;     /* options to pass to the C preprocessor */
extern String iconpath;        /* search path for pixmaps */
extern String color_scheme;    /* active color scheme */
extern String sh_list;         /* value of bourneShells resource */
extern String shell;           /* value of shell resource */
extern String usertypes;       /* additional user-defined types */

extern unsigned char keyboardFocusPolicy;

extern char rootdirname[];     /* base directory of tree pane */
extern char curdirname[];      /* current directory in file pane */
extern char shelfdirname[];    /* the shelf directory */

extern Boolean show_getwd;     /* show getwd in window title */
extern int update_time;        /* update cycle in milliseconds */
extern Boolean updates;        /* automatic updates enabled? */

extern int savelines;	       /* max lines in log */

typedef enum _ViewType {
    LargeIconView, SmallIconView, DetailView,
} ViewType;

extern ViewType viewtype;      /* current view type */
extern Boolean filepane;       /* whether or not the file pane is shown */
extern Boolean treepane;       /* whether or not the tree pane is shown */
extern Boolean shelfpane;      /* whether or not the shelf is shown */
extern Boolean logpane;        /* whether or not the log is shown */

extern Boolean absolute;       /* enable/disable absolute paths */
extern Boolean dirtarget;      /* do drop action in target dir? */
extern Boolean pushdir;        /* do push action in current dir? */

extern Boolean                 /* enable/disable confirmation options */
confirm_drop,
confirm_move,
confirm_copy,
confirm_link,
confirm_delete,
confirm_deldir,
confirm_delfile,
confirm_overwrt,
confirm_quit;

extern Boolean autosave;       /* automagically save defaults when exiting */
extern Boolean autosession;    /* automagically save session (SAVE_YOURSELF) */
extern Boolean autopopup;      /* automagically pop up log on output */
extern Boolean showhelp;       /* show help message and exit */
extern Boolean showver;        /* show version info and exit */
extern Boolean splash;	       /* show splash screen at startup */
extern Boolean console;	       /* redirect output to console */

/* input and update timer id */

extern XtInputId input_id;
extern XtIntervalId timer_id;

/* source directory in a tree drag operation */

extern DirPtr tree_drag_dir;

/* routines for managing the application */

void init_app(int argc, char **argv);
/* initialize the application */

void create_app(void);
/* create the application widgets */

void recreate_app(void);
/* recreate application after resource changes */

void run_app(void);
/* run the application */

void exit_app(int val);
/* exit the application */

void reload_user_types(void);
/* reload the user type information from the defaults file */

void reinit(void);
/* reinitialize after configuration has been reread */

/* hide and show the panes */

void HideTreePane(void);
void ShowTreePane(void);

void HideFilePane(void);
void ShowFilePane(void);

void HideShelfPane(void);
void ShowShelfPane(void);

void HideLogPane(void);
void ShowLogPane(void);

/* save the current status of the panes and rebuild the display after
   changes */

void SavePanes(void);
void UpdatePanes(Boolean do_shelf, Boolean do_tree, Boolean do_file,
		 Boolean save_shelf, Boolean save_tree, Boolean save_file,
		 Boolean set_cur);

/* expand/collapse an item in the tree pane */

void Expand(Widget w);
void Collapse(Widget w);

/* status line */

void UpdateStatusLine(void);
void RestoreStatusLine(void);
void StatusMessage(String msg);

/* menus */

void UpdateMenus(void);

/* session management */

void SaveSession(Bool full_save);
void ClearSession(void);

#endif /* INTERFACE_H */
