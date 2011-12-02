
/* curdir.h: current dir and shelf ******************************************/

/* This module encapsulates the internal data objects necessary for managing
   the file view and the shelf. */

#ifndef CURDIR_H
#define CURDIR_H

#include "xplore.h"
#include "dirs.h"

/* operation values for drop operations */

enum { OP_MOVE, OP_COPY, OP_LINK };

/* root, current and shelf directory */

extern DirPtr rootdir, curdir, shelfdir;

/* number of currently selected shelf */

extern int shelf_no;

/* these flags are set after each operation to indicate status changes
   in the corresponding parts of the directory tree */

extern Boolean root_modified, cur_modified, shelf_modified;
extern Boolean cur_changed;

/* recalculate current dir; sets cur_changed accordingly */

void cur_check(void);

/* update the contents of the directory tree and the shelf as necessary */

void update(void);
void restat(void);

/* initialize current and shelf directory (return True iff successful) */

Boolean cur_init(String root_path, String cur_path, int options);
Boolean shelf_init(String shelf_path, int options);

/* determine drop targets */

Boolean cur_is_drop_target(int i);
Boolean shelf_is_drop_target(int i);

/* operations on the current directory */

Boolean cur_mkdir(String name);
Boolean cur_chdir(String name);

Boolean cur_delete(void);
Boolean cur_move(String target, Boolean absolute);
Boolean cur_copy(String target, Boolean absolute);
Boolean cur_link(String target, Boolean absolute);

Boolean cur_push(int i, Boolean absolute, Boolean pushdir);
Boolean cur_drop(int i, int op, SelPtr sel, Boolean absolute,
		 Boolean dirtarget);

/* operations on the shelf */

Boolean shelf_goto(int i);

Boolean shelf_delete(void);
Boolean shelf_move(String target, Boolean absolute);
Boolean shelf_copy(String target, Boolean absolute);
Boolean shelf_link(String target, Boolean absolute);

Boolean shelf_push(int i, Boolean absolute, Boolean pushdir);
Boolean shelf_drop(int i, int op, SelPtr sel, Boolean absolute,
		   Boolean dirtarget);

#endif /* CURDIR_H */
