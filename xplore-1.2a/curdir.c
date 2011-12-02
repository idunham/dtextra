
#include "xplore.h"
#include "curdir.h"

#include "dirs.h"
#include "fileops.h"
#include "ftype.h"
#include "menus.h"
#include "mnttable.h"
#include "util.h"

DirPtr rootdir, curdir, shelfdir;
int shelf_no;

Boolean root_modified, cur_modified, shelf_modified;
Boolean cur_changed;

static char save_cur_path[MAXPATHLEN+1];
static char save_shelf_path[MAXPATHLEN+1];
static int save_shelf_options;

static void update_cur_path(String dirname)
{
  strcpy(save_cur_path, dirname);
  mysetenv("CURDIR", dirname, True);
}

static void update_shelf_path(String dirname)
{
  mysetenv("CURSHELF", dirname, True);
}

void cur_check(void)
{
  cur_changed = dirFindSubdir(rootdir, save_cur_path, &curdir) == NULL;
  if (!curdir)
    curdir = rootdir;
  if (cur_changed)
    update_cur_path(dirName(curdir));
  if (!dirHasFileInfo(curdir)) {
    if (dirIsReadable(curdir) ||
	dirDev(curdir) == NONE && !(dirOptions(curdir) & NOMOUNT)) {
      int l = strlen(dirName(curdir)) + 8;
      String msg = alloca((l+1) * sizeof(char));
      sprintf(msg, "Reading %s", dirName(curdir));
      WaitMsg(msg);
    }
    if (dirReadFileInfo(curdir))
      cur_modified = True;
  }
}

static Boolean check(DirPtr dir)
{
  if (dirModified(dir))
    return True;
  else if (dirHasSubdirInfo(dir)) {
    int i;

    for (i = 0; i < dirNSubdirs(dir); i++)
      if (check(dirSubdir(dir, i)))
	return True;
  }
  return False;
}

void update(void)
{
  StatRec stats;

  if (check_mnt_table())
    get_mnt_table();
  cur_changed = False;
  root_modified = check(rootdir);
  cur_modified = dirModified(curdir);
  shelf_modified = dirModified(shelfdir);
  if (!root_modified && !cur_modified && !shelf_modified)
    return;
  if (root_modified || shelf_modified ||
      !dirIsReadable(shelfdir) && !stat(dirName(shelfdir), &stats) &&
      S_ISDIR(stats.st_mode))
    WaitMsg("Updating view, please wait ...");
  else
    Wait();
  if (root_modified)
    dirUpdate(rootdir);
  if (shelf_modified)
    dirUpdate(shelfdir);
  else if (!dirIsReadable(shelfdir) && !stat(dirName(shelfdir), &stats) &&
	   S_ISDIR(stats.st_mode)) {
    /* force shelf to be reread */
    dirReread(shelfdir);
    dirReadFileInfo(shelfdir);
    shelf_modified = True;
  }
  cur_check();
  Done();
}

void restat(void)
{
  int d;

  if (check_mnt_table())
    get_mnt_table();
  root_modified = cur_modified = shelf_modified = True;
  WaitMsg("Rebuilding view, please wait ...");
  dirRestat(rootdir);
  dirRestat(shelfdir);
  cur_check();
  Done();
}

static DirPtr dirSearchSubdir(DirPtr dir, String dirname, DirPtr *ret)
{
  DirPtr ret1, ret2;

  if (!dirExpandSubdir(dir, dirname, &ret1)) {
  retry:
    if (!ret1)
      return (*ret = NULL);
    else if (dirFilter(ret1)) {
      dirSetFilter(ret1, NULL);
      if (dirExpandSubdir(dir, dirname, &ret2))
	return (*ret = ret2);
      else if (ret2 == ret1) {
	*ret = ret1;
	return NULL;
      } else {
	ret1 = ret2;
	goto retry;
      }
    } else if (!(dirOptions(ret1) & INCLUDE_HIDDEN)) {
      dirSetOptions(ret1, INCLUDE_HIDDEN);
      if (dirExpandSubdir(dir, dirname, &ret2))
	return (*ret = ret2);
      else if (ret2 == ret1) {
	*ret = ret1;
	return NULL;
      } else {
	ret1 = ret2;
	goto retry;
      }
    } else {
      *ret = ret1;
      return NULL;
    }
  } else
    return (*ret = ret1);
}

Boolean cur_init(String root_path, String cur_path, int options)
{
  DirPtr ret;

  if (rootdir)
    dirFree(rootdir);
  root_modified = cur_modified = cur_changed = True;
  shelf_modified = False;
  rootdir = dirCreate(root_path, NULL, options);
  if (!(curdir = dirSearchSubdir(rootdir, cur_path, &ret)))
    if (ret)
      curdir = ret;
    else
      curdir = rootdir;
  update_cur_path(dirName(curdir));
  if (!dirHasSubdirInfo(curdir))
    if (!dirReadSubdirInfo(curdir))
      return False;
  return dirReadFileInfo(curdir) != NULL;
}

Boolean shelf_init(String shelf_path, int options)
{
  char path[MAXPATHLEN+1];

  root_modified = cur_modified = cur_changed = False;
  shelf_modified = True;
  strcpy(save_shelf_path, shelf_path);
  save_shelf_options = options;
  if (shelfdir)
    dirFree(shelfdir);
  shelf_no = default_shelf;
  if (shelf_dir(shelf_no))
    pathname(path, shelf_path, shelf_dir(shelf_no));
  else if (shelf_label(shelf_no))
    pathname(path, shelf_path, shelf_label(shelf_no));
  else
    strcpy(path, shelf_path);
  update_shelf_path(path);
  shelfdir = dirCreate(path, NULL, options);
  return dirReadFileInfo(shelfdir) != NULL;
}

static Boolean dir_is_drop_target(DirPtr dir, int i)
{
  FilePtr file = dirFile(dir, i);
  String action = drop_action(fileType(file));

  return i == NONE || file &&
    (action && *action ||
     S_ISDIR(fileStats(file)->st_mode) ||
     (fileStats(file)->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)));
}

Boolean cur_is_drop_target(int i)
{
  return dir_is_drop_target(curdir, i);
}

Boolean shelf_is_drop_target(int i)
{
  return dir_is_drop_target(shelfdir, i);
}

Boolean cur_mkdir(String name)
{
  Boolean ret = dirNew(curdir, name);
  update();
  return ret;
}

Boolean cur_chdir(String name)
{
  DirPtr newdir;
  StatRec stats;

  root_modified = cur_modified = shelf_modified = cur_changed = False;
  if (!strcmp(dirName(curdir), name) && dirIsReadable(curdir))
    return True;
  else {
    Wait();
    root_modified = True;
    if (!dirSearchSubdir(rootdir, name, &newdir) ||
	strcmp(dirName(newdir), name)) {
      if (!newdir)
	root_modified = False;
      Done();
      return False;
    } else if (curdir != newdir || !dirIsReadable(curdir)) {
      int l = strlen(dirName(newdir)) + 8;
      String msg = alloca((l+1) * sizeof(char));
      sprintf(msg, "Reading %s", dirName(newdir));
      WaitMsg(msg);
      cur_modified = cur_changed = True;
      dirFreeFileInfo(curdir);
      curdir = newdir;
      if (dirOptions(curdir) & NOMOUNT)
	/* force remount */
	dirRestat(curdir);
      if (dirModified(curdir) && dirHasSubdirInfo(curdir))
	dirUpdateSubdirInfo(curdir);
      dirReadFileInfo(curdir);
      update_cur_path(dirName(curdir));
      Done();
      return True;
    }
  }
}

static Boolean dir_delete(DirPtr dir)
{
  if (dirNFilesSelected(dir)) {
    SelPtr sel = selFromDir(dir);
    Boolean ret = fileDelete(sel);

    selFree(sel);
    update();
    return ret;
  } else {
    root_modified = cur_modified = shelf_modified = cur_changed = False;
    return False;
  }
}

static Boolean dir_move(DirPtr dir, String target, Boolean absolute)
{
  if (dirNFilesSelected(dir)) {
    SelPtr sel = selFromDir(dir);
    Boolean ret = fileMove(sel, target, absolute);

    selFree(sel);
    update();
    return ret;
  } else {
    root_modified = cur_modified = shelf_modified = cur_changed = False;
    return False;
  }
}

static Boolean dir_copy(DirPtr dir, String target, Boolean absolute)
{
  if (dirNFilesSelected(dir)) {
    SelPtr sel = selFromDir(dir);
    Boolean ret = fileCopy(sel, target, absolute);

    selFree(sel);
    update();
    return ret;
  } else {
    root_modified = cur_modified = shelf_modified = cur_changed = False;
    return False;
  }
}

static Boolean dir_link(DirPtr dir, String target, Boolean absolute)
{
  if (dirNFilesSelected(dir)) {
    SelPtr sel = selFromDir(dir);
    Boolean ret = fileLink(sel, target, absolute);

    selFree(sel);
    update();
    return ret;
  } else {
    root_modified = cur_modified = shelf_modified = cur_changed = False;
    return False;
  }
}

Boolean cur_delete(void)
{
  return dir_delete(curdir);
}

Boolean cur_move(String target, Boolean absolute)
{
  return dir_move(curdir, target, absolute);
}

Boolean cur_copy(String target, Boolean absolute)
{
  return dir_copy(curdir, target, absolute);
}

Boolean cur_link(String target, Boolean absolute)
{
  return dir_link(curdir, target, absolute);
}

Boolean cur_push(int i, Boolean absolute, Boolean pushdir)
{
  FilePtr file = dirFile(curdir, i);
  String action;
  char path1[MAXPATHLEN+1], path2[MAXPATHLEN+1];

  root_modified = cur_modified = shelf_modified = cur_changed = False;
  if (file)
    if (!((action = push_action(fileType(file))) && *action) &&
	S_ISDIR(fileStats(file)->st_mode)) {
      return
	cur_chdir(shortestpath(path1, pathname(path2, dirName(curdir),
					       fileName(file))));
    } else {
      Boolean ret = filePush(curdir, i, NULL, absolute, False);
      update();
      return ret;
    }
  else
    return False;
}

#define fileMov(op) ((op==OP_COPY)?fileCopy:(op==OP_LINK)?fileLink:fileMove)

Boolean cur_drop(int i, int op, SelPtr sel, Boolean absolute,
		 Boolean dirtarget)
{
  Boolean ret;
  FilePtr file = dirFile(curdir, i);
  String action;
  char path[MAXPATHLEN+1];

  if (i != NONE)
    if (!((action = drop_action(fileType(file))) && *action) &&
	S_ISDIR(fileStats(file)->st_mode))
      ret = fileMov(op)(sel, pathname(path, dirName(curdir),
				      fileName(file)), absolute);
    else
      ret = fileDrop(curdir, i, sel, absolute, dirtarget, False);
  else
    ret = fileMov(op)(sel, dirName(curdir), absolute);
  update();
  return ret;
}

Boolean shelf_goto(int i)
{
  char path[MAXPATHLEN+1];

  root_modified = cur_modified = cur_changed = shelf_modified = False;
  if (i == shelf_no && dirIsReadable(shelfdir))
    return True;
  Wait();
  shelf_modified = True;
  dirFree(shelfdir);
  shelf_no = i;
  if (shelf_dir(shelf_no))
    pathname(path, save_shelf_path, shelf_dir(shelf_no));
  else if (shelf_label(shelf_no))
    pathname(path, save_shelf_path, shelf_label(shelf_no));
  else
    strcpy(path, save_shelf_path);
  update_shelf_path(path);
  shelfdir = dirCreate(path, NULL, save_shelf_options);
  Done();
  return dirReadFileInfo(shelfdir) != NULL;
}

Boolean shelf_delete(void)
{
  return dir_delete(shelfdir);
}

Boolean shelf_move(String target, Boolean absolute)
{
  return dir_move(shelfdir, target, absolute);
}

Boolean shelf_copy(String target, Boolean absolute)
{
  return dir_copy(shelfdir, target, absolute);
}

Boolean shelf_link(String target, Boolean absolute)
{
  return dir_link(shelfdir, target, absolute);
}

Boolean shelf_push(int i, Boolean absolute, Boolean pushdir)
{
  FilePtr file = dirFile(shelfdir, i);
  String action;
  char path1[MAXPATHLEN+1], path2[MAXPATHLEN+1];

  root_modified = cur_modified = shelf_modified = cur_changed = False;
  if (file)
    if (!((action = push_action(fileType(file))) && *action) &&
	S_ISDIR(fileStats(file)->st_mode)) {
      return
	cur_chdir(resolve(path1, pathname(path2, dirName(shelfdir),
					  fileName(file))));
    } else {
      Boolean ret =
	filePush(shelfdir, i,
		 (pushdir && (fileStats(file)->st_mode &
			      (S_IXUSR | S_IXGRP | S_IXOTH)))?
		 dirName(curdir):NULL, absolute, True);
      update();
      return ret;
    }
  else
    return False;
}

Boolean shelf_drop(int i, int op, SelPtr sel, Boolean absolute,
		   Boolean dirtarget)
{
  Boolean ret;
  FilePtr file = dirFile(shelfdir, i);
  String action;
  char path1[MAXPATHLEN+1], path2[MAXPATHLEN+1];

  if (i != NONE)
    if (!((action = drop_action(fileType(file))) && *action) &&
	S_ISDIR(fileStats(file)->st_mode))
      ret = fileMov(op)(sel, resolve(path1,
				     pathname(path2, dirName(shelfdir),
					      fileName(file))), absolute);
    else
      ret = fileDrop(shelfdir, i, sel, absolute, dirtarget, True);
  else
    ret = fileMov(op)(sel, dirName(shelfdir), absolute);
  update();
  return ret;
}
