
#include "xplore.h"
#include "callbacks.h"

#include "curdir.h"
#include "devmount.h"
#include "dialogs.h"
#include "dirs.h"
#include "error.h"
#include "icons.h"
#include "fileops.h"
#include "ftype.h"
#include "interface.h"
#include "menus.h"
#include "parse.h"
#include "util.h"

#include <Xm/XmAll.h>
#include <Tabs.h>

/* private functions */

/* update directory information and redisplay the panes */

static void Update(void)
{
  if (updates)
    XtRemoveTimeOut(timer_id);
  SavePanes();
  update();
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
  if (updates)
    timer_id = XtAppAddTimeOut(app, update_time, TimerCB, NULL);
}

static void Restat(void)
{
  if (updates)
    XtRemoveTimeOut(timer_id);
  SavePanes();
  restat();
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
  if (updates)
    timer_id = XtAppAddTimeOut(app, update_time, TimerCB, NULL);
}

/* determine selected files in the file pane or shelf */

static int FindSelected(DirPtr dir)
{
  int i;
    
  for (i = 0; i < dirNFiles(dir); i++)
    if (fileSelected(dirFile(dir, i)))
      return i;
  return NONE;
}

static IconPtr sel_icon(DirPtr dir)
{
  int i;

  if (dirNFilesSelected(dir) == 1 && (i = FindSelected(dir)) != NONE)
    return fileLargeIcon(dirFile(dir, i));
  else
    return &std_icons[ICON_FILES];
}

/* ----------------------------------------------------------------------- */

void OpenCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int i;

  if (filepane && dirNFilesSelected(curdir) == 1 &&
      (i = FindSelected(curdir)) != NONE) {
    SavePanes();
    cur_push(i, absolute, pushdir);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

#define MAX_BUF 10000

void CommandActionCB(Widget w, int i, XtPointer widget_data)
{
  char buf[MAX_BUF+1];

  if (!filepane)
    return;
  XmUpdateDisplay(app_shell);
  if (cmd_action(i) && *cmd_action(i) &&
      param_dlg(&std_icons[ICON_APPLY], cmd_label(i), cmd_action(i), buf,
		MAX_BUF) == OK) {
    SelPtr sel = filepane ? selFromDir(curdir) : NULL;

    fileApply(sel, buf, dirName(curdir), absolute);
    selFree(sel);
    Update();
  }
}

void MkdirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  char name[MAXPATHLEN+1];

  if (!filepane && !treepane)
    return;
  XmUpdateDisplay(app_shell);
  *name = '\0';
  if (mkdir_dlg(&std_icons[ICON_DIR], dirName(curdir), name, MAXPATHLEN) == OK) {
    SavePanes();
    cur_mkdir(name);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void ChdirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  char name[MAXPATHLEN+1], path[MAXPATHLEN+1];

  if (!filepane && !treepane)
    return;
  XmUpdateDisplay(app_shell);
  *name = '\0';
  if (chdir_dlg(&std_icons[ICON_DIR], dirName(curdir), name, MAXPATHLEN) == OK) {
    SavePanes();
    if (!cur_chdir(abspath(path, dirName(curdir), name)))
      xplerr1(ERR_FIND_DIR, path);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void ParentCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  char path[MAXPATHLEN+1];

  if (!filepane && !treepane)
    return;
  XmUpdateDisplay(app_shell);
  SavePanes();
  if (!cur_chdir(abspath(path, dirName(curdir), "..")))
    xplerr1(ERR_FIND_DIR, path);
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
}

void HomeCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  char path[MAXPATHLEN+1];

  if (!filepane && !treepane)
    return;
  XmUpdateDisplay(app_shell);
  SavePanes();
  if (!cur_chdir(fnexpand(path, "~")))
    xplerr1(ERR_FIND_DIR, path);
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
}

void RenameCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int i;

  if (filepane && dirNFilesSelected(curdir) == 1 &&
      (i = FindSelected(curdir)) != NONE) {
    FilePtr file = dirFile(curdir, i);
    char target[MAXPATHLEN+1], abstarget[MAXPATHLEN+1];
    StatRec stats;

    XmUpdateDisplay(app_shell);
    strcpy(target, fileName(file));
    if (rename_dlg(fileLargeIcon(file), dirName(curdir), target, MAXPATHLEN) == OK &&
	strcmp(target, fileName(file)))
      if (!stat(abspath(abstarget, dirName(curdir), target), &stats) &&
	  S_ISDIR(stats.st_mode))
	xplerr1(ERR_TARGET_IS_DIR, target);
      else {
	SavePanes();
	cur_move(target, absolute);
	UpdatePanes(shelf_modified, root_modified, cur_modified,
		    True, True, !cur_changed, cur_changed);
      }
  }
}

void MoveCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && dirNFilesSelected(curdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (move_dlg(sel_icon(curdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      cur_move(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void CopyCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && dirNFilesSelected(curdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (copy_dlg(sel_icon(curdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      cur_copy(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void LinkCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && dirNFilesSelected(curdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (link_dlg(sel_icon(curdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      cur_link(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void DeleteCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && dirNFilesSelected(curdir)) {
    XmUpdateDisplay(app_shell);
    SavePanes();
    cur_delete();
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void PropertiesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  IconPtr icon = NULL;
  int nFiles = dirNFilesSelected(curdir);
  int nBytes = dirNBytesSelected(curdir);
  String dirname = dirName(curdir), filename = NULL, linkname = NULL;
  String desc = NULL, comm = NULL;
  String last_acc = NULL, last_mod = NULL, last_chg = NULL;
  uid_t uid;
  gid_t gid;
  Boolean uid_mask = True, gid_mask = True;
  mode_t mode, mode_mask = S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX;
  char user[100], group[100];
  char ctime_acc[100], ctime_mod[100], ctime_chg[100];
  char path[MAXPATHLEN+1], buf[MAXPATHLEN+1];
  struct passwd *pw;
  struct group *gr;
  Boolean modified = False;

  if (filepane && nFiles > 0) {
    int i, i0 = FindSelected(curdir);
    FilePtr file = dirFile(curdir, i0);
    StatPtr lstats = fileLstats(file);

    Wait();
    if (nFiles == 1) {
      icon = fileLargeIcon(file);
      filename = fileName(file);
      comm = comment(fileType(file));
      if (S_ISLNK(lstats->st_mode)) {
	int len = readlink(pathname(path, dirname, filename), buf, MAXPATHLEN);
	if (len >= 0) {
	  buf[len] = '\0';
	  linkname = buf;
	} else
	  linkname = NULL;
	desc = "Symbolic link";
      } else if (S_ISDIR(lstats->st_mode))
	desc = "Directory";
      else if (S_ISCHR(lstats->st_mode))
	desc = "Character special file";
      else if(S_ISBLK(lstats->st_mode))
	desc = "Block special file";
      else if(S_ISSOCK(lstats->st_mode))
	desc = "Socket";
      else if(S_ISFIFO(lstats->st_mode))
	desc = "Pipe or FIFO special file";
      else
	desc = "Ordinary file";
      strcpy(ctime_acc, ctime(&lstats->st_atime));
      strcpy(ctime_mod, ctime(&lstats->st_mtime));
      strcpy(ctime_chg, ctime(&lstats->st_ctime));
      if (*ctime_acc) ctime_acc[strlen(ctime_acc)-1] = '\0';
      if (*ctime_mod) ctime_mod[strlen(ctime_mod)-1] = '\0';
      if (*ctime_chg) ctime_chg[strlen(ctime_chg)-1] = '\0';
      last_acc = ctime_acc;
      last_mod = ctime_mod;
      last_chg = ctime_chg;
    } else
      icon = &std_icons[ICON_FILES];
    uid = lstats->st_uid;
    gid = lstats->st_gid;
    mode = lstats->st_mode;
    for (i = i0+1; i < dirNFiles(curdir); i++) {
      file = dirFile(curdir, i);
      if (fileSelected(file)) {
	mode_t m;

	lstats = fileLstats(file);
	if (uid_mask && lstats->st_uid != uid)
	  uid_mask = False;
	if (gid_mask && lstats->st_gid != gid)
	  gid_mask = False;
	if ((m = (mode_mask & lstats->st_mode) ^ (mode_mask & mode)))
	  mode_mask &= ~m;
      }
    }
    if (uid_mask)
      if ((pw = getpwuid(uid)) == NULL)
	sprintf(user, "%lu", (unsigned long) uid);
      else
	strcpy(user, pw->pw_name);
    else
      *user = '\0';
    if (gid_mask)
      if ((gr = getgrgid(gid)) == NULL)
	sprintf(group, "%lu", (unsigned long) gid);
      else
	strcpy(group, gr->gr_name);
    else
      *group = '\0';
    if (props_dlg(icon, nFiles, nBytes, dirname, filename, linkname,
		  desc, comm, last_acc, last_mod, last_chg,
		  user, group, 100, &mode, &mode_mask) != CANCEL) {
      /* the following stuff should really be replaced by a fileChmod
	 operation in the fileops module sometime */
      long l;
      String p;

      if (*user)
	if ((l = strtol(user, &p, 10)) >= 0 && *p == '\0')
	  uid = l;
	else if ((pw = getpwnam(user))) {
	  uid = pw->pw_uid;
	  uid_mask = True;
	} else {
	  xplerr1(ERR_INVALID_USR, user);
	  goto exit;
	}
      else
	uid_mask = False;
      if (*group)
	if ((l = strtol(group, &p, 10)) >= 0 && *p == '\0')
	  gid = l;
	else if ((gr = getgrnam(group))) {
	  gid = gr->gr_gid;
	  gid_mask = True;
	} else {
	  xplerr1(ERR_INVALID_GRP, group);
	  goto exit;
	}
      else
	gid_mask = False;
      for (i = i0; i < dirNFiles(curdir); i++) {
	file = dirFile(curdir, i);
	
	if (fileSelected(file)) {
	  mode_t mode1, mode2;

	  lstats = fileLstats(file);
	  pathname(path, dirname, fileName(file));
	  mode1 = lstats->st_mode;
	  mode2 = (mode1 & ~mode_mask) | (mode & mode_mask);
	  if (mode2 != mode1 && chmod(path, mode2)) {
	    syserr1(ERR_CHMOD, path);
	    goto exit;
	  }
	  modified = modified || mode2 != mode1;
	  if ((uid_mask && lstats->st_uid != uid ||
	       gid_mask && lstats->st_gid != gid) &&
	      chown(path,
		    uid_mask?uid:lstats->st_uid, 
		    gid_mask?gid:lstats->st_gid)) {
	    syserr1(ERR_CHOWN, path);
	    goto exit;
	  }
	  modified = modified || uid_mask && lstats->st_uid != uid ||
	    gid_mask && lstats->st_gid != gid;
	}
      }
    }
  exit:
    SavePanes();
    /* force update of the current dir */
    if (modified) dirSetOptions(curdir, MODIFIED);
    update();
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
    Done();
  }
}

#define MAXTEXTLEN 10000
#define MBUFSZ 1024

static String unparse_str(String s, String t)
{
  String buf = alloca(2 * strlen(t) + 1);
  sprintf(s, "\"%s\"", quote(buf, t, "\\\""));
  return s;
}

static String unparse_pattern(String s, String magic, String pattern)
{
  if (!(magic && *magic) && !(pattern && *pattern)) return s;
  if (*s)
    strcat(s, " ");
  if (magic && *magic) {
    String buf = alloca(2 * strlen(magic) + 1);
    sprintf(s+strlen(s), "<%s>", quote(buf, magic, "\\>"));
  }
  if (pattern && *pattern) {
    String buf = alloca(2 * strlen(pattern) + 1);
    strcat(s, quote(buf, pattern, "\\ <"));
  }
  return s;
}

static String unparse_action(String s, String tag, String label,
			     String action, String dirname)
{
  if (strcmp(tag, "Separator") && !(label && *label) && !(action && *action))
    return s;
  strcat(s, tag);
  if (label && *label) {
    strcat(s, " ");
    unparse_str(s+strlen(s), label);
  }
  if (action && *action) {
    strcat(s, (label && *label)?", ":" ");
    unparse_str(s+strlen(s), action);
  }
  if (dirname && *dirname) {
    strcat(s, (label && *label || action && *action)?", ":" ");
    unparse_str(s+strlen(s), dirname);
  }
  strcat(s, "\n");
  return s;
}

static String quote_magic(String s, String t, String c)
{
  Boolean magic = False;
  String s0 = s, t0 = t;
  for (; *t; t++) {
    if (*t == '<' && (t == t0 || t[-1] != '\\'))
      magic = True;
    else if (*t == '>' && (t == t0 || t[-1] != '\\'))
      magic = False;
    if (magic && strchr(c, *t) && (t == t0 || t[-1] != '\\'))
      *s++ = '\\';
    *s++ = *t;
  }
  *s = '\0';
  return s0;
}

static String mysplit2(String s, char c)
{
    static String t;
    if (!s)
	s = t;
    t = s;
    if (t)
	while ((t = strchr(t, c)) && t>s && t[-1]=='\\') t++;
    if (t)
	*t++ = '\0';
    return s;
}

static Boolean parse_type(String patterns_s, Boolean magic_flag_b,
			  String comment_s, String large_icon_s,
			  String small_icon_s, String actions_s)
{
  /* add a new type */
  int type = add_file_type(True);
  /* split the pattern list */
  String p, pat = alloca((2*strlen(patterns_s)+1)*sizeof(char));
  /* magic part <...> may contain unquoted blanks, we quote them
     automagically: */
  quote_magic(pat, patterns_s, " ");
  /* damn split2 is not reentrant, we have to use our own version here
     since split_type already uses split2 */
  if ((p = mysplit2(pat, ' ')))
    do
      if (*p) {
	String pattern = unquote(alloca((strlen(p)+1)*sizeof(char)), p);
	String magic = split_type(&pattern);
	add_pattern(type, magic_flag_b, magic, pattern);
      }
    while ((p = mysplit2(NULL, ' ')));
  /* add comment and icons */
  set_comment(type, (*comment_s)?comment_s:NULL);
  set_icons(type, (*large_icon_s)?large_icon_s:NULL,
	    (*small_icon_s)?small_icon_s:NULL);
  if (*large_icon_s && !large_icon(type))
    xplerr1(ERR_READ_ICON, large_icon_s);
  if (*small_icon_s && !small_icon(type))
    xplerr1(ERR_READ_ICON, small_icon_s);
  /* parse the action list */
  return parse_actions(type, actions_s);
}

static void print_str(FILE *f, String t)
{
  String buf = alloca(2 * strlen(t) + 1);
  String buf2 = alloca(4 * strlen(t) + 1);
  /* needs an extra quote level to escape \ in resource file */
  fprintf(f, "\"%s\"", quote(buf2, quote(buf, t, "\\\""), "\\"));
}

static void print_pattern(FILE *f, String magic, String pattern)
{
  if (!(magic && *magic) && !(pattern && *pattern)) return;
  fprintf(f, "\"");
  if (magic && *magic) {
    String buf = alloca(2 * strlen(magic) + 1);
    String buf2 = alloca(4 * strlen(magic) + 1);
    fprintf(f, "<%s>", quote(buf2, quote(buf, magic, "\\\">"), "\\"));
  }
  if (pattern && *pattern) {
    String buf = alloca(2 * strlen(pattern) + 1);
    String buf2 = alloca(4 * strlen(pattern) + 1);
    fprintf(f, quote(buf2, quote(buf, pattern, "\\\"<"), "\\"));
  }
  fprintf(f, "\"");
}

static void print_action(FILE *f, String tag, String label,
			 String action, String dirname)
{
  if (strcmp(tag, "Separator") && !(label && *label) && !(action && *action))
    return;
  fprintf(f, tag);
  if (label && *label) {
    fprintf(f, " ");
    print_str(f, label);
  }
  if (action && *action) {
    fprintf(f, (label && *label)?", ":" ");
    print_str(f, action);
  }
  if (dirname && *dirname) {
    fprintf(f, (label && *label || action && *action)?", ":" ");
    print_str(f, dirname);
  }
}

#define MAXLEN 1024
#define TIDLINE_START \
  "!!! start of user type definitions -- DO NOT EDIT THIS LINE !!!\n"
#define TIDLINE_END \
  "!!! end of user type definitions -- DO NOT EDIT THIS LINE !!!\n"

void save_types(void)
{
  char tmpfile[MAXPATHLEN+1];
  char buf[MAXLEN+1];
  FILE *src, *dest;
  int count, type, i;

  if (resfile && *resfile) {
    strcpy(tmpfile, resfile);
    strcat(tmpfile, ".$$$");
  }
  if (!resfile || !*resfile || !(dest = fopen(tmpfile, "w"))) {
    syserr(ERR_CREATE_RESFILE);
    return;
  }
  if ((src = fopen(resfile, "r"))) {
    while (!ferror(src) && !feof(src) && !ferror(dest) &&
	   fgets(buf, MAXLEN, src) && strcmp(buf, TIDLINE_START))
      fputs(buf, dest);
    while (!ferror(src) && !feof(src) && fgets(buf, MAXLEN, src) &&
	   strcmp(buf, TIDLINE_END))
      ;
    if (ferror(src) || ferror(dest)) {
      syserr(ERR_CREATE_RESFILE);
      fclose(src); fclose(dest);
      return;
    }
  }
  count = 0;
  for (type = 1; type < n_types(); type++) {
    int base, acttype;
    if (!tmp_flag(type)) continue;
    if (count == 0) {
      fputs(TIDLINE_START, dest);
      fprintf(dest, "xplore.types: ");
    }
    count++;
    for (base = type+1; base < n_types() &&
	   (tmp_flag(base) || n_patterns(base) <= 0); base++) ;
    fprintf(dest, "\\n\\\n");
    if (base >= n_types())
      fprintf(dest, "@Default");
    else {
      if (magic_flag(base, 0))
	fprintf(dest, "@Magic ");
      else
	fprintf(dest, "@Type ");
      for (i = 0; i < n_patterns(base); i++) {
	if (i > 0) fprintf(dest, ", ");
	print_pattern(dest, magic(base, i), pattern(base, i));
      }
    }
    for (acttype = base-1; acttype >= type; acttype--) {
      if (n_patterns(acttype) <= 0) continue;
      fprintf(dest, "\\n\\\n");
      if (magic_flag(acttype, 0))
	fprintf(dest, "Magic ");
      else
	fprintf(dest, "Type ");
      for (i = 0; i < n_patterns(acttype); i++) {
	if (i > 0) fprintf(dest, ", ");
	print_pattern(dest, magic(acttype, i), pattern(acttype, i));
      }
      fprintf(dest, " {\\n\\\n");
      if (comment(acttype)) {
	fprintf(dest, "  Description ");
	print_str(dest, comment(acttype));
	fprintf(dest, "\\n\\\n");
      }
      if (large_icon_name(acttype)) {
	fprintf(dest, "  LargeIcon ");
	print_str(dest, large_icon_name(acttype));
	fprintf(dest, "\\n\\\n");
      }
      if (small_icon_name(acttype)) {
	fprintf(dest, "  SmallIcon ");
	print_str(dest, small_icon_name(acttype));
	fprintf(dest, "\\n\\\n");
      }
      if (push_action(acttype)) {
	fprintf(dest, "  ");
	print_action(dest, "Push", NULL, push_action(acttype),
		     push_dirname(acttype));
	fprintf(dest, "\\n\\\n");
      }
      if (drop_action(acttype)) {
	fprintf(dest, "  ");
	print_action(dest, "Drop", NULL, drop_action(acttype),
		     drop_dirname(acttype));
	fprintf(dest, "\\n\\\n");
      }
      for (i = 0; i < n_menu_actions(acttype); i++) {
	fprintf(dest, "  ");
	if (menu_label(acttype, i))
	  print_action(dest, "Item", menu_label(acttype, i),
		       menu_action(acttype, i), menu_dirname(acttype, i));
	else
	  print_action(dest, "Separator", NULL, NULL, NULL);
	fprintf(dest, "\\n\\\n");
      }
      fprintf(dest, "}\\n");
    }
    type = base-1;
  }
  if (count > 0) {
    fprintf(dest, "\n");
    fputs(TIDLINE_END, dest);
  }
  if (src) {
    while (!ferror(src) && !feof(src) && !ferror(dest) &&
	   fgets(buf, MAXLEN, src))
      fputs(buf, dest);
    if (ferror(src) || ferror(dest)) {
      syserr(ERR_CREATE_RESFILE);
      fclose(src); fclose(dest);
      Done();
      return;
    }
    fclose(src);
  }
  if (fclose(dest))
    syserr(ERR_CREATE_RESFILE);
  else {
    unlink(resfile);
    if (rename(tmpfile, resfile))
      syserr(ERR_CREATE_RESFILE);
  }
}

static Boolean is_empty_str(String s)
{
  String t;
  for (t = s; *t; t++)
    if (*t != ' ')
      return False;
  return True;
}

static int ntypes = 0;
static String *types = NULL;

static void unparse_type_list()
{
  int type;

  ntypes = n_types();
  types = (String*)MALLOC(ntypes * sizeof(String));
  for (type = 1; type < n_types(); type++) {
    char patterns[MAXTEXTLEN];
    int i;

    *patterns = '\0';
    for (i = 0; i < n_patterns(type); i++)
      unparse_pattern(patterns, magic(type, i), pattern(type, i));
    types[type-1] = NEWSTRING(patterns);
  }
  types[ntypes-1] = NEWSTRING("(Default)");
}

static void free_type_list()
{
  if (types) {
    int i;
    for (i = 0; i < ntypes; i++) FREE(types[i]);
    FREE(types);
    types = NULL;
  }
}

#define type_pos(type) ((type==DFLT)?ntypes-1:type-1)
#define pos_type(pos) (pos+1)

void BindingsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int item;

  if (filepane && dirNFilesSelected(curdir) == 1 &&
      (item = FindSelected(curdir)) != NONE) {
    DirPtr dir = curdir;
    FilePtr file = dirFile(dir, item);
    IconPtr icon = fileLargeIcon(file);
    int type = fileType(file), ntype;
    Boolean edited, magic_flag_b, use_default_b;
    char patterns_s[MAXTEXTLEN], default_s[MAXTEXTLEN], comment_s[MAXTEXTLEN],
      large_icon_s[MAXTEXTLEN], small_icon_s[MAXTEXTLEN],
      actions_s[MAXTEXTLEN];
    int i, res, pos;
    char magic_s[MBUFSZ];
    char s[MAXPATHLEN+1];
    StatPtr stats = fileStats(file);
    String pat;

    /* unparse pattern */
    magic_flag_b = n_patterns(type)?magic_flag(type, 0):False;
    *patterns_s = '\0';
    for (i = 0; i < n_patterns(type); i++)
      unparse_pattern(patterns_s, magic(type, i), pattern(type, i));
    /* create a reasonable default pattern */
    *default_s = '\0';
    magic_get_type(pathname(s, dirName(dir), fileName(file)), magic_s);
    unparse_pattern(default_s, magic_s, s);
    if (!*patterns_s &&
	(S_ISDIR(stats->st_mode) ||
	(stats->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))))
      magic_flag_b = True;
    /* get the remaining type info */
    if (comment(type))
      strcpy(comment_s, comment(type));
    else
      *comment_s = '\0';
    use_default_b = (*patterns_s == '\0');
    if (large_icon_name(type))
      strcpy(large_icon_s, large_icon_name(type));
    else
      *large_icon_s = '\0';
    if (small_icon_name(type))
      strcpy(small_icon_s, small_icon_name(type));
    else
      *small_icon_s = '\0';
    /* unparse the actions */
    *actions_s = '\0';
    if (push_action(type))
      unparse_action(actions_s, "Push", NULL, push_action(type),
		     push_dirname(type));
    if (drop_action(type))
      unparse_action(actions_s, "Drop", NULL, drop_action(type),
		     drop_dirname(type));
    for (i = 0; i < n_menu_actions(type); i++)
      if (menu_label(type, i))
	unparse_action(actions_s, "Item", menu_label(type, i),
		       menu_action(type, i), menu_dirname(type, i));
      else
	unparse_action(actions_s, "Separator", NULL, NULL, NULL);
    errline = 0;
    /* build the type list */
    unparse_type_list();
    pos = type_pos(type);
    edited = False;
retry:
    res = bind_dlg(icon, type, errline, patterns_s, default_s,
		   &use_default_b, &magic_flag_b,
		   comment_s, large_icon_s, small_icon_s, actions_s,
		   types, ntypes, &pos,
		   MAXTEXTLEN, &edited);
    pat = use_default_b ? default_s : patterns_s;
    switch (res) {
    case REPLACE2:
      if (!edited) goto out;
    case ADD2:
      if (is_empty_str(pat)) {
	xplerr(ERR_EMPTY_PATTERN);
	goto retry;
      } else if (!parse_type(pat, magic_flag_b, comment_s,
			     large_icon_s, small_icon_s, actions_s)) {
	pop_file_type();
	xplerr1(ERR_PARSE_TYPE, errstr);
	goto retry;
      } else if (!match_file_type(n_types()-1,
				  pathname(s, dirName(dir), fileName(file)),
				  !(dirOptions(dir) & NOMAGIC)?True:False) &&
		 confirm_nomatch_type_dlg(NULL, pat, fileName(file)) != OK) {
	pop_file_type();
	goto retry;
      }
      if (res == REPLACE2) {
	int dest = pos_type(pos);
	move_file_type(dest);
	if (dest <= type)
	  delete_file_type(type+1);
	else
	  delete_file_type(type);
      } else
	move_file_type(pos_type(pos));
      break;
    case DELETE:
      if (confirm_delete_type_dlg(NULL, type, pat) == OK)
	delete_file_type(type);
      else
	goto out;
      break;
    default:
      goto out;
    }
    Wait();
    save_types();
    Restat();
    Done();
out:
    free_type_list();
  }
}

#define MAXPATLEN 5000

void SelectCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    char pattern[MAXPATLEN+1];

    XmUpdateDisplay(app_shell);
    *pattern = '\0';
    switch (select_dlg(&std_icons[ICON_FILES], pattern, MAXPATLEN)) {
    case REPLACE:
      Wait();
      dirDeselect(curdir, ALL, NULL);
      dirSelect(curdir, ALL, pattern);
      break;
    case ADD:
      Wait();
      dirSelect(curdir, ALL, pattern);
      break;
    case REMOVE:
      Wait();
      dirDeselect(curdir, ALL, pattern);
      break;
    default:
      return;
    }
    SavePanes();
    UpdatePanes(False, False, True, True, True, True, False);
    Done();
  }
}

void SelectAllCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    dirSelect(curdir, ALL, "*");
    SavePanes();
    UpdatePanes(False, False, True, True, True, True, False);
    Done();
  }
}

void InvertAllCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    dirInvert(curdir, ALL, "*");
    SavePanes();
    UpdatePanes(False, False, True, True, True, True, False);
    Done();
  }
}

void FilterCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    int ret;
    char pattern[MAXPATLEN+1];

    XmUpdateDisplay(app_shell);
    if (dirFilter(curdir))
      strcpy(pattern, dirFilter(curdir));
    else
      *pattern = '\0';
    if ((ret = filter_dlg(&std_icons[ICON_FILES], pattern, MAXPATLEN))
	!= CANCEL) {
      Wait();
      dirSetFilter(curdir, (ret == CLEAR)?NULL:pattern);
      SavePanes();
      UpdatePanes(False, True, True, True, True, False, False);
      Done();
    }
  }
}

void QuitCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  XmUpdateDisplay(app_shell);
  if (confirm_quit_dlg(NULL) == OK)
    quit();
}

void MenuActionCB(Widget w, int j, XtPointer widget_data)
{
  int i;

  if (filepane && dirNFilesSelected(curdir) == 1 &&
      (i = FindSelected(curdir)) != NONE) {
    XmUpdateDisplay(app_shell);
    fileMenu(curdir, i, j, absolute, False);
    Update();
  }
}

/* ----------------------------------------------------------------------- */

void ShelfOpenCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int i;

  if (shelfpane && dirNFilesSelected(shelfdir) == 1 &&
      (i = FindSelected(shelfdir)) !=
      NONE) {
    SavePanes();
    shelf_push(i, absolute, pushdir);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void ShelfRenameCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int i;

  if (shelfpane && dirNFilesSelected(shelfdir) == 1 &&
      (i = FindSelected(shelfdir)) !=
      NONE) {
    FilePtr file = dirFile(shelfdir, i);
    char target[MAXPATHLEN+1], abstarget[MAXPATHLEN+1];
    StatRec stats;

    XmUpdateDisplay(app_shell);
    strcpy(target, fileName(file));
    if (rename_dlg(fileLargeIcon(file), dirName(curdir), target, MAXPATHLEN) == OK &&
	strcmp(target, fileName(file)))
      if (!stat(abspath(abstarget, dirName(shelfdir), target), &stats) &&
	  S_ISDIR(stats.st_mode))
	xplerr1(ERR_TARGET_IS_DIR, target);
      else {
	SavePanes();
	shelf_move(target, absolute);
	UpdatePanes(shelf_modified, root_modified, cur_modified,
		    True, True, !cur_changed, cur_changed);
      }
  }
}

void ShelfMoveCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane && dirNFilesSelected(shelfdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (move_dlg(sel_icon(shelfdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      shelf_move(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void ShelfCopyCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane && dirNFilesSelected(shelfdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (copy_dlg(sel_icon(shelfdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      shelf_copy(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void ShelfLinkCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane && dirNFilesSelected(shelfdir)) {
    char target[MAXPATHLEN+1];

    XmUpdateDisplay(app_shell);
    *target = '\0';
    if (link_dlg(sel_icon(shelfdir), dirName(curdir), target, MAXPATHLEN) == OK) {
      SavePanes();
      shelf_link(target, absolute);
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
  }
}

void ShelfDeleteCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane && dirNFilesSelected(shelfdir)) {
    XmUpdateDisplay(app_shell);
    SavePanes();
    shelf_delete();
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void ShelfPropertiesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  IconPtr icon = NULL;
  int nFiles = dirNFilesSelected(shelfdir);
  int nBytes = dirNBytesSelected(shelfdir);
  String dirname = dirName(shelfdir), filename = NULL, linkname = NULL;
  String desc = NULL, comm = NULL;
  String last_acc = NULL, last_mod = NULL, last_chg = NULL;
  uid_t uid;
  gid_t gid;
  Boolean uid_mask = True, gid_mask = True;
  mode_t mode, mode_mask = S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX;
  char user[100], group[100];
  char ctime_acc[100], ctime_mod[100], ctime_chg[100];
  char path[MAXPATHLEN+1], buf[MAXPATHLEN+1];
  struct passwd *pw;
  struct group *gr;
  Boolean modified = False;

  if (shelfpane && nFiles > 0) {
    int i, i0 = FindSelected(shelfdir);
    FilePtr file = dirFile(shelfdir, i0);
    StatPtr lstats = fileLstats(file);

    Wait();
    if (nFiles == 1) {
      icon = fileLargeIcon(file);
      filename = fileName(file);
      comm = comment(fileType(file));
      if (S_ISLNK(lstats->st_mode)) {
	int len = readlink(pathname(path, dirname, filename), buf, MAXPATHLEN);
	if (len >= 0) {
	  buf[len] = '\0';
	  linkname = buf;
	} else
	  linkname = NULL;
	desc = "Symbolic link";
      } else if (S_ISDIR(lstats->st_mode))
	desc = "Directory";
      else if (S_ISCHR(lstats->st_mode))
	desc = "Character special file";
      else if(S_ISBLK(lstats->st_mode))
	desc = "Block special file";
      else if(S_ISSOCK(lstats->st_mode))
	desc = "Socket";
      else if(S_ISFIFO(lstats->st_mode))
	desc = "Pipe or FIFO special file";
      else
	desc = "Ordinary file";
      strcpy(ctime_acc, ctime(&lstats->st_atime));
      strcpy(ctime_mod, ctime(&lstats->st_mtime));
      strcpy(ctime_chg, ctime(&lstats->st_ctime));
      if (*ctime_acc) ctime_acc[strlen(ctime_acc)-1] = '\0';
      if (*ctime_mod) ctime_mod[strlen(ctime_mod)-1] = '\0';
      if (*ctime_chg) ctime_chg[strlen(ctime_chg)-1] = '\0';
      last_acc = ctime_acc;
      last_mod = ctime_mod;
      last_chg = ctime_chg;
    } else
      icon = &std_icons[ICON_FILES];
    uid = lstats->st_uid;
    gid = lstats->st_gid;
    mode = lstats->st_mode;
    for (i = i0+1; i < dirNFiles(shelfdir); i++) {
      file = dirFile(shelfdir, i);
      if (fileSelected(file)) {
	mode_t m;

	lstats = fileLstats(file);
	if (uid_mask && lstats->st_uid != uid)
	  uid_mask = False;
	if (gid_mask && lstats->st_gid != gid)
	  gid_mask = False;
	if ((m = (mode_mask & lstats->st_mode) ^ (mode_mask & mode)))
	  mode_mask &= ~m;
      }
    }
    if (uid_mask)
      if ((pw = getpwuid(uid)) == NULL)
	sprintf(user, "%lu", (unsigned long) uid);
      else
	strcpy(user, pw->pw_name);
    else
      *user = '\0';
    if (gid_mask)
      if ((gr = getgrgid(gid)) == NULL)
	sprintf(group, "%lu", (unsigned long) gid);
      else
	strcpy(group, gr->gr_name);
    else
      *group = '\0';
    if (props_dlg(icon, nFiles, nBytes, dirname, filename, linkname,
		  desc, comm, last_acc, last_mod, last_chg,
		  user, group, 100, &mode, &mode_mask) != CANCEL) {
      /* the following stuff should really be replaced by a fileChmod
	 operation in the fileops module sometime */
      long l;
      String p;

      if (*user)
	if ((l = strtol(user, &p, 10)) >= 0 && *p == '\0')
	  uid = l;
	else if ((pw = getpwnam(user))) {
	  uid = pw->pw_uid;
	  uid_mask = True;
	} else {
	  xplerr1(ERR_INVALID_USR, user);
	  goto exit;
	}
      else
	uid_mask = False;
      if (*group)
	if ((l = strtol(group, &p, 10)) >= 0 && *p == '\0')
	  gid = l;
	else if ((gr = getgrnam(group))) {
	  gid = gr->gr_gid;
	  gid_mask = True;
	} else {
	  xplerr1(ERR_INVALID_GRP, group);
	  goto exit;
	}
      else
	gid_mask = False;
      for (i = i0; i < dirNFiles(shelfdir); i++) {
	file = dirFile(shelfdir, i);
	
	if (fileSelected(file)) {
	  mode_t mode1, mode2;

	  lstats = fileLstats(file);
	  pathname(path, dirname, fileName(file));
	  mode1 = lstats->st_mode;
	  mode2 = (mode1 & ~mode_mask) | (mode & mode_mask);
	  if (mode2 != mode1 && chmod(path, mode2)) {
	    syserr1(ERR_CHMOD, path);
	    goto exit;
	  }
	  modified = modified || mode2 != mode1;
	  if ((uid_mask && lstats->st_uid != uid ||
	       gid_mask && lstats->st_gid != gid) &&
	      chown(path,
		    uid_mask?uid:lstats->st_uid, 
		    gid_mask?gid:lstats->st_gid)) {
	    syserr1(ERR_CHOWN, path);
	    goto exit;
	  }
	  modified = modified || uid_mask && lstats->st_uid != uid ||
	    gid_mask && lstats->st_gid != gid;
	}
      }
    }
  exit:
    SavePanes();
    /* force update of the shelf dir */
    if (modified) dirSetOptions(shelfdir, MODIFIED);
    update();
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
    Done();
  }
}

void ShelfBindingsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int item;

  if (shelfpane && dirNFilesSelected(shelfdir) == 1 &&
      (item = FindSelected(shelfdir)) != NONE) {
    DirPtr dir = shelfdir;
    FilePtr file = dirFile(dir, item);
    IconPtr icon = fileLargeIcon(file);
    int type = fileType(file), ntype;
    Boolean edited, magic_flag_b, use_default_b;
    char patterns_s[MAXTEXTLEN], default_s[MAXTEXTLEN], comment_s[MAXTEXTLEN],
      large_icon_s[MAXTEXTLEN], small_icon_s[MAXTEXTLEN],
      actions_s[MAXTEXTLEN];
    int i, res, pos;
    char magic_s[MBUFSZ];
    char s[MAXPATHLEN+1];
    StatPtr stats = fileStats(file);
    String pat;

    magic_flag_b = n_patterns(type)?magic_flag(type, 0):False;
    *patterns_s = '\0';
    for (i = 0; i < n_patterns(type); i++)
      unparse_pattern(patterns_s, magic(type, i), pattern(type, i));
    *default_s = '\0';
    magic_get_type(pathname(s, dirName(dir), fileName(file)), magic_s);
    unparse_pattern(default_s, magic_s, s);
    if (!*patterns_s &&
	(S_ISDIR(stats->st_mode) ||
	(stats->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))))
      magic_flag_b = True;
    use_default_b = (*patterns_s == '\0');
    if (comment(type))
      strcpy(comment_s, comment(type));
    else
      *comment_s = '\0';
    if (large_icon_name(type))
      strcpy(large_icon_s, large_icon_name(type));
    else
      *large_icon_s = '\0';
    if (small_icon_name(type))
      strcpy(small_icon_s, small_icon_name(type));
    else
      *small_icon_s = '\0';
    *actions_s = '\0';
    if (push_action(type))
      unparse_action(actions_s, "Push", NULL, push_action(type),
		     push_dirname(type));
    if (drop_action(type))
      unparse_action(actions_s, "Drop", NULL, drop_action(type),
		     drop_dirname(type));
    for (i = 0; i < n_menu_actions(type); i++)
      if (menu_label(type, i))
	unparse_action(actions_s, "Item", menu_label(type, i),
		       menu_action(type, i), menu_dirname(type, i));
      else
	unparse_action(actions_s, "Separator", NULL, NULL, NULL);
    errline = 0;
    unparse_type_list();
    pos = type_pos(type);
    edited = False;
retry:
    res = bind_dlg(icon, type, errline, patterns_s, default_s,
		   &use_default_b, &magic_flag_b,
		   comment_s, large_icon_s, small_icon_s, actions_s,
		   types, ntypes, &pos,
		   MAXTEXTLEN, &edited);
    pat = use_default_b ? default_s : patterns_s;
    switch (res) {
    case REPLACE2:
      if (!edited) goto out;
    case ADD2:
      if (is_empty_str(pat)) {
	xplerr(ERR_EMPTY_PATTERN);
	goto retry;
      } else if (!parse_type(pat, magic_flag_b, comment_s,
			     large_icon_s, small_icon_s, actions_s)) {
	pop_file_type();
	xplerr1(ERR_PARSE_TYPE, errstr);
	goto retry;
      } else if (!match_file_type(n_types()-1,
				  pathname(s, dirName(dir), fileName(file)),
				  !(dirOptions(dir) & NOMAGIC)?True:False) &&
		 confirm_nomatch_type_dlg(NULL, pat, fileName(file)) != OK) {
	pop_file_type();
	goto retry;
      }
      if (res == REPLACE2) {
	int dest = pos_type(pos);
	move_file_type(dest);
	if (dest <= type)
	  delete_file_type(type+1);
	else
	  delete_file_type(type);
      } else
	move_file_type(pos_type(pos));
      break;
    case DELETE:
      if (confirm_delete_type_dlg(NULL, type, pat) == OK)
	delete_file_type(type);
      else
	goto out;
      break;
    default:
      goto out;
    }
    Wait();
    save_types();
    Restat();
    Done();
out:
    free_type_list();
  }
}

void ShelfSelectCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane) {
    char pattern[MAXPATLEN+1];

    XmUpdateDisplay(app_shell);
    *pattern = '\0';
    switch (select_dlg(&std_icons[ICON_FILES], pattern, MAXPATLEN)) {
    case REPLACE:
      Wait();
      dirDeselect(shelfdir, ALL, NULL);
      dirSelect(shelfdir, ALL, pattern);
      break;
    case ADD:
      Wait();
      dirSelect(shelfdir, ALL, pattern);
      break;
    case REMOVE:
      Wait();
      dirDeselect(shelfdir, ALL, pattern);
      break;
    default:
      return;
    }
    SavePanes();
    UpdatePanes(True, False, False, True, True, True, False);
    Done();
  }
}

void ShelfSelectAllCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane) {
    Wait();
    dirSelect(shelfdir, ALL, "*");
    SavePanes();
    UpdatePanes(True, False, False, True, True, True, False);
    Done();
  }
}

void ShelfInvertAllCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane) {
    Wait();
    dirInvert(shelfdir, ALL, "*");
    SavePanes();
    UpdatePanes(True, False, False, True, True, True, False);
    Done();
  }
}

void ShelfMenuActionCB(Widget w, int j, XtPointer widget_data)
{
  int i;

  if (shelfpane && dirNFilesSelected(shelfdir) == 1 &&
      (i = FindSelected(shelfdir)) !=
      NONE) {
    XmUpdateDisplay(app_shell);
    fileMenu(shelfdir, i, j, absolute, True);
    Update();
  }
}

/* ----------------------------------------------------------------------- */

void LargeIconsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && viewtype != LargeIconView) {
    viewtype = LargeIconView;
    UpdateMenus();
    Wait();
    SavePanes();
    UpdatePanes(False, False, True, True, True, False, False);
    Done();
  }
}

void SmallIconsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && viewtype != SmallIconView) {
    viewtype = SmallIconView;
    UpdateMenus();
    Wait();
    SavePanes();
    UpdatePanes(False, False, True, True, True, False, False);
    Done();
  }
}

void DetailCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane && viewtype != DetailView) {
    viewtype = DetailView;
    UpdateMenus();
    Wait();
    SavePanes();
    UpdatePanes(False, False, True, True, True, False, False);
    Done();
  }
}

void TreePaneCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (treepane)
    HideTreePane();
  else
    ShowTreePane();
}

void FilePaneCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane)
    HideFilePane();
  else
    ShowFilePane();
}

void ShelfPaneCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (shelfpane)
    HideShelfPane();
  else
    ShowShelfPane();
}

void LogPaneCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (logpane)
    HideLogPane();
  else
    ShowLogPane();
}

void SortByNameCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    UpdateMenus();
    Wait();
    if ((dirOptions(curdir) & SORT_MASK) != SORT_BY_NAME) {
      dirSetOptions(curdir, SORT_BY_NAME);
      SavePanes();
      UpdatePanes(False, True, True, True, True, False, False);
    }
    Done();
  }
}

void SortBySizeCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    UpdateMenus();
    Wait();
    if ((dirOptions(curdir) & SORT_MASK) != SORT_BY_SIZE) {
      dirSetOptions(curdir, SORT_BY_SIZE);
      SavePanes();
      UpdatePanes(False, True, True, True, True, False, False);
    }
    Done();
  }
}

void SortByDateCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    UpdateMenus();
    Wait();
    if ((dirOptions(curdir) & SORT_MASK) != SORT_BY_DATE) {
      dirSetOptions(curdir, SORT_BY_DATE);
      SavePanes();
      UpdatePanes(False, True, True, True, True, False, False);
    }
    Done();
  }
}

void ReverseCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    if (dirOptions(curdir) & REVERSE)
      dirClearOptions(curdir, REVERSE);
    else
      dirSetOptions(curdir, REVERSE);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void DirsFirstCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    if (dirOptions(curdir) & DIRS_FIRST)
      dirClearOptions(curdir, DIRS_FIRST);
    else
      dirSetOptions(curdir, DIRS_FIRST);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void ShowHiddenFilesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane || treepane) {
    Wait();
    if (dirOptions(curdir) & INCLUDE_HIDDEN)
      dirClearOptions(curdir, INCLUDE_HIDDEN);
    else
      dirSetOptions(curdir, INCLUDE_HIDDEN);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void ShowParentDirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    if (dirOptions(curdir) & INCLUDE_UPDIR)
      dirClearOptions(curdir, INCLUDE_UPDIR);
    else
      dirSetOptions(curdir, INCLUDE_UPDIR);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void ShowFilesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    if (dirOptions(curdir) & INCLUDE_FILES)
      dirClearOptions(curdir, INCLUDE_FILES);
    else
      dirSetOptions(curdir, INCLUDE_FILES);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void ShowDirsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (filepane) {
    Wait();
    if (dirOptions(curdir) & INCLUDE_DIRS)
      dirClearOptions(curdir, INCLUDE_DIRS);
    else
      dirSetOptions(curdir, INCLUDE_DIRS);
    SavePanes();
    UpdatePanes(False, True, True, True, True, False, False);
    Done();
  }
}

void RescanFilesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (!filepane && !treepane)
    return;
  Wait();
  if (dirOptions(curdir) & CHECK_FILES)
    dirClearOptions(curdir, CHECK_FILES);
  else
    dirSetOptions(curdir, CHECK_FILES);
  SavePanes();
  UpdatePanes(False, True, True, True, True, True, False);
  Done();
}

void MagicHeadersCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (!filepane && !treepane)
    return;
  Wait();
  if (dirOptions(curdir) & NOMAGIC)
    dirClearOptions(curdir, NOMAGIC);
  else
    dirSetOptions(curdir, NOMAGIC);
  SavePanes();
  UpdatePanes(False, True, True, True, True, True, False);
  Done();
}

void UpdateCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (!filepane && !treepane && ! shelfpane)
    return;
  XmUpdateDisplay(app_shell);
  Update();
}

void RereadCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (!filepane && !treepane && ! shelfpane)
    return;
  XmUpdateDisplay(app_shell);
  Restat();
}

void UnmountCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  DirPtr dir = curdir;

  if (!filepane && !treepane && ! shelfpane)
    return;
  if (dirDev(dir) == NONE || dirDev(dir) == ANY) return;
  XmUpdateDisplay(app_shell);
  SavePanes();
  Wait();
  while (dirParent(dir) && dirDev(dirParent(dir)) == dirDev(dir))
    dir = dirParent(dir);
  dirUnmountAll(dir);
  shelf_modified = dirModified(shelfdir);
  if (shelf_modified)
    dirUpdate(shelfdir);
  cur_check();
  UpdatePanes(shelf_modified || !dirIsReadable(shelfdir), True, cur_changed ||
	      !dirIsReadable(curdir), True, True, !cur_changed &&
	      dirIsReadable(curdir), cur_changed);
  Done();
}

void UnmountAllCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (!filepane && !treepane && ! shelfpane)
    return;
  XmUpdateDisplay(app_shell);
  SavePanes();
  Wait();
  dirUnmountAll(rootdir);
  dirUnmountAll(shelfdir);
  cur_check();
  UpdatePanes(!dirIsReadable(shelfdir), True, cur_changed ||
	      !dirIsReadable(curdir), True, True, !cur_changed &&
	      dirIsReadable(curdir), cur_changed);
  Done();
}

static int lines = 0;

void ClearLogCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  XmTextSetString(log_scroll, "");
  lines = 0;
}

/* ----------------------------------------------------------------------- */

void AbsolutePathsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  absolute = !absolute;
}

void TargetDirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  dirtarget = !dirtarget;
}

void PushDirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  pushdir = !pushdir;
}

void EchoCommandsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  echo = !echo;
}

void CheckMountsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  check = !check;
}

void BackupsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  backups = !backups;
}

void ShowSplashCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  splash = !splash;
  /*
  warn_dlg(NULL, "You must save the defaults for this option to take effect.");
  */
}

void AutosaveDefaultsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  autosave = !autosave;
}

void AutosaveSessionCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  autosession = !autosession;
}

void AutopopupCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  autopopup = !autopopup;
}

void AutoUpdatesCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (updates) {
    XtRemoveTimeOut(timer_id);
    updates = False;
  } else {
    updates = update_time > 0;
    if (updates)
      timer_id = XtAppAddTimeOut(app, update_time, TimerCB, NULL);
  }
}

void ConfirmDropCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_drop = !confirm_drop;
}

void ConfirmMoveCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_move = !confirm_move;
}

void ConfirmCopyCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_copy = !confirm_copy;
}

void ConfirmLinkCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_link = !confirm_link;
}

void ConfirmDeleteCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_delete = !confirm_delete;
}

void ConfirmDelDirCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_deldir = !confirm_deldir;
}

void ConfirmDelFileCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_delfile = !confirm_delfile;
}

void ConfirmOverwriteCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_overwrt = !confirm_overwrt;
}

void ConfirmQuitCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  confirm_quit = !confirm_quit;
}

#define IDLINE_START \
  "!!! start of xplore resource section -- DO NOT EDIT THIS LINE !!!\n"
#define IDLINE_END \
  "!!! end of xplore resource section -- DO NOT EDIT THIS LINE !!!\n"

static void SaveDefaults(void)
{
  char tmpfile[MAXPATHLEN+1];
  char buf[MAXLEN+1];
  FILE *src, *dest;
  Position x, y;
  Dimension width, height;

  if (resfile && *resfile) {
    strcpy(tmpfile, resfile);
    strcat(tmpfile, ".$$$");
  }
  if (!resfile || !*resfile || !(dest = fopen(tmpfile, "w"))) {
    syserr(ERR_CREATE_RESFILE);
    return;
  }
  if ((src = fopen(resfile, "r"))) {
    while (!ferror(src) && !feof(src) && !ferror(dest) &&
	   fgets(buf, MAXLEN, src) && strcmp(buf, IDLINE_START))
      fputs(buf, dest);
    while (!ferror(src) && !feof(src) && fgets(buf, MAXLEN, src) &&
	   strcmp(buf, IDLINE_END))
      ;
    if (ferror(src) || ferror(dest)) {
      syserr(ERR_CREATE_RESFILE);
      fclose(src); fclose(dest);
      return;
    }
  }
  fputs(IDLINE_START, dest);
  if (color_scheme)
    fprintf(dest, "xplore.scheme: %s\n", color_scheme);
  fprintf(dest, "! geometry\n");
  XtVaGetValues(app_shell, XmNheight, &height, XmNwidth, &width, NULL);
  fprintf(dest, "xplore.width: %d\n", width);
  fprintf(dest, "xplore.height: %d\n", height);
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
  fputs(IDLINE_END, dest);
  if (src) {
    while (!ferror(src) && !feof(src) && !ferror(dest) &&
	   fgets(buf, MAXLEN, src))
      fputs(buf, dest);
    if (ferror(src) || ferror(dest)) {
      syserr(ERR_CREATE_RESFILE);
      fclose(src); fclose(dest);
      return;
    }
    fclose(src);
  }
  if (fclose(dest))
    syserr(ERR_CREATE_RESFILE);
  else {
    unlink(resfile);
    if (rename(tmpfile, resfile))
      syserr(ERR_CREATE_RESFILE);
  }
}

void SaveDefaultsCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  Wait();
  SaveDefaults();
  Done();
}

void SchemeCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  char name[MAXPATHLEN+1], *tmp;
  int ret;

  XmUpdateDisplay(app_shell);
  if (color_scheme)
    strcpy(name, color_scheme);
  else
    *name = '\0';
  if ((ret = scheme_dlg(&std_icons[ICON_FILE], name, MAXPATHLEN)) != CANCEL) {
    if (ret == CLEAR) *name = '\0';
    XmUpdateDisplay(app_shell);
    if (color_scheme) FREE(color_scheme);
    color_scheme = NEWSTRING(name);
    /* reset to default resources */
    if (db0) {
      XrmSetDatabase(display, db0);
      XrmDestroyDatabase(db);
      db = db0;
      tmp = tmpnam(NULL);
      XrmPutFileDatabase(db, tmp);
      db0 = XrmGetFileDatabase(tmp);
      unlink(tmp);
    }
    /* get the new resources */
    XrmCombineFileDatabase(name, &db, True);
    /* recreate the application */
    recreate_app();
  }
}

void SaveSessionCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  SaveSession(True);
  UpdateMenus();
}

extern Bool save_in_progress;

void ClearSessionCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (*sessionfile) {
    save_in_progress = False;
    ClearSession();
    UpdateMenus();
  }
}

static Boolean ReloadWorkProc(XtPointer p)
{
  /* looks like we have to invoke this as a background procedure s.t. old
     icons don't get disposed before the display is refreshed */
  refresh_icons();
  return True;
}

void ReloadConfigCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  int d;
  
  Wait();
  dirUnmountAll(rootdir);
  dirUnmountAll(shelfdir);
  cur_check();
  if (exists(magicfile))
    magic_parse_file(magicfile);
  else
    magic_parse_file(libmagic);
  reinit_icons();
  initdevs(); init_file_types(); init_menus();
  if (exists(configfile))
    parse_config_file(configfile, CPP, cpp_options);
  else
    parse_config_file(libconfig, CPP, cpp_options);
  reload_user_types();
  parse_config_string(usertypes);
  if (!n_cmd_actions())
    default_cmd_menu();
  if (!n_shelves())
    default_shelf_menu(curshelf);
  restat();
  reinit();
  Done();
  XtAppAddWorkProc(app, ReloadWorkProc, NULL);
}

/* ----------------------------------------------------------------------- */

void HelpCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  String doc = (String)app_data;
  char cmd[MAXPATHLEN+30];
  sprintf(cmd, "%s/netscape-remote %s", libdir, doc);
  fileExec(cmd, dirName(curdir), NULL);
}

void AboutCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  about_dlg(xplore_logo());
}

/* ----------------------------------------------------------------------- */

static char locpath[MAXPATHLEN+1];

void LogPopupCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  XmPopupHandlerCallbackStruct *cbs = (XmPopupHandlerCallbackStruct*)
    call_data;
  XButtonPressedEvent *ev = (XButtonPressedEvent*)cbs->event;
  XmTextPosition pos = XmTextXYToPos(w, ev->x, ev->y), start, end;
  char *s, sub[MAXPATHLEN+1];
  struct stat st;
  XmTextGetSelectionPosition(w, &start, &end);
  if (pos < start || pos >= end) {
    char *log = XmTextGetString(w);
    for (start = pos; start > 0; start--)
      if (log[start-1] == '\n') break;
    for (end = pos; log[end] != 0 && log[end] != '\n'; end++)
      ;
    FREE(log);
    XmTextSetSelection(w, start, end, XtLastTimestampProcessed(display));
  }
  if ((s = XmTextGetSelection(w))) {
    int l = strlen(s);
    if (l > 0 && s[l-1] == '\n') s[--l] = 0;
    if (l == 0 || l > MAXPATHLEN) {
      FREE(s); goto error;
    }
  } else
    goto error;
  abspath(locpath, dirName(curdir), s);
  FREE(s);
  if (lstat(locpath, &st)) goto error;
  XtSetSensitive(locate_button, True);
  return;
error:
  locpath[0] = 0;
  XtSetSensitive(locate_button, False);
}

extern int loc_num;

void LocateCB(Widget w, XtPointer client_data, XtPointer call_data)
{
  char dirname[MAXPATHLEN+1], filename[MAXPATHLEN+1];
  int res, i = NONE;

  if (!filepane && !treepane)
    return;
  dirpart(dirname, locpath);
  filepart(filename, locpath);
  SavePanes();
  if (!(res = cur_chdir(dirname)))
    xplerr1(ERR_FIND_DIR, dirname);
  if (res) {
    dirDeselect(curdir, ALL, NULL);
    if (*filename) {
      for (i = 0; i < dirNFiles(curdir); i++)
	if (!strcmp(filename, fileName(dirFile(curdir, i))))
	  break;
      if (i < dirNFiles(curdir))
	dirSelect(curdir, i, NULL);
      else
	i = NONE;
    } else
      i = 0;
  }
  loc_num = i;
  UpdatePanes(shelf_modified, root_modified, res,
	      True, True, i!=NONE, cur_changed);
}

/* ----------------------------------------------------------------------- */

void ShelfCB(Widget w, XtPointer app_data, XgTabsCallbackStruct *tcs)
{
  int i = tcs->tab;
  if (shelfpane && i != shelf_no) {
    SavePanes();
    shelf_goto(i);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		False, True, True, False);
  }
}

/* ----------------------------------------------------------------------- */

static void WaitRelease(void)
{
  /* wait until the button or key is released after a default action,
     to prevent spurious selections in the file pane (Motif 2.1 only??) */
  XEvent event;
  
  while (1) {
    XtAppNextEvent(app, &event);
    XtDispatchEvent(&event);
    if (event.type == ButtonRelease ||
	event.type == KeyRelease)
      break;
  }
}

void FileActionCB(Widget w, XtPointer app_data,
		  XmContainerSelectCallbackStruct *cscs)
{
  if (cscs->selected_item_count == 1) {
    int i;

    WaitRelease();
    SavePanes();
    XtVaGetValues(cscs->selected_items[0], XmNuserData, &i, NULL);
    cur_push(i, absolute, pushdir);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void FileSelectionCB(Widget w, XtPointer app_data,
		     XmContainerSelectCallbackStruct *cscs)
{
  int i, j;

  dirDeselect(curdir, ALL, NULL);
  for (j = 0; j < cscs->selected_item_count; j++) {
    XtVaGetValues(cscs->selected_items[j], XmNuserData, &i, NULL);
    dirSelect(curdir, i, NULL);
  }
  UpdateStatusLine();
  UpdateMenus();
}

void ShelfActionCB(Widget w, XtPointer app_data,
		   XmContainerSelectCallbackStruct *cscs)
{
  if (cscs->selected_item_count == 1) {
    int i;

    WaitRelease();
    SavePanes();
    XtVaGetValues(cscs->selected_items[0], XmNuserData, &i, NULL);
    shelf_push(i, absolute, pushdir);
    UpdatePanes(shelf_modified, root_modified, cur_modified,
		True, True, !cur_changed, cur_changed);
  }
}

void ShelfSelectionCB(Widget w, XtPointer app_data,
		      XmContainerSelectCallbackStruct *cscs)
{
  int i, j;

  dirDeselect(shelfdir, ALL, NULL);
  for (j = 0; j < cscs->selected_item_count; j++) {
    XtVaGetValues(cscs->selected_items[j], XmNuserData, &i, NULL);
    dirSelect(shelfdir, i, NULL);
  }
  UpdateStatusLine();
  UpdateMenus();
}

void TreeOutlineCB(Widget w, XtPointer app_data,
		   XmContainerOutlineCallbackStruct *cocs)
{
  DirPtr dir;

  XtVaGetValues(cocs->item, XmNuserData, &dir, NULL);

  if (dir)
    if (cocs->new_outline_state) {
      if (!dirHasSubdirInfo(dir)) {
	Wait();
	if (dirReadSubdirInfo(dir))
	  Expand(cocs->item);
	Done();
      }
    } else if (dirHasSubdirInfo(dir)) {
      Wait();
      SavePanes();
      dirFreeSubdirInfo(dir);
      cur_check();
      if (cur_changed)
	UpdatePanes(False, True, True, True, True, False, True);
      else
	Collapse(cocs->item);
      Done();
    }
}

void TreeSelectionCB(Widget w, XtPointer app_data,
		     XmContainerSelectCallbackStruct *cscs)
{
  Widget gadget = curgadget;
  DirPtr dir;

  if (cscs->selected_item_count == 1)
    gadget = cscs->selected_items[0];

  XtVaSetValues(gadget, XmNvisualEmphasis, XmSELECTED, NULL);
  XtVaGetValues(gadget, XmNuserData, &dir, NULL);
  if (dir)
    if (dir == curdir && dirIsReadable(curdir))
      Update();
    else {
      SavePanes();
      curgadget = gadget;
      cur_chdir(dirName(dir));
      UpdatePanes(shelf_modified, root_modified, cur_modified,
		  True, True, !cur_changed, cur_changed);
    }
}

/* ----------------------------------------------------------------------- */

void FileConvertCB(Widget w, XtPointer app_data,
		   XmConvertCallbackStruct *ccs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom STRING = XInternAtom(display, "STRING", False);
  Atom URILIST = XInternAtom(display, "text/uri-list", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);
  Atom TARGETS = XInternAtom(display, "TARGETS", False);
  Atom _MOTIF_EXPORT_TARGETS = XInternAtom(display, XmS_MOTIF_EXPORT_TARGETS,
					   False);

  if (ccs->target == _MOTIF_EXPORT_TARGETS ||
      ccs->target == TARGETS) {
    Atom *targets = (Atom*)MALLOC(4 * sizeof(Atom));

    targets[0] = FILES;
    targets[1] = STRING;
    targets[2] = URILIST;
    targets[3] = FILE_NAME;
    ccs->value = (XtPointer) targets;
    ccs->type = XA_ATOM;
    ccs->length = 4;
    ccs->format = 8 * sizeof(XtPointer);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == XPLORE_WIN) {
    Window *data = (Window*)MALLOC(sizeof(Window));
    
    data[0] = XtWindow(app_shell);
    ccs->value = (XtPointer) data;
    ccs->type = XA_WINDOW;
    ccs->length = 1;
    ccs->format = 8 * sizeof(Window);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == FILES) {
    SelPtr sel = selFromDir(curdir);

    if (sel) {
      String buf = selBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = FILES;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == STRING || ccs->target == URILIST) {
    SelPtr sel = selFromDir(curdir);

    if (sel) {
      String buf = uriBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = ccs->target;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == FILE_NAME) {
    SelPtr sel = selFromDir(curdir);

    if (sel && selNFiles(sel) == 1) {
      String buf = selName(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = FILE_NAME;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else
    ccs->status = XmCONVERT_REFUSE;
}

void ShelfConvertCB(Widget w, XtPointer app_data,
		    XmConvertCallbackStruct *ccs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom STRING = XInternAtom(display, "STRING", False);
  Atom URILIST = XInternAtom(display, "text/uri-list", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);
  Atom TARGETS = XInternAtom(display, "TARGETS", False);
  Atom _MOTIF_EXPORT_TARGETS = XInternAtom(display, XmS_MOTIF_EXPORT_TARGETS,
					   False);

  if (ccs->target == _MOTIF_EXPORT_TARGETS ||
      ccs->target == TARGETS) {
    Atom *targets = (Atom*)MALLOC(4 * sizeof(Atom));

    targets[0] = FILES;
    targets[1] = STRING;
    targets[2] = URILIST;
    targets[3] = FILE_NAME;
    ccs->value = (XtPointer) targets;
    ccs->type = XA_ATOM;
    ccs->length = 4;
    ccs->format = 8 * sizeof(XtPointer);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == XPLORE_WIN) {
    Window *data = (Window*)MALLOC(sizeof(Window));
    
    data[0] = XtWindow(app_shell);
    ccs->value = (XtPointer) data;
    ccs->type = XA_WINDOW;
    ccs->length = 1;
    ccs->format = 8 * sizeof(Window);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == FILES) {
    SelPtr sel = selFromDir(shelfdir);

    if (sel) {
      String buf = selBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = FILES;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == STRING || ccs->target == URILIST) {
    SelPtr sel = selFromDir(shelfdir);

    if (sel) {
      String buf = uriBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = ccs->target;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == FILE_NAME) {
    SelPtr sel = selFromDir(shelfdir);

    if (sel && selNFiles(sel) == 1) {
      String buf = selName(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = FILE_NAME;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else
    ccs->status = XmCONVERT_REFUSE;
}

void TreeConvertCB(Widget w, XtPointer app_data,
		   XmConvertCallbackStruct *ccs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom STRING = XInternAtom(display, "STRING", False);
  Atom URILIST = XInternAtom(display, "text/uri-list", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);
  Atom TARGETS = XInternAtom(display, "TARGETS", False);
  Atom _MOTIF_EXPORT_TARGETS = XInternAtom(display, XmS_MOTIF_EXPORT_TARGETS,
					   False);

  if (ccs->target == _MOTIF_EXPORT_TARGETS ||
      ccs->target == TARGETS) {
    Atom *targets = (Atom*)MALLOC(4 * sizeof(Atom));

    targets[0] = FILES;
    targets[1] = STRING;
    targets[2] = URILIST;
    targets[3] = FILE_NAME;
    ccs->value = (XtPointer) targets;
    ccs->type = XA_ATOM;
    ccs->length = 4;
    ccs->format = 8 * sizeof(XtPointer);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == XPLORE_WIN) {
    Window *data = (Window*)MALLOC(sizeof(Window));
    
    data[0] = XtWindow(app_shell);
    ccs->value = (XtPointer) data;
    ccs->type = XA_WINDOW;
    ccs->length = 1;
    ccs->format = 8 * sizeof(Window);
    ccs->status = XmCONVERT_DONE;
  } else if (ccs->target == FILES) {
    DirPtr dir = (DirPtr) ccs->location_data;
    SelPtr sel = selFromName(dirName(dir));

    if (sel) {
      String buf = selBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = FILES;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == STRING || ccs->target == URILIST) {
    DirPtr dir = (DirPtr) ccs->location_data;
    SelPtr sel = selFromName(dirName(dir));

    if (sel) {
      String buf = uriBuffer(sel);

      ccs->value = (XtPointer) buf;
      ccs->type = ccs->target;
      ccs->length = strlen(buf);
      ccs->format = 8;
      ccs->status = XmCONVERT_DONE;
      selFree(sel);
    } else
      ccs->status = XmCONVERT_REFUSE;
  } else if (ccs->target == FILE_NAME) {
    DirPtr dir = (DirPtr) ccs->location_data;
    String buf = NEWSTRING(dirName(dir));

    ccs->value = (XtPointer) buf;
    ccs->type = FILE_NAME;
    ccs->length = strlen(buf);
    ccs->format = 8;
    ccs->status = XmCONVERT_DONE;
  } else
    ccs->status = XmCONVERT_REFUSE;
}

typedef struct {
  int op;
  int item;
  FilePtr file;
  SelPtr sel;
  Boolean valid;
  Window win;
} FileTransferRec;

typedef struct {
  int op;
  DirPtr dir;
  SelPtr sel;
  Boolean valid;
  Window win;
} TreeTransferRec;

static void SendUpdateYourself(Window win)
{
  Atom _XPLORE_UPDATE = XInternAtom(display, "_XPLORE_UPDATE", False);
  Window data[1];

  data[0] = XtWindow(app_shell);
  XChangeProperty(display, win, _XPLORE_UPDATE, XA_WINDOW,
		  8*sizeof(Window), PropModeReplace, (XtPointer)data, 1);
}

static Boolean DoFileTransfer(FileTransferRec *t)
{
  if (!t || !t->valid) return True;
  SavePanes();
  cur_drop(t->item, t->op, t->sel, absolute, dirtarget);
  if (t->win && t->win != XtWindow(app_shell)) SendUpdateYourself(t->win);
  selFree(t->sel);
  FREE(t);
  lock = False;
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
  return True;
}

static Boolean DoShelfTransfer(FileTransferRec *t)
{
  if (!t || !t->valid) return True;
  SavePanes();
  shelf_drop(t->item, t->op, t->sel, absolute, dirtarget);
  if (t->win && t->win != XtWindow(app_shell)) SendUpdateYourself(t->win);
  selFree(t->sel);
  FREE(t);
  lock = False;
  UpdatePanes(shelf_modified, root_modified, cur_modified,
	      True, True, !cur_changed, cur_changed);
  return True;
}

#define fileMov(op) ((op==OP_COPY)?fileCopy:(op==OP_LINK)?fileLink:fileMove)

static Boolean DoTreeTransfer(TreeTransferRec *t)
{
  if (!t || !t->valid) return True;
  fileMov(t->op)(t->sel, dirName(t->dir), absolute);
  if (t->win && t->win != XtWindow(app_shell)) SendUpdateYourself(t->win);
  selFree(t->sel);
  FREE(t);
  lock = False;
  Update();
  return True;
}

static void FileTransferCB(Widget w, FileTransferRec *t,
			   XmSelectionCallbackStruct *scs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);

  if (scs->target == XPLORE_WIN && t && t->valid && t->op != NONE) {
    Window *buf = (Window*) scs->value;

    if (buf) {
      t->win = buf[0];
      FREE(buf);
    }
    lock = True;
    XtAppAddWorkProc(app, (XtWorkProc)DoFileTransfer, (XtPointer)t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_SUCCEED);
  } else if (scs->target == FILES && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromBuffer(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)FileTransferCB,
		    (XtPointer)t, CurrentTime);
  } else if (scs->target == FILE_NAME && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromName(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)FileTransferCB,
		    (XtPointer)t, CurrentTime);
  } else {
    FREE(scs->value);
    FREE(t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
  RestoreStatusLine();
  XmUpdateDisplay(w);
}

static void ShelfTransferCB(Widget w, FileTransferRec *t,
			   XmSelectionCallbackStruct *scs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);

  if (scs->target == XPLORE_WIN && t && t->valid && t->op != NONE) {
    Window *buf = (Window*) scs->value;

    if (buf) {
      t->win = buf[0];
      FREE(buf);
    }
    lock = True;
    XtAppAddWorkProc(app, (XtWorkProc)DoShelfTransfer, (XtPointer)t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_SUCCEED);
  } else if (scs->target == FILES && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromBuffer(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)ShelfTransferCB,
		    (XtPointer)t, CurrentTime);
  } else if (scs->target == FILE_NAME && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromName(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)ShelfTransferCB,
		    (XtPointer)t, CurrentTime);
  } else {
    FREE(scs->value);
    FREE(t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
  RestoreStatusLine();
  XmUpdateDisplay(w);
}

static void TreeTransferCB(Widget w, TreeTransferRec *t,
			   XmSelectionCallbackStruct *scs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  Atom XPLORE_WIN = XInternAtom(display, "XPLORE_WIN", False);

  if (scs->target == XPLORE_WIN && t && t->valid && t->op != NONE) {
    Window *buf = (Window*) scs->value;

    if (buf) {
      t->win = buf[0];
      FREE(buf);
    }
    lock = True;
    XtAppAddWorkProc(app, (XtWorkProc)DoTreeTransfer, (XtPointer)t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_SUCCEED);
  } else if (scs->target == FILES && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromBuffer(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)TreeTransferCB,
		    (XtPointer)t, CurrentTime);
  } else if (scs->target == FILE_NAME && t && t->valid && t->op != NONE) {
    String buf = (String) scs->value;

    t->sel = selFromName(buf);
    FREE(buf);
    XmTransferValue(scs->transfer_id, XPLORE_WIN,
		    (XtCallbackProc)TreeTransferCB,
		    (XtPointer)t, CurrentTime);
  } else {
    FREE(scs->value);
    FREE(t);
    XmTransferDone(scs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
  RestoreStatusLine();
  XmUpdateDisplay(w);
}

void FileDestinationCB(Widget w, XtPointer app_data,
		       XmDestinationCallbackStruct *dcs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  XmDropProcCallbackStruct *ds = dcs->destination_data;
  Widget target = XmObjectAtPoint(w, ds->x, ds->y);
  FileTransferRec *t = (FileTransferRec*) MALLOC(sizeof(FileTransferRec));
  Atom *exportTargets;
  Cardinal numExportTargets;
  int i;

  switch (dcs->operation) {
  case XmMOVE:
    t->op = OP_MOVE; break;
  case XmCOPY:
    t->op = OP_COPY; break;
  case XmLINK:
    t->op = OP_LINK; break;
  default:
    t->op = NONE; break;
  }
  t->sel = NULL;
  t->valid = True;
  t->file = NULL;
  t->win = 0;
  if (target) {
    XtVaGetValues(target, XmNuserData, &t->item, NULL);
    if (!cur_is_drop_target(t->item))
      if (dcs->flags != XmCONVERTING_SAME)
	t->item = NONE;
      else
	t->valid = False;
    else if (dcs->flags == XmCONVERTING_SAME &&
	     fileSelected(dirFile(curdir, t->item)))
      t->valid = False;
    else
      t->file = dirFile(curdir, t->item);
  } else if (dcs->flags != XmCONVERTING_SAME)
    t->item = NONE;
  else
    t->valid = False;
  XtVaGetValues(ds->dragContext, XmNexportTargets, &exportTargets,
		XmNnumExportTargets, &numExportTargets, NULL);
  for (i = 0; i < numExportTargets; i++)
    if (exportTargets[i] == FILES || exportTargets[i] == FILE_NAME)
      break;
  if (i < numExportTargets)
    XmTransferValue(dcs->transfer_id, exportTargets[i],
		    (XtCallbackProc)FileTransferCB,
		    (XtPointer)t, CurrentTime);
  else {
    FREE(t);
    XmTransferDone(dcs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
}

void ShelfDestinationCB(Widget w, XtPointer app_data,
		       XmDestinationCallbackStruct *dcs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  XmDropProcCallbackStruct *ds = dcs->destination_data;
  Widget target = XmObjectAtPoint(w, ds->x, ds->y);
  FileTransferRec *t = (FileTransferRec*) MALLOC(sizeof(FileTransferRec));
  Atom *exportTargets;
  Cardinal numExportTargets;
  int i;

  switch (dcs->operation) {
  case XmMOVE:
    t->op = OP_MOVE; break;
  case XmCOPY:
    t->op = OP_COPY; break;
  case XmLINK:
    t->op = OP_LINK; break;
  default:
    t->op = NONE; break;
  }
  t->sel = NULL;
  t->valid = True;
  t->file = NULL;
  t->win = 0;
  if (target) {
    XtVaGetValues(target, XmNuserData, &t->item, NULL);
    if (!shelf_is_drop_target(t->item))
      if (dcs->flags != XmCONVERTING_SAME)
	t->item = NONE;
      else
	t->valid = False;
    else if (dcs->flags == XmCONVERTING_SAME &&
	     fileSelected(dirFile(shelfdir, t->item)))
      t->valid = False;
    else
      t->file = dirFile(shelfdir, t->item);
  } else if (dcs->flags != XmCONVERTING_SAME)
    t->item = NONE;
  else
    t->valid = False;
  XtVaGetValues(ds->dragContext, XmNexportTargets, &exportTargets,
		XmNnumExportTargets, &numExportTargets, NULL);
  for (i = 0; i < numExportTargets; i++)
    if (exportTargets[i] == FILES || exportTargets[i] == FILE_NAME)
      break;
  if (i < numExportTargets)
    XmTransferValue(dcs->transfer_id, exportTargets[i],
		    (XtCallbackProc)ShelfTransferCB,
		    (XtPointer)t, CurrentTime);
  else {
    FREE(t);
    XmTransferDone(dcs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
}

void TreeDestinationCB(Widget w, XtPointer app_data,
		       XmDestinationCallbackStruct *dcs)
{
  Atom FILE_NAME = XInternAtom(display, "FILE_NAME", False);
  Atom FILES = XInternAtom(display, "FILES", False);
  XmDropProcCallbackStruct *ds = dcs->destination_data;
  Widget target = XmObjectAtPoint(w, ds->x, ds->y);
  TreeTransferRec *t = (TreeTransferRec*) MALLOC(sizeof(TreeTransferRec));
  Atom *exportTargets;
  Cardinal numExportTargets;
  int i;

  switch (dcs->operation) {
  case XmMOVE:
    t->op = OP_MOVE; break;
  case XmCOPY:
    t->op = OP_COPY; break;
  case XmLINK:
    t->op = OP_LINK; break;
  default:
    t->op = NONE; break;
  }
  t->sel = NULL;
  t->valid = True;
  t->win = 0;
  if (target) {
    XtVaGetValues(target, XmNuserData, &t->dir, NULL);
    if (dcs->flags == XmCONVERTING_SAME && t->dir == tree_drag_dir)
      t->valid = False;
  } else
    t->valid = False;
  XtVaGetValues(ds->dragContext, XmNexportTargets, &exportTargets,
		XmNnumExportTargets, &numExportTargets, NULL);
  for (i = 0; i < numExportTargets; i++)
    if (exportTargets[i] == FILES || exportTargets[i] == FILE_NAME)
      break;
  if (i < numExportTargets)
    XmTransferValue(dcs->transfer_id, exportTargets[i],
		    (XtCallbackProc)TreeTransferCB,
		    (XtPointer)t, CurrentTime);
  else {
    FREE(t);
    XmTransferDone(dcs->transfer_id, XmTRANSFER_DONE_FAIL);
  }
}

void DragProcCB(Widget w, XtPointer app_data,
		XmDragProcCallbackStruct *dpcs)
{
  static unsigned char saveStatus = XmDROP_SITE_VALID;
  static unsigned char prevStatus = XmDROP_SITE_VALID;
  static DirPtr prev_dir = NULL;
  static int prev_item = NONE;
  Widget gadget = XmObjectAtPoint(w, dpcs->x, dpcs->y);
  Boolean update = False;
  DirPtr dir = NULL;
  int item = NONE;

  switch (dpcs->reason) {
  case XmCR_DROP_SITE_LEAVE_MESSAGE:
    RestoreStatusLine();
    break;
  case XmCR_DROP_SITE_ENTER_MESSAGE:
    saveStatus = dpcs->dropSiteStatus = XmDROP_SITE_VALID;
    update = True;
  case XmCR_DROP_SITE_MOTION_MESSAGE:
    if (saveStatus == XmDROP_SITE_VALID) {
      if (w == tree_scroll) {
	if (gadget)
	  XtVaGetValues(gadget, XmNuserData, &dir, NULL);
	if (dir)
	  dpcs->dropSiteStatus = XmDROP_SITE_VALID;
	else
	  dpcs->dropSiteStatus = XmDROP_SITE_INVALID;
      } else if (w == file_scroll || w == shelf_scroll) {
	dir = (w == file_scroll)?curdir:shelfdir;
	if (gadget)
	  XtVaGetValues(gadget, XmNuserData, &item, NULL);
	if (item != NONE && !(((dir==curdir)?cur_is_drop_target:
			       shelf_is_drop_target)(item)))
	  item = NONE;
	dpcs->dropSiteStatus = XmDROP_SITE_VALID;
      } else
	dpcs->dropSiteStatus = XmDROP_SITE_INVALID;
    }
    update = update || dpcs->dropSiteStatus != prevStatus ||
      dpcs->dropSiteStatus == XmDROP_SITE_VALID &&
      (dir != prev_dir || item != prev_item);
    if (update) {
      if (dpcs->dropSiteStatus == XmDROP_SITE_VALID) {
	String name, desc;
	if (item != NONE) {
	  name = fileName(dirFile(dir, item));
	  desc = comment(fileType(dirFile(dir, item)));
	} else {
	  name = dirName(dir);
	  desc = comment(dirType(dir));
	}
	if (desc) {
	  int l = strlen(name) + strlen(desc) + 11;
	  String msg = alloca((l+1) * sizeof(char));
	  sprintf(msg, "Target: %s (%s)", name, desc);
	  StatusMessage(msg);
	} else {
	  int l = strlen(name) + 8;
	  String msg = alloca((l+1) * sizeof(char));
	  sprintf(msg, "Target: %s", name);
	  StatusMessage(msg);
	}
	prev_dir = dir;
	prev_item = item;
      } else
	RestoreStatusLine();
      prevStatus = dpcs->dropSiteStatus;
    }
    break;
  }
}

void DropFinishCB(Widget w, XtPointer app_data,
		  XmDropFinishCallbackStruct *dfcs)
{
  lock = False;
}

/* ----------------------------------------------------------------------- */

void InputCB(XtPointer data, int *fd, XtInputId *id)
{
  char buf[4096];
  int count;
  while ((count = read(*fd, buf, sizeof(buf)-1)) > 0) {
    buf[count] = 0;
    XmTextInsert(log_scroll, XmTextGetLastPosition(log_scroll), buf);
  }
  if (savelines > 0) {
    char *s = buf;
    while ((s = strchr(s, '\n'))) lines++, s++;
    if (lines > savelines) {
      char *log = XmTextGetString(log_scroll);
      s = log;
      while (lines > savelines && (s = strchr(s, '\n'))) s++, lines--;
      XmTextSetString(log_scroll, s);
      FREE(log);
    }
  }
  XmTextSetInsertionPosition(log_scroll, XmTextGetLastPosition(log_scroll));
  XmTextShowPosition(log_scroll, XmTextGetLastPosition(log_scroll));
  if (autopopup) ShowLogPane();
}

/* ----------------------------------------------------------------------- */

Boolean lock = False;

void TimerCB(XtPointer data, XtIntervalId *id)
{
  if (lock)
    timer_id = XtAppAddTimeOut(app, update_time, TimerCB, NULL);
  else
    Update();
}

/* ----------------------------------------------------------------------- */

void UpdateH(Widget w, XtPointer client_data, XEvent *event)
{
  Atom _XPLORE_UPDATE = XInternAtom(display, "_XPLORE_UPDATE", False);
  XPropertyEvent *prop_event = (XPropertyEvent *) event;
  if (prop_event->atom == _XPLORE_UPDATE &&
      prop_event->state == PropertyNewValue) {
    XDeleteProperty(display, XtWindow(app_shell), _XPLORE_UPDATE);
    Update();
  }
}

void DeleteWindowCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  quit();
}

void SaveYourselfCB(Widget w, XtPointer app_data, XtPointer widget_data)
{
  if (autosession) {
    save_in_progress = True;
    SaveSession(True);
    UpdateMenus();
  } else {
    char **argv;
    int argc;
    /* do a trivial update to show that we're done saving */
    XGetCommand(display, XtWindow(app_shell), &argv, &argc);
    XSetCommand(display, XtWindow(app_shell), argv, argc);
    FREE(argv);
  }
}
