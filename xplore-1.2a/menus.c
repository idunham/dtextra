
#include "xplore.h"
#include "menus.h"

typedef struct _ActionRec {
    String action;
    String dirname;
} ActionRec;

static int n_cmd = 0;
static String *cmd_lbls = NULL;
static ActionRec *cmd_acts = NULL;

int default_shelf = NONE;
static int n_shelf = 0;
static String *shelf_lbls = NULL;
static String *shelf_dirs = NULL;

void init_cmd_menu(void)
{
    int i;

    for (i = 0; i < n_cmd; i++) {
	FREE(cmd_lbls[i]);
	FREE(cmd_acts[i].action);
	FREE(cmd_acts[i].dirname);
    }
    FREE(cmd_lbls);
    FREE(cmd_acts);
    n_cmd = 0;
    cmd_lbls = NULL;
    cmd_acts = NULL;
}

void init_shelf_menu(void)
{
    int i;

    for (i = 0; i < n_shelf; i++) {
	FREE(shelf_lbls[i]);
	FREE(shelf_dirs[i]);
    }
    FREE(shelf_lbls);
    FREE(shelf_dirs);
    n_shelf = 0;
    shelf_lbls = NULL;
    shelf_dirs = NULL;
    default_shelf = NONE;
}

void init_menus(void)
{
  init_cmd_menu();
  init_shelf_menu();
}

void default_cmd_menu(void)
{
  init_cmd_menu();
  add_cmd_action("(Empty)", NULL, NULL);
}

void default_shelf_menu(String shelfname)
{
  init_shelf_menu();
  default_shelf = add_shelf((shelfname && *shelfname)?shelfname:"Desk", NULL);
}

int add_cmd_action(String cmd_label, String cmd_action, String cmd_dirname)
{
    char s[MAXPATHLEN+1];

    cmd_lbls = (String*) REALLOC(cmd_lbls, (n_cmd+1)*sizeof(String));
    cmd_acts = (ActionRec*) REALLOC(cmd_acts, (n_cmd+1)*sizeof(ActionRec));
    cmd_lbls[n_cmd] = NEWSTRING(cmd_label);
    cmd_acts[n_cmd].action = NEWSTRING(cmd_action);
    if (cmd_dirname) {
	abspath(s, basedir, cmd_dirname);
	cmd_acts[n_cmd].dirname = NEWSTRING(s);
    } else
	cmd_acts[n_cmd].dirname = NULL;
    return n_cmd++;
}

int add_shelf(String shelf_label, String shelf_dir)
{
    char s[MAXPATHLEN+1];

    shelf_lbls = (String*) REALLOC(shelf_lbls, (n_shelf+1)*sizeof(String));
    shelf_dirs = (String*) REALLOC(shelf_dirs, (n_shelf+1)*sizeof(String));
    shelf_lbls[n_shelf] = NEWSTRING(shelf_label);
    shelf_dirs[n_shelf] = NEWSTRING(shelf_dir);
    return n_shelf++;
}

int n_cmd_actions(void)
{
    return n_cmd;
}

String cmd_label(int i)
{
    if (0 <= i && i < n_cmd)
	return cmd_lbls[i];
    else
	return NULL;
}

String cmd_action(int i)
{
    if (0 <= i && i < n_cmd)
	return cmd_acts[i].action;
    else
	return NULL;
}

String cmd_dirname(int i)
{
    if (0 <= i && i < n_cmd)
	return cmd_acts[i].dirname;
    else
	return NULL;
}

int n_shelves(void)
{
    return n_shelf;
}

String shelf_label(int i)
{
    if (0 <= i && i < n_shelf)
	return shelf_lbls[i];
    else
	return NULL;
}

String shelf_dir(int i)
{
    if (0 <= i && i < n_shelf)
	return shelf_dirs[i];
    else
	return NULL;
}
