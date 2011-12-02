
/****************************************************************************/
/* dialogs.h: abstract wrapper for dialogs implemented using the toolkit    */
/****************************************************************************/

#ifndef DIALOGS_H
#define DIALOGS_H

#include "xplore.h"
#include "icons.h"

/* button values */

enum {
  ERROR, OK, CANCEL, YES, NO, DOALL, IGNORE, REPLACE, ADD, REMOVE, CLEAR,
  ADD2, REPLACE2, DELETE, N_BUTTONS
};

/* confirmation dialogs */

/* override resource settings */
extern Boolean force_overwrt, force_deldir, force_delfile;

/* returns: OK, CANCEL */
int confirm_drop_dlg(IconPtr icon, int nFiles, String dirname, String target);
int confirm_move_dlg(IconPtr icon, int nFiles, String dirname, String target);
int confirm_copy_dlg(IconPtr icon, int nFiles, String dirname, String target);
int confirm_link_dlg(IconPtr icon, int nFiles, String dirname, String target);
int confirm_delete_dlg(IconPtr icon, int nFiles, String dirname);

/* returns: YES, NO, DOALL, CANCEL */
int confirm_overwrite_dir_dlg(IconPtr icon, String target);
int confirm_overwrite_file_dlg(IconPtr icon, String target);

int confirm_rename_dir_dlg(IconPtr icon, String target);
int confirm_rename_file_dlg(IconPtr icon, String target);

int confirm_delete_dir_dlg(IconPtr icon, String name);
int confirm_delete_file_dlg(IconPtr icon, String name);

/* returns: OK, CANCEL */
int confirm_nomatch_type_dlg(IconPtr icon, String patterns, String name);
int confirm_delete_type_dlg(IconPtr icon, int type, String patterns);

/* quit dialog (OK/CANCEL) */

int confirm_quit_dlg(IconPtr icon);

/* color scheme dialog (OK/CANCEL) */

int scheme_dlg(IconPtr icon, String s, int maxlen);

/* mount/unmount dialogs */

int mount_dlg(IconPtr icon, String name, Boolean verify);
/* try to mount a device, return the device number */
void umount_dlg(IconPtr icon, int d);
/* try to unmount a device */

/* fileop parameter dialogs (OK/CANCEL); returns string value */

int mkdir_dlg(IconPtr icon, String d, String s, int maxlen);
int chdir_dlg(IconPtr icon, String d, String s, int maxlen);
int rename_dlg(IconPtr icon, String d, String s, int maxlen);
int move_dlg(IconPtr icon, String d, String s, int maxlen);
int copy_dlg(IconPtr icon, String d, String s, int maxlen);
int link_dlg(IconPtr icon, String d, String s, int maxlen);

/* properties dialog (OK/CANCEL) */

int props_dlg(IconPtr icon, int nFiles, int nBytes,
	      String dirname, String filename, String linkname,
	      String desc, String comment,
	      String last_acc, String last_mod, String last_chg,
	      String user, String group, int maxlen,
	      mode_t *mode, mode_t *mask);

/* bindings dialog (ADD2/REPLACE2/DELETE/CANCEL) */

int bind_dlg(IconPtr icon, int type, int errline, String patterns,
	     String deflt, Boolean *use_default, Boolean *magic,
	     String comment, String large_icon,
	     String small_icon, String actions,
	     String *types, int ntypes, int *pos,
	     int maxlen,
	     Boolean *edited);

/* selection dialog (REPLACE/ADD/REMOVE/CANCEL) */

int select_dlg(IconPtr icon, String s, int maxlen);

/* filter dialog (OK/CLEAR/CANCEL) */

int filter_dlg(IconPtr icon, String s, int maxlen);

/* obtain action parameters and write the result to buf (returns OK, CANCEL
   or ERROR if buf is too small to hold the result) */

int param_dlg(IconPtr icon, String msg, String action, String buf, int bufsz);

/* about dialog */

int about_dlg(IconPtr icon);

/* splash screen */

void splash_on(IconPtr icon);
void splash_off(void);

/* setup dialog (OK/CANCEL) */

int setup_dlg(IconPtr icon);

/* error and warning dialogs */

int error_dlg(IconPtr icon, String msg);
int warn_dlg(IconPtr icon, String msg);

#endif /* DIALOGS_H */
