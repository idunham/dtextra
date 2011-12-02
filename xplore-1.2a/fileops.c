
#include "xplore.h"
#include "fileops.h"

#include "devmount.h"
#include "dialogs.h"
#include "error.h"
#include "ftype.h"
#include "signals.h"
#include "util.h"

int user_umask;
String metachars = NULL;

#define MOVE_MSG "Moving..."
#define COPY_MSG "Copying..."
#define LINK_MSG "Linking..."
#define DELETE_MSG "Deleting..."

#define COPY_MSG2 "Copying %s"
#define DELETE_MSG2 "Deleting %s"

/* private functions */

/* create a new file */

static int mkfile(String name, mode_t mode)
{
    int desc = open(name, O_CREAT|O_WRONLY|O_EXCL, mode);
    if (desc == -1)
	return -1;
    else {
	close(desc);
	return 0;
    }
}

/* pathnames for diagnostics in file operations */

static char err_source[MAXPATHLEN+1], err_target[MAXPATHLEN+1],
  err_path[MAXPATHLEN+1];

/* create a symbolic link */

static int slink(const char *oldpath, const char *newpath)
{
  strcpy(err_source, oldpath);
  strcpy(err_target, newpath);
  if (unlink(newpath) && errno != ENOENT)
    return -1;
  else
    return symlink(oldpath, newpath);
}

/* copy files recursively */

/* bug fixed by Walt Killjoy (ngogn@clark.net) */

static ino_t *inodes;
static int copyfile(const char *oldpath, const char *newpath);
static int copydir(int n_inodes, struct stat *oldstats,
		   const char *oldpath, const char *newpath);
static int copy(int n_inodes, const char *oldpath, const char *newpath);

static int rcopy(const char *oldpath, const char *newpath)
{
  inodes = NULL;
  return copy(0, oldpath, newpath);
}

static int copyfile(const char *oldpath, const char *newpath)
{
  struct stat stats;
  int src = -1, dest = -1, n, errno_ret;
  char buf[BUFSIZ];

  if ((src = open(oldpath, O_RDONLY)) == -1 || stat(oldpath, &stats))
    goto err;
  else if ((dest = creat(newpath, stats.st_mode)) == -1)
    goto err;

  while ( (n = read(src, buf, BUFSIZ)) != 0)
    if ( n == -1 || write(dest, buf, n) != n )
      goto err;

  if (close(src)) {
    src = -1;
    goto err;
  } else
    return close(dest);

 err:

  errno_ret = errno;
  if (src != -1) close(src);
  if (dest != -1) close(dest);
  errno = errno_ret;
  return -1;
}

static int copydir(int n_inodes, struct stat *oldstats,
		   const char *oldpath, const char *newpath)
{
  DIR *dir;
  struct dirent *entry;
  int i, ol = strlen(oldpath), nl = strlen(newpath);
  struct stat newstats;

  for (i = n_inodes-1; i >= 0; i--)
    if (inodes[i] == oldstats->st_ino)
      return 0;

  if ((mkdir(newpath, user_umask & 0777) < 0 && errno != EEXIST) ||
       lstat(newpath, &newstats) ||
       !(dir = opendir(oldpath)))
    return -1;

  inodes = (ino_t *) REALLOC(inodes, (n_inodes+1)*sizeof(ino_t));
  inodes[n_inodes++] = newstats.st_ino;

  for(i = 0; (entry = readdir(dir)); i++)
    if (entry->d_name[0] != '.' || (entry->d_name[1] != '\0'
				    && (entry->d_name[1] != '.' ||
					entry->d_name[2] != '\0'))) {
      int ol1 = ol, nl1 = nl, l = strlen(entry->d_name), res;
      char *oldpath1 = (char *)alloca(ol1+l+2);
      char *newpath1 = (char *)alloca(nl1+l+2);

      strcpy(oldpath1, oldpath);
      strcpy(newpath1, newpath);
      if (oldpath1[ol1-1] != '/')
	oldpath1[ol1++] = '/';
      if (newpath1[nl1-1] != '/')
	newpath1[nl1++] = '/';
      strcpy(oldpath1+ol1, entry->d_name);
      strcpy(newpath1+nl1, entry->d_name);
      if ((res = copy(n_inodes, oldpath1, newpath1))) {
	int errno_ret = errno;

	closedir(dir);
	errno = errno_ret;
	return res;
      }
    }

  if (n_inodes > 1)
    inodes = (ino_t *) REALLOC(inodes, (n_inodes-1)*sizeof(ino_t));
  else
    FREE(inodes);
  return closedir(dir);
}

static int copy(int n_inodes, const char *oldpath, const char *newpath)
{
  struct stat stats, stats2;

  strcpy(err_source, oldpath);
  strcpy(err_target, newpath);
  if (lstat(oldpath, &stats))
    return -1;
  else if (!lstat(newpath, &stats2) && !S_ISDIR(stats2.st_mode))
    unlink(newpath);

  /* Directory: copy recursively */
  if (S_ISDIR(stats.st_mode))
    return copydir(n_inodes, &stats, oldpath, newpath);

  if (CheckBreak()) return -2;
  WaitMsg2(COPY_MSG2, (String)oldpath);

  /* Regular file: copy block by block */
  if (S_ISREG(stats.st_mode))
    return copyfile(oldpath, newpath);

  /* Fifo: make a new one */
  else if (S_ISFIFO(stats.st_mode))
    return mkfifo(newpath, user_umask & 0666);

  /* Device: make a new one */
  else if (S_ISBLK(stats.st_mode) || S_ISCHR(stats.st_mode) ||
	     S_ISSOCK(stats.st_mode))
    return mknod(newpath, user_umask & 0666, stats.st_rdev);

  /* Symbolic link: make a new one */
  else if (S_ISLNK(stats.st_mode)) {
    static char lnk[MAXPATHLEN+1];
    int l = readlink(oldpath, lnk, MAXPATHLEN);

    if (l<0)
      return -1;
    lnk[l] = '\0';
    return(symlink(lnk, newpath));
  }

  /* This shouldn't happen */
  else {
    xplerr1(ERR_INVALID, (String) oldpath);
    return 0;
  }
}

/* move files (by copy/del across file systems) */

static int rdel(const char *path);

static int rmove(const char *oldpath, const char *newpath)
{
  StatRec stats;

  strcpy(err_source, oldpath);
  strcpy(err_target, newpath);
  if (unlink(newpath) && errno != ENOENT)
    return -1;
  if (rename(oldpath, newpath))
    if (errno == EXDEV)
      if (rcopy(oldpath, newpath))
	return -1;
      else if (stat(oldpath, &stats))
	return -1;
      else if (S_ISDIR(stats.st_mode))
	return rdel(oldpath);
      else
	return unlink(oldpath);
    else
      return -1;
  else
    return 0;
}

/* delete files recursively */

static int rdel(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    struct stat stats;
    int i, pl = strlen(path);
  
    strcpy(err_path, path);
    if (lstat(path, &stats))
	return -1;
    else if (!S_ISDIR(stats.st_mode)) {
        if (CheckBreak()) return -2;
        WaitMsg2(DELETE_MSG2, (String)path);
	return(unlink(path));
    } else if (!(dir = opendir(path)))
	return -1;

    for (i=0; (entry = readdir(dir)); i++) {
	if (entry->d_name[0] != '.' ||
	    (entry->d_name[1] != '\0' &&
	     (entry->d_name[1] != '.' || entry->d_name[2] != '\0'))) {
	    int pl1 = pl, l = strlen(entry->d_name), res;
	    char *path1 = (char *)alloca(pl1+l+2);

	    strcpy(path1, path);
	    if (path1[pl1-1] != '/')  path1[pl1++] = '/';
	    strcpy(path1+pl1, entry->d_name);
	    if ((res = rdel(path1))) {
	      int errno_ret = errno;

	      closedir(dir);
	      errno = errno_ret;
	      return res;
	    }
	}
    }
    WaitMsg2(DELETE_MSG2, (String)path);
    if (closedir(dir) || rmdir(path))
	return -1;
    else
	return 0;
}

/* generic file move/copy/link operation */

Boolean backups = False;

typedef int (*MovProc)(const char *oldname, const char *newname);

#define confirm_overwrite_dlg (backups? \
 (S_ISDIR(stats.st_mode)?confirm_rename_dir_dlg: \
  confirm_rename_file_dlg): \
 (S_ISDIR(stats.st_mode)?confirm_overwrite_dir_dlg: \
  confirm_overwrite_file_dlg))

static Boolean fileMovToDir(SelPtr sel, String target, MovProc mov, int err,
			    Boolean absolute)
{
  int i;

  if (((mov == rmove)?confirm_move_dlg:
       (mov == rcopy)?confirm_copy_dlg:
       confirm_link_dlg)
      (NULL, selNFiles(sel), selDirname(sel), target) != OK)
    return False;
  WaitMsg((mov == rmove)?MOVE_MSG:
	  (mov == rcopy)?COPY_MSG:
	  LINK_MSG);
  for (i = 0; i < selNFiles(sel); i++) {
    static char path0[MAXPATHLEN+1], path1[MAXPATHLEN+1],
      path2[MAXPATHLEN+1], bak[MAXPATHLEN+20];
    StatRec stats;
    int res;

    if (!strcmp(selFilename(sel, i), "..")) {
      Done();
      xplerr1(ERR_DOTDOT, (mov == rmove)?"move":
	      (mov == rcopy)?"copy":"link");
      return False;
    }
    pathname(path0, selDirname(sel), selFilename(sel, i));
    if (absolute || mov != slink)
      strcpy(path1, path0);
    else
      relpath(path1, target, path0);
    pathname(path2, target, selFilename(sel, i));
    if (identical(path0, path2)) {
      Done();
      force_overwrt = False;
      xplerr2(ERR_IDENTICAL, path0, path2);
      return False;
    } else if (!lstat(path2, &stats)) {
      switch ((res = confirm_overwrite_dlg(NULL, path2))) {
      case YES:
	break;
      case NO:
	if (backups)
	  break;
	else
	  continue;
      case DOALL:
	force_overwrt = True;
	break;
      case CANCEL:
	Done();
	force_overwrt = False;
	return False;
      }
      if (res != NO && backups && rename(path2, bakname(bak, path2))) {
	Done();
	force_overwrt = False;
	syserr2(ERR_RENAME, path2, bak);
	return False;
      }
    }
    if ((res = mov(path1, path2))) {
      Done();
      force_overwrt = False;
      if (res == -2)
	xplerr(ERR_ABORT);
      else
	syserr2(err, err_source, err_target);
      return False;
    }
  }
  Done();
  force_overwrt = False;
  return True;
}

static Boolean fileMovToFile(SelPtr sel, String target, MovProc mov, int err,
			     Boolean absolute)
{
  char path0[MAXPATHLEN+1], path[MAXPATHLEN+1], dirname[MAXPATHLEN+1];
  char bak[MAXPATHLEN+20];
  StatRec stats;
  int res;

  if (!strcmp(selFilename(sel, 0), "..")) {
    Done();
    xplerr1(ERR_DOTDOT, (mov == rmove)?"move":
	    (mov == rcopy)?"copy":"link");
    return False;
  }
  pathname(path0, selDirname(sel), selFilename(sel, 0));
  if (absolute || mov != slink)
    strcpy(path, path0);
  else
    relpath(path, dirpart(dirname, target), path0);
  if (identical(path0, target)) {
    xplerr2(ERR_IDENTICAL, path0, target);
    return False;
  } else if (!lstat(target, &stats))
    if ((res = confirm_overwrite_dlg(NULL, target)) == NO &&
	!backups || res == CANCEL)
      return False;
    else if (res != NO && backups && rename(target, bakname(bak, target))) {
      Done();
      syserr2(ERR_RENAME, target, bak);
      return False;
    }
  WaitMsg((mov == rmove)?MOVE_MSG:
	  (mov == rcopy)?COPY_MSG:
	  LINK_MSG);
  if ((res = mov(path, target))) {
    Done();
    if (res == -2)
      xplerr(ERR_ABORT);
    else
      syserr2(err, err_source, err_target);
    return False;
  } else {
    Done();
    return True;
  }
}

static Boolean fileMov(SelPtr sel, String target, MovProc mov, int err,
		       Boolean absolute)
{
  if (!sel || !target || !*target)
    return False;
  else {
    int d, res;
    StatRec stats;
    char path[MAXPATHLEN+1], abstarget[MAXPATHLEN+1];
    
    abspath(abstarget, selDirname(sel), target);
    if (!selNFiles(sel))
      return True;
    else if ((d = mount_dlg(NULL, abstarget, True)) == NONE) {
      xplerr1(ERR_MOUNT, abstarget);
      return False;
    } else if (selNFiles(sel) > 1) {
      if ((res = stat(abstarget, &stats)) || !S_ISDIR(stats.st_mode)) {
	if (res)
	  syserr1(ERR_TARGET_NOT_DIR, abstarget);
	else
	  xplerr1(ERR_TARGET_NOT_DIR, abstarget);
	umount_dlg(NULL, d);
	return False;
      } else {
	res = fileMovToDir(sel, abstarget, mov, err, absolute);
	umount_dlg(NULL, d);
	return res;
      }
    } else if (stat(abstarget, &stats) || !S_ISDIR(stats.st_mode)) {
      res = fileMovToFile(sel, abstarget, mov, err, absolute);
      umount_dlg(NULL, d);
      return res;
    } else {
      res = fileMovToDir(sel, abstarget, mov, err, absolute);
      umount_dlg(NULL, d);
      return res;
    }
  }
}

/* shell-related stuff (thanks to Robert Vogelgesang (vogelges@rhrk.uni-kl.de)
   for the shell detection code he wrote for xfm) */

static char shell_command[MAXPATHLEN+1];
static Boolean shell_needs_arg0;

static int shell_test()
{
    int pipe_fd[2];
    int p;
    char val[3];

    if (pipe(pipe_fd) < 0)
	fatal("Can't create pipe");

    p = fork();
    if (p < 0)
	fatal("Can't fork");

    if (!p) {       /* child; exec the shell w/ test args */
	dup2(pipe_fd[1], fileno(stdout));
	if (close(pipe_fd[0]) == -1) {
	    error("shell test (child): can't close pipe");
	    exit(1);
	}
	execlp(shell_command, shell_command, "-c", "echo $*", "1", NULL);
	perror("shell test: exec failed");
	exit(1);
    } else {        /* parent; read and check the child's output */
	if (close(pipe_fd[1]) == -1)
	    fatal("shell test: (parent) Can't close pipe");
	val[0] = '\0';
	while ((p = read(pipe_fd[0], val, 3)) < 0) {
	    if (errno != EINTR)
		fatal("shell test: reading child's output failed");
	}
	if (p == 3)
	    return -1;
	shell_needs_arg0 = (val[0] != '1');
	return 0;
    }
}

static void shell_init(char *sh_list, char *shell)
{
    if (!shell || !*shell)
        shell = getenv("SHELL");
    if (!shell || !*shell)
        shell = "/bin/sh";
    strcpy(shell_command, shell);
    if (sh_list == NULL || !strcmp(sh_list, "AUTO")) {
	if (shell_test() == -1)
	    fatal("Shell test failed. See the manual page for help.\n");
    } else
	shell_needs_arg0 = fnxmatch(sh_list, shell_command);
}

static int nargs(String *args)
{
    if (!args)
	return 0;
    else {
	int n;
	for (n = 0; args[n]; n++) ;
	return n;
    }
}

static String *create_argv(String command, String *args)
{
    int i, n = nargs(args)+1, k = shell_needs_arg0?4:3;
    args = (String*) REALLOC(args, (n+k)*sizeof(String));
    for (i = n - 1; i >= 0; i--)
      args[i+k] = args[i];
    args[0] = shell_command;
    args[1] = "-c";
    args[2] = command;
    if (shell_needs_arg0) args[3] = shell_command;
    args[n+k-1] = NULL;
    return args;
}

static void free_argv(String *args)
{
    int i;
    for (i = (shell_needs_arg0?4:3); args[i]; i++)
	FREE(args[i]);
    FREE(args);
}

static Boolean needs_quote(String s)
{
  if (metachars && s) {
    for (; *s; s++)
      if (strchr(metachars, *s))
	return True;
    return False;
  } else
    return False;
}

static String echo_arg(String arg)
{
  if (needs_quote(arg)) {
    String buf = alloca(2 * strlen(arg) + 1);
    
    printf(" '%s'", quote(buf, arg, "\\'"));
  } else
    printf(" %s", arg);
}

/* public functions */

void shellTest(String sh_list, String shell)
{
    shell_init(sh_list, shell);
}

Boolean fileNew(DirPtr dir, String name)
{
    char path[MAXPATHLEN+1];
    if (!dir)
	return False;
    else if (mkfile(pathname(path, dirName(dir), name),
		    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) {
	syserr1(ERR_CREATE_FILE, path);
	return False;
    } else
	return True;
}

Boolean dirNew(DirPtr dir, String name)
{
    char path[MAXPATHLEN+1];
    if (!dir)
	return False;
    else if (mkdir((*name != '/')?pathname(path, dirName(dir), name):name,
		   S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |
		   S_IXOTH)) {
	syserr1(ERR_CREATE_DIR, path);
	return False;
    } else
	return True;
}

Boolean fileMove(SelPtr sel, String target, Boolean absolute)
{
    return fileMov(sel, target, rmove, ERR_MOVE, absolute);
}

Boolean fileCopy(SelPtr sel, String target, Boolean absolute)
{
    return fileMov(sel, target, rcopy, ERR_COPY, absolute);
}

Boolean fileLink(SelPtr sel, String target, Boolean absolute)
{
    return fileMov(sel, target, slink, ERR_LINK, absolute);
}

Boolean fileDelete(SelPtr sel)
{
  int i;
  static char path[MAXPATHLEN+1];
  Boolean is_dir;
  int res;

  if (confirm_delete_dlg(NULL, selNFiles(sel), selDirname(sel)) != OK)
    return False;
  WaitMsg(DELETE_MSG);
  for (i = 0; i < selNFiles(sel); i++) {
    StatRec stats;

    if (!strcmp(selFilename(sel, i), "..")) {
      Done();
      xplerr1(ERR_DOTDOT, "delete");
      return False;
    }
    pathname(path, selDirname(sel), selFilename(sel, i));
    switch (((is_dir = (!lstat(path, &stats) && S_ISDIR(stats.st_mode)))?
	      confirm_delete_dir_dlg:confirm_delete_file_dlg)
	    (NULL, path)) {
    case YES:
      break;
    case NO:
      continue;
    case DOALL:
      if (is_dir)
	force_deldir = True;
      else
	force_delfile = True;
      break;
    case CANCEL:
      Done();
      force_deldir = force_delfile = False;
      return False;
    }
    if ((res = rdel(path))) {
      Done();
      force_deldir = force_delfile = False;
      if (res == -2)
	xplerr(ERR_ABORT);
      else
	syserr1(ERR_DELETE, err_path);
      return False;
    }
  }
  Done();
  force_deldir = force_delfile = False;
  return True;
}

String *makeArgv(String dirname, String firstarg, SelPtr sel,
		 Boolean absolute)
{
    int n;

    if (!dirname || !*dirname)
	dirname = basedir;
    n = selNFiles(sel)+((firstarg && *firstarg)?1:0);
    if (n) {
	String *argv = (String *) MALLOC((n+1)*sizeof(String));
	int i, argc = 0;
	char path[MAXPATHLEN+1], rpath[MAXPATHLEN+1];

	if (firstarg && *firstarg)
	    if (absolute)
		argv[argc++] = NEWSTRING(firstarg);
	    else {
		relpath(rpath, dirname, firstarg);
		argv[argc++] = NEWSTRING(rpath);
	    }
	for (i = 0; i < selNFiles(sel); i++)
	    if (absolute) {
		pathname(path, selDirname(sel), selFilename(sel, i));
		argv[argc++] = NEWSTRING(path);
	    } else {
		relpath(rpath, dirname, pathname(path, selDirname(sel),
						 selFilename(sel, i)));
		argv[argc++] = NEWSTRING(rpath);
	    }
	argv[argc] = NULL;
	return argv;
    } else
	return NULL;
}

Boolean echo;

Boolean fileExec(String command, String dirname, String *args)
{
  int pid, status, res;
  char absdir[MAXPATHLEN+1];

  if (!*shell_command)
    return False;
  else if (!command || !*command)
    return True;
  else if (!dirname || !*dirname)
    strcpy(absdir, basedir);
  else
    abspath(absdir, basedir, dirname);
  if (!*absdir)
    return False;
  args = create_argv(command, args);
  Wait();
  if (echo) {
    String *argp;
    printf("[%s]", absdir);
    for (argp = args; *argp; argp++)
      echo_arg(*argp);
    printf("\n");
    fflush(stdout);
  }
  if (chdir(absdir)) {
    Done();
    free_argv(args);
    syserr1(ERR_CHDIR, absdir);
    return False;
  } else if ((pid = fork()) == -1) {
    Done();
    free_argv(args);
    syserr2(ERR_EXEC, command, absdir);
    return False;
  } else if (!pid) {
    /* redirect stdin to /dev/null to prevent the started program from
       blocking us with terminal input: */
    freopen("/dev/null", "r", stdin);
    execvp(shell_command, args);
    error("exec failed");
    exit(1);
  } else {
    free_argv(args);
    res = waitpid(pid, &status, 0);
    Done();
    if (res == -1 || !WIFEXITED(status) || WEXITSTATUS(status))
      return False;
    else
      return True;
  }
}

#define MAX_BUF 10000

Boolean filePush(DirPtr dir, int i, String pushdir, Boolean absolute,
		 Boolean res_link)
{
  if (dir) {
    FilePtr file = dirFile(dir, i);
    int type = fileType(file);
    String msg = comment(type);
    String action = push_action(type);
    String dirname = push_dirname(type);
    char path[MAXPATHLEN+1], realpath[MAXPATHLEN+1];
    char realdir[MAXPATHLEN+1], realname[MAXPATHLEN+1];

    if (!msg) msg = "";
    pathname(path, dirName(dir), fileName(file));
    if (res_link && S_ISLNK(fileLstats(file)->st_mode)) {
      resolve(realpath, path);
      dirpart(realdir, realpath);
      filepart(realname, realpath);
    } else {
      strcpy(realpath, path);
      strcpy(realdir, dirName(dir));
      strcpy(realname, fileName(file));
    }

    if (!dirname || !*dirname)
      if (pushdir && *pushdir)
	dirname = pushdir;
      else
	dirname = realdir;
    if (action && *action) {
      char buf[MAX_BUF+1];
      
      if (param_dlg(fileLargeIcon(file), msg, action, buf, MAX_BUF)
	  == OK)
	return fileExec(buf, dirname,
			makeArgv(dirname, realpath,
				 NULL, absolute));
      else
	return True;
    } else if (fileStats(file)->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
      action = alloca(strlen(realpath)+3);
      strcpy(action, realpath);
      strcat(action, " &");
      return fileExec(action, dirname, NULL);
    } else
      return True;
  } else
    return False;
}

Boolean fileMenu(DirPtr dir, int i, int j, Boolean absolute, Boolean res_link)
{
  if (dir) {
    FilePtr file = dirFile(dir, i);
    int type = fileType(file);
    String action;
    String dirname;
    char path[MAXPATHLEN+1], realpath[MAXPATHLEN+1];
    char realdir[MAXPATHLEN+1], realname[MAXPATHLEN+1];
    char buf[MAX_BUF+1];
    
    if (j < 0 || j > n_menu_actions(type))
      return False;
    else if (!(action = menu_action(type, j)) || !*action)
      return True;
    
    pathname(path, dirName(dir), fileName(file));
    if (res_link && S_ISLNK(fileLstats(file)->st_mode)) {
      resolve(realpath, path);
      dirpart(realdir, realpath);
      filepart(realname, realpath);
    } else {
      strcpy(realpath, path);
      strcpy(realdir, dirName(dir));
      strcpy(realname, fileName(file));
    }

    if (!(dirname = menu_dirname(type, j)) || !*dirname)
      dirname = realdir;
    if (param_dlg(fileLargeIcon(file), menu_label(type, j), action, buf,
		  MAX_BUF) == OK)
      return fileExec(buf, dirname,
		      makeArgv(dirname, realpath,
			       NULL, absolute));
    else
      return True;
  } else
    return False;
}

Boolean fileDrop(DirPtr dir, int i, SelPtr sel, Boolean absolute,
		 Boolean dir_is_target, Boolean res_link)
{
  if (!dir || !sel)
    return False;
  else {
    FilePtr file = dirFile(dir, i);
    int type = fileType(file);
    String msg = comment(type);
    String action = drop_action(type);
    String dirname = drop_dirname(type);
    char path[MAXPATHLEN+1], realpath[MAXPATHLEN+1];
    char realdir[MAXPATHLEN+1], realname[MAXPATHLEN+1];

    if (!msg) msg = "";
    pathname(path, dirName(dir), fileName(file));
    if (res_link && S_ISLNK(fileLstats(file)->st_mode)) {
      resolve(realpath, path);
      dirpart(realdir, realpath);
      filepart(realname, realpath);
    } else {
      strcpy(realpath, path);
      strcpy(realdir, dirName(dir));
      strcpy(realname, fileName(file));
    }

    if (!dirname || !*dirname)
      dirname = dir_is_target?realdir:selDirname(sel);
    if (action && *action) {
      char buf[MAX_BUF+1];
      
      if (confirm_drop_dlg(fileLargeIcon(file), selNFiles(sel),
			   selDirname(sel), realpath) != OK)
	return False;
      if (param_dlg(fileLargeIcon(file), msg, action, buf, MAX_BUF)
	  == OK)
	return fileExec(buf, dirname,
			makeArgv(dirname, realpath, sel, absolute));
      else
	return True;
    } else if (fileStats(file)->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
      if (confirm_drop_dlg(fileLargeIcon(file), selNFiles(sel),
			   selDirname(sel), realpath) != OK)
	return False;
      action = alloca(strlen(realpath)+6);
      strcpy(action, realpath);
      strcat(action, " $* &");
      return fileExec(action, dirname,
		      makeArgv(dirname, NULL, sel, absolute));
    } else
      return True;
  }
}

Boolean fileApply(SelPtr sel, String command, String dirname,
		  Boolean absolute)
{
  if (command && *command)
    return fileExec(command, dirname,
		    makeArgv(dirname, NULL, sel, absolute));
  else
    return True;
}
