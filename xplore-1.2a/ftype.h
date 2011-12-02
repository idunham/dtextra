
/***************************************************************************/
/* ftypes.h: file type table                                               */
/***************************************************************************/

#ifndef FTYPE_H
#define FTYPE_H

#include "xplore.h"
#include "icons.h"

/* This module implements xplore's type registry. The registry can be extended
   dynamically at runtime, and gives you full control over the order in which
   types are matched against target files. */

void init_file_types(void);
/* (re)initialize the file type table */

/* Types are numbered contiguously, starting at 0 (which always denotes the
   default type). With the exception of type 0, lower type numbers are matched
   first. Each type has a tmp_flag which can be used to distinguish
   "permanent" types (usually created at intialization time from a config
   file) from "temporary" ones (typically created by the user at runtime). */

int add_file_type(Boolean tmp_flag);
/* create a new type, return its number */

void pop_file_type(void);
/* purge the last type */

int move_file_type(int type);
/* move the last type before the given one */

int init_file_type(int type);
/* reinitialize a file type, return its number */

void delete_file_type(int type);
/* delete the given type */

/* for the following operations DFLT denotes the default type number */

int add_pattern(int type, Boolean magic_flag, String magic, String pattern);
/* add a pattern to the given type and return the pattern number; ~ in pattern
   is expanded */

int set_icons(int type, String large_icon_name, String small_icon_name);
/* set the icons of the given type; return value: type */

int set_comment(int type, String comment);
/* set the description of the given type; return value: type */

int set_actions(int type,
		String push_action, String push_dirname,
		String drop_action, String drop_dirname);
/* set push and drop actions of given type; ~ in dirnames is expanded,
   relative pathnames are interpreted w.r.t. basedir; return value: type */

int add_menu_action(int type, String menu_label, String menu_action,
		    String menu_dirname);
/* add a new menu action to the given type, return the action number */

/* value ops */

int search_file_type(void);
/* look up an existing entry for the most recent type (i.e., a type with the
   same pattern set), return the type (NONE if none is found) */

int file_type(String name, Boolean magic_flag);
/* determine type of file name (absolute filename); if not in table, return
   NONE for directories and executables, and DFLT otherwise */

Boolean match_file_type(int type, String name, Boolean magic_flag);
/* check whether file name matches the given type */

int n_types(void);
/* return total number of registered file types (including default type) */

Boolean tmp_flag(int type);
/* return the tmp_flag of the given type */

int n_patterns(int type);
/* return the number of patterns for the given type */

Boolean magic_flag(int type, int i);
/* return magic flag of ith pattern for given type
   (0 <= i < n_patterns(type)) */

String magic(int type, int i);
String pattern(int type, int i);
/* return magic key and pattern of ith pattern for given type */

String large_icon_name(int type);
String small_icon_name(int type);
/* return the names of the pixmaps for type */

IconPtr large_icon(int type);
IconPtr small_icon(int type);
/* return the icons for type */

String comment(int type);
/* return the comment for type */

String push_action(int type);
/* push action of given type */

String push_dirname(int type);
/* directory name for push action (NULL if none) */

String drop_action(int type);
/* drop action of given type */

String drop_dirname(int type);
/* directory name for drop action (NULL if none) */

int n_menu_actions(int type);
/* number of menu actions for given type */

String menu_label(int type, int i);
/* menu label no. i (0 <= i < n_menu_actions(type)) for given type */

String menu_action(int type, int i);
/* menu action no. i for given type */

String menu_dirname(int type, int i);
/* directory name for menu action (NULL if none) */

#endif /* FTYPE_H */
