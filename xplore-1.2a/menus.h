
/***************************************************************************/
/* menus.h: tables for constructing the command and shelf menus            */
/***************************************************************************/

#ifndef MENUS_H
#define MENUS_H

#include "xplore.h"

void init_cmd_menu(void);
void init_shelf_menu(void);
void init_menus(void);
/* (re)initialize the menu tables */

void default_cmd_menu(void);
void default_shelf_menu(String shelfname);
/* provide reasonable defaults for the menus */

int add_cmd_action(String cmd_label, String cmd_action, String cmd_dirname);
/* add an action to the table of command actions, return the action
   number */

int add_shelf(String shelf_label, String shelf_dir);
/* add an entry to the shelf table, return the entry number */

int n_cmd_actions(void);
/* number of command actions */

String cmd_label(int i);
/* command action label no. i (0 <= i < n_cmd_actions()) */

String cmd_action(int i);
/* command action no. i */

String cmd_dirname(int i);
/* directory name for command action (NULL if none) */

extern int default_shelf;
/* the default shelf entry */

int n_shelves(void);
/* number of shelf entries */

String shelf_label(int i);
/* shelf label no. i (0 <= i < n_shelves()) */

String shelf_dir(int i);
/* shelf subdir no. i */

#endif /* MENUS_H */

