
#include "xplore.h"
#include "ftype.h"

#include "util.h"

#define MBUFSZ 1024   /* size of buffer for magic type names */

typedef struct _ActionRec {
  String action;
  String dirname;
} ActionRec;

typedef struct _TypeRec {
  Boolean tmp_flag;
  unsigned hash;
  int n_patterns;
  Boolean *magic_flag;
  String *magic;
  String *pattern;
  String large_icon_name, small_icon_name, comment;
  IconPtr large_icon, small_icon;
  ActionRec push;
  ActionRec drop;
  int n_menu;
  String *menu_label;
  ActionRec *menu;
} TypeRec;

static int ntypes = 0;
static TypeRec *types = NULL;

static void free_file_type(int type)
{
  int j;

  for (j = 0; j < types[type].n_patterns; j++) {
    FREE(types[type].pattern[j]);
    FREE(types[type].magic[j]);
  }
  FREE(types[type].magic_flag);
  FREE(types[type].magic);
  FREE(types[type].pattern);
  FREE(types[type].large_icon_name);
  FREE(types[type].small_icon_name);
  FREE(types[type].comment);
  FREE(types[type].push.action); FREE(types[type].push.dirname);
  FREE(types[type].drop.action); FREE(types[type].drop.dirname);
  for (j = 0; j < types[type].n_menu; j++) {
    FREE(types[type].menu_label[j]);
    FREE(types[type].menu[j].action);
    FREE(types[type].menu[j].dirname);
  }
  FREE(types[type].menu_label);
  FREE(types[type].menu);
}

static int new_file_type(int type)
{
  types[type].tmp_flag = False;
  types[type].hash = 0;
  types[type].n_patterns = 0;
  types[type].magic_flag = NULL;
  types[type].magic = NULL;
  types[type].pattern = NULL;
  types[type].large_icon_name = NULL;
  types[type].large_icon = NULL;
  types[type].small_icon_name = NULL;
  types[type].small_icon = NULL;
  types[type].comment = NULL;
  types[type].push.action = NULL;
  types[type].push.dirname = NULL;
  types[type].drop.action = NULL;
  types[type].drop.dirname = NULL;
  types[type].n_menu = 0;
  types[type].menu_label = NULL;
  types[type].menu = NULL;
  return type;
}

void init_file_types(void)
{
  int i;

  for (i = 0; i < ntypes; i++)
    free_file_type(i);
  FREE(types);
  ntypes = 0;
  types = NULL;
  add_file_type(False);
}

int add_file_type(Boolean tmp_flag)
{
  types = (TypeRec*) REALLOC(types, (ntypes+1)*sizeof(TypeRec));
  new_file_type(ntypes);
  types[ntypes].tmp_flag = tmp_flag;
  return ntypes++;
}

void pop_file_type(void)
{
  if (ntypes > 1) {
    free_file_type(--ntypes);
    types = (TypeRec*) REALLOC(types, (ntypes+1)*sizeof(TypeRec));
  }
}

int move_file_type(int type)
{
  if (0 <= type && type < ntypes - 1) {
    int i;
    TypeRec t;
    t = types[ntypes-1];
    for (i = ntypes-1; i > type; i--)
      types[i] = types[i-1];
    types[type] = t;
  }
}

int init_file_type(int type)
{
  if (0 <= type && type < ntypes) {
    free_file_type(type);
    return new_file_type(type);
  } else
    return type;
}

void delete_file_type(int type)
{
  if (0 <= type && type < ntypes) {
    int t;
    free_file_type(type);
    for (t = type+1; t < ntypes; t++)
      types[t-1] = types[t];
    ntypes--;
    types = (TypeRec*) REALLOC(types, (ntypes+1)*sizeof(TypeRec));
  }
}

static unsigned hash(unsigned h, const char *s)
{
  unsigned g;
  if (!s) return h;
  while (*s) {
    h = (h<<4)+*(s++);
    if ((g = (h & 0xf0000000)))	{
      h = h^(g>>24);
      h = h^g;
    }
  }
  return h;
}

int add_pattern(int type, Boolean magic_flag, String magic, String pattern)
{
  if (0 <= type && type < ntypes) {
    static const char *mstr = "\377";
    char s[MAXPATHLEN+1];

    types[type].magic_flag = (Boolean*)
      REALLOC(types[type].magic_flag,
	      (types[type].n_patterns+1)*sizeof(Boolean));
    types[type].magic = (String*)
      REALLOC(types[type].magic,
	      (types[type].n_patterns+1)*sizeof(String));
    types[type].pattern = (String*)
      REALLOC(types[type].pattern,
	      (types[type].n_patterns+1)*sizeof(String));

    types[type].magic_flag[types[type].n_patterns] = magic_flag;
    types[type].magic[types[type].n_patterns] =
      NEWSTRING(magic);
    if (pattern) {
      fnexpand(s, pattern);
      types[type].pattern[types[type].n_patterns] = NEWSTRING(s);
    } else
      types[type].pattern[types[type].n_patterns] = NULL;

    if (magic_flag) types[type].hash = hash(types[type].hash, mstr);
    types[type].hash = hash(types[type].hash,
			    types[type].magic[types[type].n_patterns]);
    types[type].hash = hash(types[type].hash,
			    types[type].pattern[types[type].n_patterns]);
    
    return types[type].n_patterns++;
  } else
    return NONE;
}

int set_icons(int type, String large_icon_name, String small_icon_name)
{
  if (0 <= type && type < ntypes) {
    if (types[type].large_icon_name)
      FREE(types[type].large_icon_name);
    types[type].large_icon_name = NEWSTRING(large_icon_name);
    if (large_icon_name && *large_icon_name)
      types[type].large_icon = read_icon(large_icon_name);
    else
      types[type].large_icon = NULL;
    if (types[type].small_icon_name)
      FREE(types[type].small_icon_name);
    types[type].small_icon_name = NEWSTRING(small_icon_name);
    if (small_icon_name && *small_icon_name)
      types[type].small_icon = read_icon(small_icon_name);
    else
      types[type].small_icon = NULL;
    return type;
  } else
    return NONE;
}

int set_comment(int type, String comment)
{
  if (0 <= type && type < ntypes) {
    if (types[type].comment)
      FREE(types[type].comment);
    types[type].comment = NEWSTRING(comment);
    return type;
  } else
    return NONE;
}

int set_actions(int type,
		String push_action, String push_dirname,
		String drop_action, String drop_dirname)
{
  if (0 <= type && type < ntypes) {
    char s[MAXPATHLEN+1];
    
    if (types[type].push.action) FREE(types[type].push.action);
    types[type].push.action = NEWSTRING(push_action);
    if (types[type].push.dirname) FREE(types[type].push.dirname);
    if (push_dirname) {
      abspath(s, basedir, push_dirname);
      types[type].push.dirname = NEWSTRING(s);
    } else
      types[type].push.dirname = NULL;
    if (types[type].drop.action) FREE(types[type].drop.action);
    types[type].drop.action = NEWSTRING(drop_action);
    if (types[type].drop.dirname) FREE(types[type].drop.dirname);
    if (drop_dirname) {
      abspath(s, basedir, drop_dirname);
      types[type].drop.dirname = NEWSTRING(s);
    } else
      types[type].drop.dirname = NULL;
    return type;
  } else
    return NONE;
}

int add_menu_action(int type, String menu_label, String menu_action,
		    String menu_dirname)
{
  if (0 <= type && type < ntypes) {
    char s[MAXPATHLEN+1];
    
    types[type].menu_label = (String*) REALLOC(types[type].menu_label,
					       (types[type].n_menu+1)*
					       sizeof(String));
    types[type].menu = (ActionRec*) REALLOC(types[type].menu,
					    (types[type].n_menu+1)*
					    sizeof(ActionRec));
    types[type].menu_label[types[type].n_menu] =
      NEWSTRING(menu_label);
    types[type].menu[types[type].n_menu].action =
      NEWSTRING(menu_action);
    if (menu_dirname) {
      abspath(s, basedir, menu_dirname);
      types[type].menu[types[type].n_menu].dirname = NEWSTRING(s);
    } else
      types[type].menu[types[type].n_menu].dirname = NULL;
    return types[type].n_menu++;
  } else
    return NONE;
}

static int nstrcmp(const char *s, const char *t)
{
  if (!s) s = "";
  if (!t) t = "";
  return strcmp(s, t);
}

int search_file_type(void)
{
  int i;
  for (i = 1; i < ntypes-1; i++) {
    if (types[i].hash == types[ntypes-1].hash &&
	types[i].n_patterns == types[ntypes-1].n_patterns) {
      int j;
      for (j = 0; j < types[ntypes-1].n_patterns; j++)
	if (types[i].magic_flag[j] !=
	    types[ntypes-1].magic_flag[j] ||
	    nstrcmp(types[i].magic[j],
		    types[ntypes-1].magic[j]) ||
	    nstrcmp(types[i].pattern[j],
		    types[ntypes-1].pattern[j]))
	  goto err;
      return i;
    err:
    ;
    }
  }
  return NONE;
}

static Boolean match(String str, String pattern)
{
  if (!pattern || !*pattern)
    return True;
  else if (!str)
    return False;
  else
    return fnmatch1(pattern, str);
}

static Boolean file_match(String file, String pattern)
{
  if (!pattern || !*pattern)
    return True;
  else {
    char file_name[MAXPATHLEN+1], file_dir[MAXPATHLEN+1];
    char pattern_name[MAXPATHLEN+1], pattern_dir[MAXPATHLEN+1];

    dirpart(file_dir, file); filepart(file_name, file);
    dirpart(pattern_dir, pattern); filepart(pattern_name, pattern);
    if (*pattern_dir)
      if (strcmp(pattern_dir, file_dir))
	return False;
      else if (*pattern_name)
	return match(file_name, pattern_name);
      else
	return *file_name == '\0';
    else
      return match(file_name, pattern_name);
  }
}

int file_type(String name, Boolean magic_flag)
{
  int i, j;
  char magic[MBUFSZ];
  struct stat stats;
  Boolean has_stats, is_special;

  if (magic_flag)
    magic_get_type(name, magic);
  has_stats = stat(name, &stats) == 0;
  is_special = has_stats &&
    (S_ISDIR(stats.st_mode) ||
     /* (stats.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) || */
     !S_ISREG(stats.st_mode));
  if (!magic_flag && is_special)
    return NONE;
  for (i = 1; i < ntypes; i++) {
    for (j = 0; j < types[i].n_patterns; j++)
      if (magic_flag) {
	if ((!is_special || types[i].magic_flag[j] &&
	     types[i].magic[j]) &&
	    match(magic, types[i].magic[j]) && 
	    file_match(name, types[i].pattern[j]))
	  return i;
      } else if ((!types[i].magic_flag[j] || !types[i].magic[j] ||
		  !*types[i].magic[j]) &&
		 types[i].pattern[j] &&
		 *types[i].pattern[j] &&
		 file_match(name, types[i].pattern[j]))
	return i;
  }
  if (!has_stats || !is_special)
    return DFLT;
  else
    return NONE;
}

Boolean match_file_type(int type, String name, Boolean magic_flag)
{
  int j;
  char magic[MBUFSZ];
  struct stat stats;
  Boolean has_stats, is_special;

  if (magic_flag)
    magic_get_type(name, magic);
  has_stats = stat(name, &stats) == 0;
  is_special = has_stats &&
    (S_ISDIR(stats.st_mode) ||
     (stats.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)));
  if (!magic_flag && is_special)
    return False;
  for (j = 0; j < types[type].n_patterns; j++)
    if (magic_flag) {
      if ((!is_special || types[type].magic_flag[j] &&
	   types[type].magic[j]) &&
	  match(magic, types[type].magic[j]) && 
	  file_match(name, types[type].pattern[j]))
	return True;
    } else if ((!types[type].magic_flag[j] || !types[type].magic[j] ||
		!*types[type].magic[j]) &&
	       types[type].pattern[j] &&
	       *types[type].pattern[j] &&
	       file_match(name, types[type].pattern[j]))
      return True;
  if ((!has_stats || !is_special) && type == DFLT)
    return True;
  else
    return False;
}

int n_types(void)
{
  return ntypes;
}

Boolean tmp_flag(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].tmp_flag;
  else
    return False;
}

int n_patterns(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].n_patterns;
  else
    return 0;
}

Boolean magic_flag(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_patterns)
    return types[type].magic_flag[i];
  else
    return False;
}

String magic(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_patterns)
    return types[type].magic[i];
  else
    return NULL;
}

String pattern(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_patterns)
    return types[type].pattern[i];
  else
    return NULL;
}

String large_icon_name(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].large_icon_name;
  else
    return NULL;
}

String small_icon_name(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].small_icon_name;
  else
    return NULL;
}

IconPtr large_icon(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].large_icon;
  else
    return NULL;
}

IconPtr small_icon(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].small_icon;
  else
    return NULL;
}

String comment(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].comment;
  else
    return NULL;
}

String push_action(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].push.action;
  else
    return NULL;
}

String push_dirname(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].push.dirname;
  else
    return NULL;
}

String drop_action(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].drop.action;
  else
    return NULL;
}

String drop_dirname(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].drop.dirname;
  else
    return NULL;
}

int n_menu_actions(int type)
{
  if (0 <= type && type < ntypes)
    return types[type].n_menu;
  else
    return 0;
}

String menu_label(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_menu)
    return types[type].menu_label[i];
  else
    return NULL;
}

String menu_action(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_menu)
    return types[type].menu[i].action;
  else
    return NULL;
}

String menu_dirname(int type, int i)
{
  if (0 <= type && type < ntypes &&
      0 <= i && i < types[type].n_menu)
    return types[type].menu[i].dirname;
  else
    return NULL;
}

