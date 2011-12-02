
/***************************************************************************/
/* parse.y: parser for xplore config file                                  */
/***************************************************************************/

%{

#include "xplore.h"
#include "parse.h"

#include "dirs.h"
#include "ftype.h"
#include "menus.h"
#include "devmount.h"
#include "util.h"

#ifdef __OpenBSD__
extern int yylineno;
#endif

int errline = 0;
char errstr[1000];
Boolean tmpflag = 0;

static String mount_act, umount_act, icon_nm, large_icon_nm, small_icon_nm,
    comment_str, push_act, push_dirnm, drop_act, drop_dirnm;
static Boolean magic_fl;
static int options, setoptions, clroptions, type, base;


static Boolean source_mode = True;

static String *strtable = NULL;
static int n_str = 0;

static void inittable(void);
static String mkstr(String s);
static void popstr(int count);

static void check_icon(String name, IconPtr icon);
static void yyerror(char *s);

%}

%union {
    String s;
    int i;
}

%token COMMAND COMMENT DEFAULT DEVICE DROP ICON ITEM LARGEICON MAGIC MOUNT
%token OPTIONS PUSH SEPARATOR SHELF SMALLICON TYPE UMOUNT
%token RESCAN SORTBYNAME SORTBYSIZE SORTBYDATE REV DIRSFIRST
%token UPDIR HIDDEN FILES DIRS
%token ACTIONS ILLEGAL
%token <s> STRING
%type <s> string opt_string
%type <i> option_name

%%

source		: config

		| action_list

		;

config		: /* empty */

		| config item

		;

item		: DEVICE string opt_string '{' device_fields '}'

{
  int d;
  if (exists($2)) {
    if (options)
      options = (options & ~NOMAGIC) | (~options & NOMAGIC);
    else
      options = NONE;
    setoptions = (setoptions & ~NOMAGIC) | (~setoptions & NOMAGIC);
    clroptions = (clroptions & ~NOMAGIC) | (~clroptions & NOMAGIC);
    d = newdev($2, $3, icon_nm, mount_act, umount_act, options, setoptions,
	       clroptions);
    check_icon(icon_nm, devicon(d));
    inittable();
  }
}

/* the '@' labels are only valid when parsing the types resource */

		| '@' TYPE

{ magic_fl = False; type = add_file_type(tmpflag); }

		  pattern_list

{ base = search_file_type(); pop_file_type(); }

		| '@' MAGIC

{ magic_fl = True; type = add_file_type(tmpflag); }

		  pattern_list

{ base = search_file_type(); pop_file_type(); }

		| '@' DEFAULT

{ base = n_types(); }

		| TYPE

{ magic_fl = False; type = add_file_type(tmpflag); }

		  type

/* 1.0d addition: Merge user-defined type into the registry at a given
   priority.  *FIXME:* If the user edits the xplorerc file, the order of
   user-defined types may get messed up, and some user types may become
   "orphans" whose base types have been removed from the xplorerc file. There
   is no straightforward way to deal with such situations, so we currently
   only remove the orphans, and otherwise hope for the best. ;-) */

{
  if (tmpflag)
    if (base == ANY)
      /* "legacy" entry from pre-1.0d; takes highest priority */
      move_file_type(1);
    else if (base == NONE)
      /* orphans are silently removed */
      pop_file_type();
    else
      /* other entries are inserted before their respective base type */
      move_file_type(base);
}

		| MAGIC

{ magic_fl = True; type = add_file_type(tmpflag); }

		  type

{
  if (tmpflag)
    if (base == ANY)
      move_file_type(1);
    else if (base == NONE)
      pop_file_type();
    else
      move_file_type(base);
}

		| DEFAULT

{ magic_fl = False; type = init_file_type(DFLT); }

		  '{' type_fields '}'

{
    set_comment(type, comment_str);
    set_icons(type, large_icon_nm, small_icon_nm);
    check_icon(large_icon_nm, large_icon(type));
    check_icon(small_icon_nm, small_icon(type));
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
}

		| COMMAND '{' cmd_fields '}'

		| SHELF '{' shelf_fields '}'

		| error '}'

{ yyerrok; inittable(); }

		;

device_fields	: /* empty */

{
  mount_act = umount_act = NULL; options = setoptions = clroptions = 0;
  icon_nm = NULL;
}

		| device_fields device_field

		;

device_field	: MOUNT string

{ mount_act = $2; }

		| UMOUNT string

{ umount_act = $2; }

		| OPTIONS option_list

		| ICON string

{ icon_nm = $2; }

		;

option_list	: option

		| option_list ',' option

		;

option		: option_name

{ options |= $1; }

		| '+' option_name

{ setoptions |= $2; clroptions &= ~$2; }

		| '-' option_name

{ clroptions |= $2; setoptions &= ~$2; }

		;

option_name	: RESCAN

{ $$ = CHECK_FILES; }

		| SORTBYNAME

{ $$ = SORT_BY_NAME; }

		| SORTBYSIZE

{ $$ = SORT_BY_SIZE; }

		| SORTBYDATE

{ $$ = SORT_BY_DATE; }

		| REV

{ $$ = REVERSE; }

		| DIRSFIRST

{ $$ = DIRS_FIRST; }

		| UPDIR

{ $$ = INCLUDE_UPDIR; }

		| HIDDEN

{ $$ = INCLUDE_HIDDEN; }

		| FILES

{ $$ = INCLUDE_FILES; }

		| DIRS

{ $$ = INCLUDE_DIRS; }

		| MAGIC

{ $$ = NOMAGIC; }

		;

type		: pattern_list '{' type_fields '}'

{
    set_comment(type, comment_str);
    set_icons(type, large_icon_nm, small_icon_nm);
    check_icon(large_icon_nm, large_icon(type));
    check_icon(small_icon_nm, small_icon(type));
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
}

		;

pattern_list	: pattern

		| pattern_list ',' pattern

		;

pattern		: string

{
    String p = $1;
    String m = split_type(&p);

    add_pattern(type, magic_fl, m, p);
}

		;

type_fields	: /* empty */

{ large_icon_nm = small_icon_nm = comment_str =
      push_act = push_dirnm = drop_act = drop_dirnm = NULL; }

		| type_fields type_field

		;


type_field	: LARGEICON string

{ large_icon_nm = $2; }

		| SMALLICON string

{ small_icon_nm = $2; }

		| COMMENT string

{ comment_str = $2; }

		| PUSH string opt_string

{ push_act = $2; push_dirnm = $3; }

		| DROP string opt_string

{ drop_act = $2; drop_dirnm = $3; }

		| ITEM string ',' string opt_string

{ add_menu_action(type, $2, $4, $5); }

		| SEPARATOR

{ add_menu_action(type, NULL, NULL, NULL); }

		;

cmd_fields	: /* empty */

{ init_cmd_menu(); }

		| cmd_fields cmd_field

		;

cmd_field	: ITEM string ',' string opt_string

{ add_cmd_action($2, $4, $5); }

		| SEPARATOR

{ add_cmd_action(NULL, NULL, NULL); }

		;

shelf_fields	: /* empty */

{ init_shelf_menu(); }

		| shelf_fields shelf_field

		;

shelf_field	: ITEM string opt_string

{ if ($2 && default_shelf == NONE)
    default_shelf = add_shelf($2, $3);
  else
    add_shelf($2, $3); }

		| DEFAULT string opt_string

{ default_shelf = add_shelf($2, $3); }

		;

string		: STRING

		| string STRING

{
  if (!$1)
    $$ = $2;
  else if (!$2)
    $$ = $1;
  else {
    String s = alloca(strlen($1)+strlen($2)+1);
    strcat(strcpy(s, $1), $2);
    popstr(2);
    $$ = mkstr(s);
  }
}

		;

opt_string	: /* empty */

{ $$ = NULL; }

		| ',' string

{ $$ = $2; }

		;

/* The following is used to parse the action list when defining types
   interactively. */

action_list	: ACTIONS action_fields

{
    set_actions(type,
		push_act, push_dirnm,
		drop_act, drop_dirnm);
    inittable();
}

action_fields	: /* empty */

{ large_icon_nm = small_icon_nm = comment_str =
      push_act = push_dirnm = drop_act = drop_dirnm = NULL; }

		| action_fields action_field

		;

action_field	: PUSH string opt_string

{ push_act = $2; push_dirnm = $3; }

		| DROP string opt_string

{ drop_act = $2; drop_dirnm = $3; }

		| ITEM string ',' string opt_string

{ add_menu_action(type, $2, $4, $5); }

		| SEPARATOR

{ add_menu_action(type, NULL, NULL, NULL); }

		;

%%

static char fname[MAXPATHLEN+1];

#include "lex.c"

static void check_icon(String name, IconPtr icon)
{
    if (name && *name && !icon) {
	char msg[MAXPATHLEN+20];

	sprintf(msg, "error reading icon %s", name);
	error(msg);
    }
}

static void yyerror(String s)
{
  if (source_mode)
    fprintf(stderr, "%s: %d: %s at or near '%s'\n", fname, yylineno, s,
	    yytext);
  else {
    errline = yylineno;
    sprintf(errstr, "%d: %s at or near '%s'", yylineno, s,
	    yytext);
  }
}

static void inittable(void)
{
    int i;

    for (i = 0; i < n_str; i++)
	FREE(strtable[i]);
    FREE(strtable); strtable = NULL; n_str = 0;
}

static String mkstr(String s)
{
    strtable = (String*) REALLOC(strtable, (n_str+1)*sizeof(String));
    strtable[n_str] = NEWSTRING(s);
    return strtable[n_str++];
}

static void popstr(int count)
{
    for (; n_str > 0 && count > 0; count--)
	FREE(strtable[--n_str]);
}

void parse_config_file(String name, String cpp_command, String cpp_options)
{
  source_mode = True;
  strcpy(fname, name);
  if (exists(name)) {
    /* we use a named pipe here; hopefully this fixes some probs with popen()
       and flex-generated scanners */
    String pipe, cpp_line;
    
    pipe = tmpnam(NULL);
    mkfifo(pipe, 0600);
    cpp_line = alloca(strlen(cpp_command) +
		      strlen(cpp_options) +
		      strlen(name) +
		      strlen(pipe) + 10);
    sprintf(cpp_line, "%s %s %s > %s &", cpp_command, cpp_options, name, pipe);
    system(cpp_line);
    if ((yyin = fopen(pipe, "r"))) {
#ifdef FLEX_SCANNER
      yyrestart(yyin);
#endif
      yylineno = 1;
      BEGIN(SRC);
      tmpflag = False; base = NONE;
      yyparse();
      inittable();
      FREE(str);
      str = NULL;
      remove(pipe);
    } else
      error("error parsing config file: cannot exec cpp");
  }
}

void parse_config_string(String source)
{
  FILE *tmpfile;
  if (!source) return;
  source_mode = True;
  tmpnam(fname);
  if (!(tmpfile = fopen(fname, "w"))) {
    error("error writing temp file");
    return;
  }
  fputs(source, tmpfile);
  if (fclose(tmpfile)) {
    error("error writing temp file");
    unlink(fname);
    return;
  }
  if ((yyin = fopen(fname, "r"))) {
#ifdef FLEX_SCANNER
    yyrestart(yyin);
#endif
    yylineno = 1;
    BEGIN(SRC);
    tmpflag = True; base = ANY;
    yyparse();
    inittable();
    FREE(str);
    str = NULL;
    unlink(fname);
    return;
  } else {
    error("error reading temp file");
    unlink(fname);
    return;
  }
}

Boolean parse_actions(int t, String actions)
{
  FILE *tmpfile;
  if (!actions) return;
  source_mode = False;
  errline = 0;
  *errstr = '\0';
  tmpnam(fname);
  if (!(tmpfile = fopen(fname, "w"))) {
    errline = 0;
    strcpy(errstr, "error writing temp file");
    return False;
  }
  fputs("Actions\t", tmpfile);
  fputs(actions, tmpfile);
  if (fclose(tmpfile)) {
    errline = 0;
    strcpy(errstr, "error writing temp file");
    unlink(fname);
    return False;
  }
  if ((yyin = fopen(fname, "r"))) {
    type = t;
#ifdef FLEX_SCANNER
    yyrestart(yyin);
#endif
    yylineno = 1;
    BEGIN(SRC);
    tmpflag = True; base = NONE;
    yyparse();
    inittable();
    FREE(str);
    str = NULL;
    unlink(fname);
    return (*errstr) == '\0';
  } else {
    errline = 0;
    strcpy(errstr, "error reading temp file");
    unlink(fname);
    return False;
  }
}
