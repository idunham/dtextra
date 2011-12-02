
#include "xplore.h"
#include "dirs.h"

#include "devmount.h"
#include "dialogs.h"
#include "error.h"
#include "ftype.h"
#include "icons.h"
#include "util.h"

static IconPtr large_std_icon(String name, StatPtr lstats, StatPtr stats)
{
    if (S_ISLNK(stats->st_mode))
	 return &std_icons[ICON_FILE_LINK_BAD];
    else if (S_ISDIR(stats->st_mode))
	if (S_ISLNK(lstats->st_mode))
	    return &std_icons[ICON_DIR_LINK];
	else if (!(strcmp(name, "..")))
	    return &std_icons[ICON_DIR_UP];
	else
	    return &std_icons[ICON_DIR];
    else if (stats->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	if (S_ISLNK(lstats->st_mode))
	    return &std_icons[ICON_EXEC_LINK];
	else
	    return &std_icons[ICON_EXEC];
    else if (S_ISLNK(lstats->st_mode))
	return &std_icons[ICON_FILE_LINK];
    else
	return &std_icons[ICON_FILE];
}

static IconPtr small_std_icon(String name, StatPtr lstats, StatPtr stats)
{
    if (S_ISLNK(stats->st_mode))
	 return &std_icons[ICON_FILE_LINK_BAD_SMALL];
    else if (S_ISDIR(stats->st_mode))
	if (S_ISLNK(lstats->st_mode))
	    return &std_icons[ICON_DIR_LINK_SMALL];
	else if (!(strcmp(name, "..")))
	    return &std_icons[ICON_DIR_UP_SMALL];
	else
	    return &std_icons[ICON_DIR_SMALL];
    else if (stats->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	if (S_ISLNK(lstats->st_mode))
	    return &std_icons[ICON_EXEC_LINK_SMALL];
	else
	    return &std_icons[ICON_EXEC_SMALL];
    else if (S_ISLNK(lstats->st_mode))
	return &std_icons[ICON_FILE_LINK_SMALL];
    else
	return &std_icons[ICON_FILE_SMALL];
}

static Boolean hasSubdirs(DirPtr dir)
{
  DIR *dirp;
  struct dirent *entry;
  StatRec stats;

  if (!chdir(dir->name) && (dirp = opendir("."))) {
    while (entry = readdir(dirp))
      if (stat(entry->d_name, &stats))
	;
      else if (S_ISDIR(stats.st_mode) &&
	       strcmp(entry->d_name, ".") &&
	       strcmp(entry->d_name, "..") &&
	       (*entry->d_name != '.' ||
		(dir->options & INCLUDE_HIDDEN))) {
	closedir(dirp);
	return True;
      }
    closedir(dirp);
  }
  return False;
}

static DirPtr dirStat(DirPtr dir, Boolean errflag)
{
  if (dir) {
    char getwd[MAXPATHLEN+1];
    char s[MAXPATHLEN+1];
    int d;
    Boolean magic_flag, has_stats = False, mounted = dir->dev != NONE;
      
    FREE(dir->getwd);
    dir->getwd = NULL;
    if (dir->dev != NONE && dir->dev != ANY && !devcount(dir->dev))
      dir->dev = NONE;
    if (!errflag && dir->dev == NONE && (d = finddev(dir->name)) != ANY &&
	!devcount(d) && !premounted(d)) {
      dir->isReadable = dir->hasSubdirs = False;
      dirFreeSubdirInfo(dir);
      dirFreeFileInfo(dir);
    } else if (dir->dev == NONE &&
	((dir->options & NOMOUNT) ||
	 (dir->dev = mount_dlg(NULL, dir->name,
			       errflag && !(dir->options & NOMOUNT)))
	 == NONE)) {
      if (errflag && !(dir->options & NOMOUNT))
	xplerr1(ERR_MOUNT, dir->name);
      dir->isReadable = dir->hasSubdirs = False;
      dirFreeSubdirInfo(dir);
      dirFreeFileInfo(dir);
    } else if (chdir(dir->name) || !getcwd(getwd, MAXPATHLEN) ||
	       lstat(dir->name, &dir->lstats) ||
	       stat(dir->name, &dir->stats)) {
      if (errflag) syserr1(ERR_READ_DIR, dir->name);
      umount_dlg(NULL, dir->dev);
      dir->isReadable = dir->hasSubdirs = False;
      dir->dev = NONE;
      dirFreeSubdirInfo(dir);
      dirFreeFileInfo(dir);
    } else if (!devcheck(dir->dev)) {
      devumount_ignore(dir->dev);
      dir->isReadable = dir->hasSubdirs = False;
      dir->dev = NONE;
      dir->options |= NOMOUNT;
      dirFreeSubdirInfo(dir);
      dirFreeFileInfo(dir);
    } else {
      has_stats = True;
      dir->getwd = NEWSTRING(getwd);
      dir->isReadable = True;
      dir->hasSubdirs = hasSubdirs(dir);
      if (!mounted && dir->dev != NONE && dir->dev != ANY &&
	  (!dir->parent || dir->dev != dir->parent->dev)) {
	if (devoptions(dir->dev) != NONE)
	  dir->options = devoptions(dir->dev);
	dir->options |= devsetoptions(dir->dev);
	dir->options &= ~devclroptions(dir->dev);
      }
    }
    if (dir->parent)
      magic_flag = !(dir->parent->options & NOMAGIC);
    else
      magic_flag = !(dir->options & NOMAGIC);
    dir->type = file_type(dir->name, magic_flag);
    dir->large_icon = large_icon(dir->type);
    dir->small_icon = small_icon(dir->type);
    if (!has_stats)
      has_stats = !lstat(dir->name, &dir->lstats) &&
	!stat(dir->name, &dir->stats);
    if (!dir->large_icon)
      if (!has_stats)
	dir->large_icon = &std_icons[ICON_DIR_BAD];
      else
	dir->large_icon = large_std_icon(filepart(s, dir->name),
					 &dir->lstats,
					 &dir->stats);
    if (!dir->small_icon)
      if (!has_stats)
	dir->small_icon = &std_icons[ICON_DIR_BAD_SMALL];
      else
	dir->small_icon = small_std_icon(filepart(s, dir->name),
					 &dir->lstats,
					 &dir->stats);
    dir->options &= ~MODIFIED;
    return dir->isReadable?dir:NULL;
  } else
    return NULL;
}

static DirPtr dirCreateSubdir(String name, DirPtr parent)
{
    DirPtr dir = (DirPtr) MALLOC(sizeof(DirRec));
    char path[MAXPATHLEN+1];

    pathname(path, parent->name, name);
    dir->name = NEWSTRING(path);
    dir->filter = NEWSTRING(parent->filter);
    dir->options = parent->options;
    dir->nSubdirs = dir->nFiles = dir->nBytes = 0;
    dir->nFilesSelected = dir->nBytesSelected = 0;
    dir->parent = parent;
    dir->subdirs = NULL;
    dir->files = NULL;
    dir->isReadable = dir->hasSubdirs =
      dir->hasSubdirInfo = dir->hasFileInfo = False;
    dir->getwd = NULL;
    dir->dev = NONE;
    dir->type = NONE;
    dir->large_icon = dir->small_icon = NULL;
    dirStat(dir, False);
    return dir;
}

/**************************************************************************/

DirPtr dirCreate(String name, String filter, int options)
{
    DirPtr dir = (DirPtr) MALLOC(sizeof(DirRec));
    char absname[MAXPATHLEN+1];

    abspath(absname, basedir, name);
    dir->name = NEWSTRING(absname);
    dir->filter = NEWSTRING(filter);
    dir->options = options;
    if (!(dir->options & SORT_MASK))
	dir->options |= SORT_BY_NAME;
    dir->nSubdirs = dir->nFiles = dir->nBytes = 0;
    dir->nFilesSelected = dir->nBytesSelected = 0;
    dir->parent = NULL;
    dir->subdirs = NULL;
    dir->files = NULL;
    dir->isReadable = dir->hasSubdirs =
      dir->hasSubdirInfo = dir->hasFileInfo = False;
    dir->getwd = NULL;
    dir->dev = NONE;
    dir->type = NONE;
    dir->large_icon = dir->small_icon = NULL;
    dirStat(dir, False);
    return dir;
}

/**************************************************************************/

static DirList dirsFree(DirList subdirs, int nSubdirs)
{
    if (subdirs) {
	int i;
	for (i = 0; i < nSubdirs; i++)
	    dirFree(subdirs[i]);
	FREE(subdirs);
    }
    return NULL;
}

static FilePtr fileFree(FilePtr file)
{
    if (file) {
	FREE(file->name);
	FREE(file);
    }
    return NULL;
}

static FileList filesFree(FileList files, int nFiles)
{
    if (files) {
	int i;
	for (i = 0; i < nFiles; i++)
	    fileFree(files[i]);
	FREE(files);
    }
    return NULL;
}

/**************************************************************************/

DirPtr dirFree(DirPtr dir)
{
    if (dir) {
	FREE(dir->name);
	FREE(dir->getwd);
	umount_dlg(NULL, dir->dev);
	FREE(dir->filter);
	dirsFree(dir->subdirs, dir->nSubdirs);
	filesFree(dir->files, dir->nFiles);
	FREE(dir);
    }
    return NULL;
}

/**************************************************************************/

static int dirCmp(const DirPtr *dirp1, const DirPtr *dirp2)
{
    DirPtr dir1 = *dirp1, dir2 = *dirp2;
    return strcmp(dir1->name, dir2->name);
}

static void dirsSort(DirList dirs, int nDirs)
{
    qsort(dirs, nDirs, sizeof(DirPtr),
	  (int (*)(const void *, const void *))dirCmp);
}

/**************************************************************************/

static DirPtr dirScanSubdirInfo(DirPtr dir, Boolean reread)
{
  if (dir) {
    DIR *dirp;

    if (!dir->isReadable && !dirStat(dir, True) ||
	chdir(dir->name) || !(dirp = opendir("."))) {
      if (dir->isReadable) {
	syserr1(ERR_READ_DIR, dir->name);
	dir->isReadable = dir->hasSubdirs = False;
	dirFreeSubdirInfo(dir);
	dirFreeFileInfo(dir);
      }
      return NULL;
    } else {
      int nSubdirs = 0;
      DirList subdirs = NULL;
      DirPtr subdir;
      StatRec stats;
      struct dirent *entry;
      int n_alloc = 0;

      /* read in subdir information: */
      for (nSubdirs = 0; (entry = readdir(dirp)); ) {
	if (stat(entry->d_name, &stats))
	  /* ignore this entry (probably stale symlink) */
	  ;
	else if (S_ISDIR(stats.st_mode) &&
		 strcmp(entry->d_name, ".") &&
		 strcmp(entry->d_name, "..") &&
		 (*entry->d_name != '.' ||
		  (dir->options & INCLUDE_HIDDEN))) {
	  char path[MAXPATHLEN+1];
	  
	  if (nSubdirs == n_alloc)
	    subdirs = (DirList) REALLOC(subdirs, (n_alloc =
						  nSubdirs+100) *
					sizeof(DirPtr));
	  subdir = subdirs[nSubdirs] =
	    dirCreateSubdir(entry->d_name, dir);
	  subdir->parent = dir;
	  nSubdirs++;
	  chdir(dir->name);
	}
      }
      if (closedir(dirp)) {
	syserr1(ERR_READ_DIR, dir->name);
	dirsFree(subdirs, nSubdirs);
	dir->isReadable = dir->hasSubdirs = False;
	dirFreeSubdirInfo(dir);
	dirFreeFileInfo(dir);
	return NULL;
      } else {
	if (nSubdirs == 0) {
	  FREE(subdirs);
	  subdirs = NULL;
	} else
	  subdirs = (DirList) REALLOC(subdirs,
				      (nSubdirs)*sizeof(DirPtr));
	/* sort subdirectories: */
	dirsSort(subdirs, nSubdirs);
	/* merge subdir info with current subdir info if present: */
	if (dir->hasSubdirInfo) {
	  int i, j, k;

	  for (i = j = 0; i < dir->nSubdirs && j < nSubdirs; )
	    if ((k = dirCmp(&dir->subdirs[i], &subdirs[j])) < 0) {
	      dirFree(dir->subdirs[i]);
	      i++;
	    } else if (k > 0)
	      j++;
	    else if (dirHasSubdirInfo(dir->subdirs[i]) ||
		     dirHasFileInfo(dir->subdirs[i])) {
	      (reread?dirReread:dirUpdate)(dir->subdirs[i]);
	      dirFree(subdirs[j]);
	      subdirs[j] = dir->subdirs[i];
	      i++, j++;
	    } else {
	      dirFree(dir->subdirs[i]);
	      i++, j++;
	    }
	  FREE(dir->subdirs);
	}
	/* install the new subdir info: */
	dir->subdirs = subdirs;
	dir->nSubdirs = nSubdirs;
	dir->hasSubdirInfo = True;
	return dir;
      }
    }
  } else
    return NULL;
}

DirPtr dirReadSubdirInfo(DirPtr dir)
{
  return dirScanSubdirInfo(dir, True);
}

DirPtr dirUpdateSubdirInfo(DirPtr dir)
{
  return dirScanSubdirInfo(dir, False);
}

/**************************************************************************/

static Boolean filepred(String name, StatPtr stats, int options)
{
    if (!name || !*name)
	return False;
    else if (!strcmp(name, "."))
	return False;
    else if (!strcmp(name, ".."))
	return (options & INCLUDE_UPDIR) != 0;
    else if (S_ISDIR(stats->st_mode))
	if (options & INCLUDE_DIRS)
	    return *name != '.' || (options & INCLUDE_HIDDEN);
	else
	    return False;
    else
	if (options & INCLUDE_FILES)
	    return *name != '.' || (options & INCLUDE_HIDDEN);
	else
	    return False;
}

typedef int (*FileCmpFun)(const FilePtr*, const FilePtr*);

static void filesSort(FileList files, int nFiles, FileCmpFun cmp)
{
    if (cmp)
	qsort(files, nFiles, sizeof(FilePtr),
	      (int (*)(const void *, const void *))cmp);
}

static Boolean dirs_first, reverse;

static int fileCmpName(const FilePtr *filep1, const FilePtr *filep2)
{
    FilePtr file1 = *filep1, file2 = *filep2;
    int ret;
    /* force .. to be the first item */
    if (!strcmp(file1->name, ".."))
      if (strcmp(file2->name, ".."))
	return -1;
      else
	return 0;
    else if (!strcmp(file2->name, ".."))
      return 1;
    if (dirs_first) {
	if (S_ISDIR(file1->stats.st_mode)) {
	    if (!S_ISDIR(file2->stats.st_mode))
		return -1;
	} else if (S_ISDIR(file2->stats.st_mode))
	    return 1;
    }
    ret = strcmp(file1->name, file2->name);
    return reverse?(-ret):ret;
}

static int fileCmpSize(const FilePtr *filep1, const FilePtr *filep2)
{
    FilePtr file1 = *filep1, file2 = *filep2;
    int ret;
    if (!strcmp(file1->name, ".."))
      if (strcmp(file2->name, ".."))
	return -1;
      else
	return 0;
    else if (!strcmp(file2->name, ".."))
      return 1;
    if (file1->stats.st_size == file2->stats.st_size)
	return fileCmpName(filep1, filep2);
    if (dirs_first) {
	if (S_ISDIR(file1->stats.st_mode)) {
	    if (!S_ISDIR(file2->stats.st_mode))
		return -1;
	} else if (S_ISDIR(file2->stats.st_mode))
	    return 1;
    }
    ret = (int)(file2->stats.st_size - file1->stats.st_size);
    return reverse?(-ret):ret;
}

static int fileCmpDate(const FilePtr *filep1, const FilePtr *filep2)
{
    FilePtr file1 = *filep1, file2 = *filep2;
    int ret;
    if (!strcmp(file1->name, ".."))
      if (strcmp(file2->name, ".."))
	return -1;
      else
	return 0;
    else if (!strcmp(file2->name, ".."))
      return 1;
    if (file1->stats.st_mtime == file2->stats.st_mtime)
	return fileCmpName(filep1, filep2);
    if (dirs_first) {
	if (S_ISDIR(file1->stats.st_mode)) {
	    if (!S_ISDIR(file2->stats.st_mode))
		return -1;
	} else if (S_ISDIR(file2->stats.st_mode))
	    return 1;
    }
    ret = (int)(file2->stats.st_mtime - file1->stats.st_mtime);
    return reverse?(-ret):ret;
}

static FileCmpFun fileCmp(DirPtr dir)
{
    dirs_first = (dir->options & DIRS_FIRST) != 0;
    reverse = (dir->options & REVERSE) != 0;
    return (dir->options & SORT_BY_NAME)?(fileCmpName):
	(dir->options & SORT_BY_SIZE)?(fileCmpSize):
	(dir->options & SORT_BY_DATE)?(fileCmpDate):
	NULL;
}

/**************************************************************************/

DirPtr dirReadFileInfo(DirPtr dir)
{
  if (dir) {
    DIR *dirp;

    if (!dir->isReadable && !dirStat(dir, True) ||
	chdir(dir->name) || !(dirp = opendir("."))) {
      if (dir->isReadable) {
	syserr1(ERR_READ_DIR, dir->name);
	dir->isReadable = dir->hasSubdirs = False;
	dirFreeSubdirInfo(dir);
	dirFreeFileInfo(dir);
      }
      return NULL;
    } else {
      int nFiles = 0, nBytes = 0;
      FileList files = NULL;
      FilePtr file;
      StatRec stats, lstats;
      struct dirent *entry;
      int n_alloc = 0;

      /* read in file information: */
      for (nFiles = nBytes = 0; (entry = readdir(dirp)); ) {
	if (lstat(entry->d_name, &lstats))
	  /* file just disappeared, ignore it */
	  continue;
	if (stat(entry->d_name, &stats))
	  /* stale symlink */
	  stats = lstats;
	if (filepred(entry->d_name, &stats, dir->options) &&
	    (!dir->filter || !strcmp(entry->d_name, "..") ||
	     fnmatch(dir->filter, entry->d_name))) {
	  char path[MAXPATHLEN+1];
	  if (nFiles == n_alloc)
	    files = (FileList) REALLOC(files, (n_alloc =
					       nFiles+100) *
				       sizeof(FilePtr));
	  file = files[nFiles] = (FilePtr) MALLOC(sizeof(FileRec));
	  file->stats = stats;
	  file->lstats = lstats;
	  nBytes += file->stats.st_size;
	  file->name = NEWSTRING(entry->d_name);
	  file->selected = False;
	  file->type = file_type(pathname(path, dir->name,
					  file->name),
				 !(dir->options & NOMAGIC)?True:
				 False);
	  file->large_icon = large_icon(file->type);
	  file->small_icon = small_icon(file->type);
	  if (!file->large_icon)
	    file->large_icon = large_std_icon(file->name,
					      &file->lstats,
					      &file->stats);
	  if (!file->small_icon)
	    file->small_icon = small_std_icon(file->name,
					      &file->lstats,
					      &file->stats);
	  nFiles++;
	}
      }
      if (closedir(dirp)) {
	syserr1(ERR_READ_DIR, dir->name);
	filesFree(files, nFiles);
	dir->isReadable = dir->hasSubdirs = False;
	dirFreeSubdirInfo(dir);
	dirFreeFileInfo(dir);
	return NULL;
      } else {
	FileCmpFun cmp = fileCmp(dir);
	
	if (nFiles == 0) {
	  FREE(files);
	  files = NULL;
	} else
	  files = (FileList) REALLOC(files, (nFiles)*sizeof(FilePtr));
	/* sort files: */
	filesSort(files, nFiles, cmp);
	/* merge file info with current file info if present: */
	if (dir->hasFileInfo) {
	  int i, j, k;
	  
	  dir->nFilesSelected = dir->nBytesSelected = 0;
	  for (i = j = 0; i < dir->nFiles && j < nFiles; )
	    if ((k = cmp(&dir->files[i], &files[j])) < 0)
	      i++;
	    else if (k > 0)
	      j++;
	    else {
	      if ((files[j]->selected =
		   dir->files[i]->selected)) {
		dir->nFilesSelected++;
		dir->nBytesSelected += files[j]->stats.st_size;
	      }
	      i++, j++;
	    }
	  filesFree(dir->files, dir->nFiles);
	}
	/* install the new fileinfo: */
	dir->files = files;
	dir->nFiles = nFiles;
	dir->nBytes = nBytes;
	dir->hasFileInfo = True;
	return dir;
      }
    }
  } else
    return NULL;
}

/**************************************************************************/

DirPtr dirFreeSubdirInfo(DirPtr dir)
{
    if (dir) {
	if (dir->hasSubdirInfo) {
	    dir->subdirs = dirsFree(dir->subdirs, dir->nSubdirs);
	    dir->nSubdirs = 0;
	    dir->hasSubdirInfo = False;
	}
	return dir;
    } else
	return NULL;
}

/**************************************************************************/

DirPtr dirFreeFileInfo(DirPtr dir)
{
    if (dir) {
	if (dir->hasFileInfo) {
	    dir->files = filesFree(dir->files, dir->nFiles);
	    dir->nFiles = dir->nBytes = 0;
	    dir->nFilesSelected = dir->nBytesSelected = 0;
	    dir->hasFileInfo = False;
	}
	return dir;
    } else
	return NULL;
}

/**************************************************************************/

DirPtr dirReread(DirPtr dir)
{
    dirStat(dir, False);
    if (dir->hasSubdirInfo)
	dirReadSubdirInfo(dir);
    if (dir->hasFileInfo)
	dirReadFileInfo(dir);
    return dir;
}

/**************************************************************************/

DirPtr dirUnmountAll(DirPtr dir)
{
  if (dir)
    if (dir->dev != ANY) {
      if (dir->dev != NONE &&
	  (devmount_action(dir->dev) || devumount_action(dir->dev))) {
	dirFreeSubdirInfo(dir);
	dirFreeFileInfo(dir);
	umount_dlg(NULL, dir->dev);
	dir->isReadable = dir->hasSubdirs = False;
	dir->dev = NONE;
	dir->options |= NOMOUNT;
      }
    } else {
      int i;

      for (i = 0; i < dir->nSubdirs; i++)
	dirUnmountAll(dir->subdirs[i]);
    }
  return dir;
}

/**************************************************************************/

static DirPtr remount(DirPtr dir)
{
  int i;

  dir->options &= ~NOMOUNT;
  for (i = 0; i < dir->nSubdirs; i++)
    remount(dir->subdirs[i]);
  return dir;
}

DirPtr dirRestat(DirPtr dir)
{
  remount(dir);
  return dirReread(dir);
}

/**************************************************************************/

DirPtr dirUpdate(DirPtr dir)
{
    if (dirModified(dir)) {
      dirStat(dir, False);
      if (dir->hasSubdirInfo)
	dirUpdateSubdirInfo(dir);
      if (dir->hasFileInfo)
	dirReadFileInfo(dir);
    } else if (dirHasSubdirInfo(dir)) {
      int i;
      
      for (i = 0; i < dirNSubdirs(dir); i++)
	dirUpdate(dirSubdir(dir, i));
    }
    return dir;
}

/**************************************************************************/

DirPtr dirSetFilter(DirPtr dir, String filter)
{
    if (!dir)
	return NULL;
    else {
	FREE(dir->filter);
	dir->filter = NEWSTRING(filter);
	return dirReread(dir);
    }
}

/**************************************************************************/

static DirPtr dirUpdateOptions(DirPtr dir)
{
  if (dir) {
    DirPtr res;
    int set_options = dir->options & MODIFIED;
    if (dir->hasFileInfo)
      filesSort(dir->files, dir->nFiles, fileCmp(dir));
    res = dirReread(dir);
    dir->options |= set_options;
    return res;
  } else
    return NULL;
}

/**************************************************************************/

DirPtr dirSetOptions(DirPtr dir, int options)
{
  if (dir) {
    if (options == MODIFIED) {
      dir->options |= options;
      return dir;
    }
    dir->options = (dir->options & ~SORT_MASK | options & ~SORT_MASK) |
      dir->options & SORT_MASK;
    if (options & SORT_MASK)
      dir->options = dir->options & ~SORT_MASK | options & SORT_MASK;
    return dirUpdateOptions(dir);
  } else
    return NULL;
}

/**************************************************************************/

DirPtr dirClearOptions(DirPtr dir, int options)
{
  if (dir) {
    if (options == MODIFIED) {
      dir->options &= ~options;
      return dir;
    }
    dir->options = dir->options & ~options;
    if (!(dir->options & SORT_MASK))
      dir->options |= SORT_BY_NAME;
    return dirUpdateOptions(dir);
  } else
    return NULL;
}

/**************************************************************************/

DirPtr dirSelect(DirPtr dir, int i, String pattern)
{
    if (!dir)
	return NULL;
    else if (i == ALL) {
	for (i = 0; i < dir->nFiles; i++)
	    if (!dirSelect(dir, i, pattern))
		return NULL;
	return dir;
    } else if (i < 0 || i >= dir->nFiles)
	return NULL;
    else {
        if ((!pattern || fnmatchnodot(pattern, dir->files[i]->name)) &&
	    !dir->files[i]->selected) {
	    dir->nFilesSelected++;
	    dir->nBytesSelected += dir->files[i]->lstats.st_size;
	    dir->files[i]->selected = True;
	}
	return dir;
    }
}

/**************************************************************************/

DirPtr dirDeselect(DirPtr dir, int i, String pattern)
{
    if (!dir)
	return NULL;
    else if (i == ALL) {
	for (i = 0; i < dir->nFiles; i++)
	    if (!dirDeselect(dir, i, pattern))
		return NULL;
	return dir;
    } else if (i < 0 || i >= dir->nFiles)
	return NULL;
    else {
        if ((!pattern || fnmatchnodot(pattern, dir->files[i]->name)) &&
	    dir->files[i]->selected) {
	    dir->nFilesSelected--;
	    dir->nBytesSelected -= dir->files[i]->lstats.st_size;
	    dir->files[i]->selected = False;
	}
	return dir;
    }
}

/**************************************************************************/

DirPtr dirInvert(DirPtr dir, int i, String pattern)
{
    if (!dir)
	return NULL;
    else if (i == ALL) {
	for (i = 0; i < dir->nFiles; i++)
	    if (!dirInvert(dir, i, pattern))
		return NULL;
	return dir;
    } else if (i < 0 || i >= dir->nFiles)
	return NULL;
    else {
        if (!pattern || fnmatchnodot(pattern, dir->files[i]->name)) {
	    if (dir->files[i]->selected) {
	      dir->nFilesSelected--;
	      dir->nBytesSelected -= dir->files[i]->lstats.st_size;
	    } else {
	      dir->nFilesSelected++;
	      dir->nBytesSelected += dir->files[i]->lstats.st_size;
	    }
	    dir->files[i]->selected = !dir->files[i]->selected;
	}
	return dir;
    }
}

/**************************************************************************/

String dirName(DirPtr dir)
{
    if (dir)
	return dir->name;
    else
	return NULL;
}

/**************************************************************************/

String dirFilter(DirPtr dir)
{
    if (dir)
	return dir->filter;
    else
	return NULL;
}

/**************************************************************************/

int dirOptions(DirPtr dir)
{
    if (dir)
	return dir->options;
    else
	return 0;
}

/**************************************************************************/

DirPtr dirParent(DirPtr dir)
{
    if (dir)
	return dir->parent;
    else
	return NULL;
}

/**************************************************************************/

DirPtr dirTop(DirPtr dir)
{
    if (dir) {
	while (dir->parent)
	    dir = dir->parent;
	return dir;
    } else
	return NULL;
}

/**************************************************************************/

#define retval(ret,dir) ((ret)?(*(ret)=(dir)):(dir))
#define dirPrefix(name1,name2) (prefix((name1),(name2))&&\
                                (!strcmp((name1),"/")||\
				 (name2)[strlen(name1)]=='/'))

DirPtr dirFindSubdir(DirPtr dir, String dirname, DirPtr *ret)
{
    if (!strcmp(dir->name, dirname))
	return retval(ret, dir);
    else if (dirPrefix(dir->name, dirname)) {
	int i;
	DirPtr ret1;

	for (i = 0; i < dir->nSubdirs; i++)
	    if (dirFindSubdir(dir->subdirs[i], dirname, &ret1))
		return retval(ret, ret1);
	    else if (ret1) {
		retval(ret, ret1);
		return NULL;
	    }
	retval(ret, dir);
	return NULL;
    } else
	return retval(ret, NULL);
}

/**************************************************************************/

DirPtr dirExpandSubdir(DirPtr dir, String dirname, DirPtr *ret)
{
  DirPtr actdir;

  if (!strcmp(dir->name, dirname))
    return (*ret = dir);
  else if (!dirPrefix(dir->name, dirname))
    return (*ret = NULL);
  else {
    for (actdir = dir; strcmp(actdir->name, dirname); actdir = *ret)
      if (!actdir->hasSubdirInfo && !dirReadSubdirInfo(actdir))
	break;
      else if (dirFindSubdir(actdir, dirname, ret)) {
	actdir = *ret;
	break;
      } else if (!*ret || *ret == actdir)
	break;
    *ret = actdir;
    return strcmp(actdir->name, dirname)?NULL:actdir;
  }
}

/**************************************************************************/

int dirType(DirPtr dir)
{
    if (dir)
	return dir->type;
    else
	return NONE;
}

/**************************************************************************/

IconPtr dirLargeIcon(DirPtr dir)
{
    if (dir)
	return dir->large_icon;
    else
	return NULL;
}

/**************************************************************************/

IconPtr dirSmallIcon(DirPtr dir)
{
    if (dir)
	return dir->small_icon;
    else
	return NULL;
}

/**************************************************************************/

Boolean dirIsReadable(DirPtr dir)
{
    if (dir)
	return dir->isReadable;
    else
	return False;
}

/**************************************************************************/

Boolean dirHasSubdirs(DirPtr dir)
{
    if (dir)
	return dir->hasSubdirs;
    else
	return False;
}

/**************************************************************************/

Boolean dirHasSubdirInfo(DirPtr dir)
{
    if (dir)
	return dir->hasSubdirInfo;
    else
	return False;
}

/**************************************************************************/

Boolean dirHasFileInfo(DirPtr dir)
{
    if (dir)
	return dir->hasFileInfo;
    else
	return False;
}

/**************************************************************************/

Boolean dirModified(DirPtr dir)
{
  StatRec stats, lstats;

  if (!dir)
    return False;
  else if (dir->options & MODIFIED)
    return True;
  else if (!dir->isReadable) {
    if (dir->dev == NONE)
      return False;
    else if (!stat(dir->name, &stats) &&
	     S_ISDIR(stats.st_mode)) 
      return True;
    else
      return False;
  } else {
    if (stat(dir->name, &stats) ||
	lstat(dir->name, &lstats) ||
	!S_ISDIR(stats.st_mode) ||
	chdir(dir->name) ||
	!devcheck(dir->dev))
      return True;
    else if (stats.st_ctime > dir->stats.st_ctime ||
	     stats.st_mtime > dir->stats.st_mtime ||
	     lstats.st_ctime > dir->lstats.st_ctime ||
	     lstats.st_mtime > dir->lstats.st_mtime)
      if (hasSubdirs(dir) != dir->hasSubdirs)
	return True;
      else if (dir->hasSubdirInfo || dir->hasFileInfo)
	return True;
      else
	return False;
    else if (dir->options & CHECK_FILES) {
      int i;
      
      for (i = 0; i < dir->nFiles; i++) {
	FilePtr file = dir->files[i];
	if (lstat(file->name, &lstats))
	  return True;
	else if (stat(file->name, &stats))
	  if (!S_ISLNK(file->stats.st_mode))
	    return True;
	  else
	    /* stale symlink */
	    ;
	else if (lstats.st_ctime > file->lstats.st_ctime ||
		 lstats.st_mtime > file->lstats.st_mtime ||
		 stats.st_ctime > file->stats.st_ctime ||
		 stats.st_mtime > file->stats.st_mtime ||
		 S_ISLNK(stats.st_mode) != S_ISLNK(file->stats.st_mode))
	  return True;
      }
      return False;
    } else
      return False;
  }
}

/**************************************************************************/

String dirGetwd(DirPtr dir)
{
    if (dir)
	return dir->getwd;
    else
	return NULL;
}

/**************************************************************************/

int dirDev(DirPtr dir)
{
    if (dir)
	return dir->dev;
    else
	return NONE;
}

/**************************************************************************/

StatPtr dirStats(DirPtr dir)
{
    if (dir)
	return &dir->stats;
    else
	return NULL;
}

/**************************************************************************/

StatPtr dirLstats(DirPtr dir)
{
    if (dir)
	return &dir->lstats;
    else
	return NULL;
}

/**************************************************************************/

int dirNSubdirs(DirPtr dir)
{
    if (dir)
	return dir->nSubdirs;
    else
	return 0;
}

/**************************************************************************/

int dirNFiles(DirPtr dir)
{
    if (dir)
	return dir->nFiles;
    else
	return 0;
}

/**************************************************************************/

int dirNBytes(DirPtr dir)
{
    if (dir)
	return dir->nBytes;
    else
	return 0;
}

/**************************************************************************/

int dirNFilesSelected(DirPtr dir)
{
    if (dir)
	return dir->nFilesSelected;
    else
	return 0;
}

/**************************************************************************/

int dirNBytesSelected(DirPtr dir)
{
    if (dir)
	return dir->nBytesSelected;
    else
	return 0;
}

/**************************************************************************/

DirPtr dirSubdir(DirPtr dir, int i)
{
    if (dir && 0 <= i && i < dir->nSubdirs)
	return dir->subdirs[i];
    else
	return NULL;
}

/**************************************************************************/

FilePtr dirFile(DirPtr dir, int i)
{
    if (dir && 0 <= i && i < dir->nFiles)
	return dir->files[i];
    else
	return NULL;
}

/**************************************************************************/

String fileName(FilePtr file)
{
    if (file)
	return file->name;
    else
	return NULL;
}

/**************************************************************************/

StatPtr fileStats(FilePtr file)
{
    if (file)
	return &file->stats;
    else
	return NULL;
}

/**************************************************************************/

StatPtr fileLstats(FilePtr file)
{
    if (file)
	return &file->lstats;
    else
	return NULL;
}

/**************************************************************************/

Boolean fileSelected(FilePtr file)
{
    if (file)
	return file->selected;
    else
	return False;
}

/**************************************************************************/

int fileType(FilePtr file)
{
    if (file)
	return file->type;
    else
	return NONE;
}

/**************************************************************************/

IconPtr fileLargeIcon(FilePtr file)
{
    if (file)
	return file->large_icon;
    else
	return NULL;
}

/**************************************************************************/

IconPtr fileSmallIcon(FilePtr file)
{
    if (file)
	return file->small_icon;
    else
	return NULL;
}

/**************************************************************************/

SelPtr selFromDir(DirPtr dir)
{
    if (dir) {
	int i, j;
	SelPtr sel = (SelPtr) MALLOC(sizeof(SelRec));

	sel->dirname = NEWSTRING(dir->name);
	sel->nFiles = dir->nFilesSelected;
	sel->filenames = (String*) MALLOC(sel->nFiles*sizeof(String));
	for (i = j = 0; i < dir->nFiles; i++)
	    if (dir->files[i]->selected)
		sel->filenames[j++] = NEWSTRING(dir->files[i]->name);
	return sel;
    } else
	return NULL;
}

/**************************************************************************/

SelPtr selFromBuffer(String buf)
{
    String buf1 = alloca(strlen(buf)+1), s;

    if ((s = split2(strcpy(buf1, buf), ' '))) {
	SelPtr sel = (SelPtr) MALLOC(sizeof(SelRec));
	char name[MAXPATHLEN+1], path[MAXPATHLEN+1];

	abspath(path, basedir, unquote(name, s));
	sel->dirname = NEWSTRING(path);
	sel->nFiles = 0;
	sel->filenames = NULL;
	while ((s = split2(NULL, ' ')))
	    if (*s) {
		sel->filenames = (String*) REALLOC(sel->filenames,
						   (sel->nFiles+1)*
						   sizeof(String));
		unquote(name, s);
		sel->filenames[sel->nFiles++] = NEWSTRING(name);
	    }
	return sel;
    } else
	return NULL;
}

/**************************************************************************/

SelPtr selFromName(String name)
{
    SelPtr sel = (SelPtr) MALLOC(sizeof(SelRec));
    char s[MAXPATHLEN+1], path[MAXPATHLEN+1];

    abspath(path, basedir, dirpart(s, name));
    sel->dirname = NEWSTRING(path);
    sel->nFiles = 1;
    filepart(s, name);
    sel->filenames = (String*) MALLOC(sizeof(String));
    sel->filenames[0] = NEWSTRING(s);
    if (*s)
      return sel;
    else
      return selFree(sel);
}

/**************************************************************************/

SelPtr selFree(SelPtr sel)
{
    if (sel) {
	int i;

	FREE(sel->dirname);
	for (i = 0; i < sel->nFiles; i++)
	    FREE(sel->filenames[i]);
	FREE(sel->filenames);
    }
    return NULL;
}

/**************************************************************************/

String selDirname(SelPtr sel)
{
    if (sel)
	return sel->dirname;
    else
	return NULL;
}

/**************************************************************************/

int selNFiles(SelPtr sel)
{
    if (sel)
	return sel->nFiles;
    else
	return 0;
}

/**************************************************************************/

String selFilename(SelPtr sel, int i)
{
    if (sel && 0 <= i && i < sel->nFiles)
	return sel->filenames[i];
    else
	return NULL;
}

/**************************************************************************/

static String addToBuffer(String buf, int *bufsz, String s, char delim)
{
    int l = strlen(s);

    buf = (String) REALLOC(buf, (*bufsz+l+(buf?2:1))*sizeof(char));
    if (delim && *bufsz)
	buf[(*bufsz)++] = delim;
    strcpy(buf+*bufsz, s);
    *bufsz += l;
    return buf;
}

/**************************************************************************/

String selBuffer(SelPtr sel)
{
    if (!sel)
	return NULL;
    else {
	String buf = NULL;
	int bufsz = 0, i;
	char namestr[2*MAXPATHLEN+1];

	buf = addToBuffer(buf, &bufsz, quote(namestr, sel->dirname, " \\"), ' ');
	for (i = 0; i < sel->nFiles; i++)
	    buf = addToBuffer(buf, &bufsz,
			      quote(namestr, sel->filenames[i], " \\"), ' ');
	return buf;
    }
}

/**************************************************************************/

String uriBuffer(SelPtr sel)
{
    if (!sel)
	return NULL;
    else {
	String buf = NULL;
	int bufsz = 0, i, l;
	char namestr[3*MAXPATHLEN+20], host[MAXPATHLEN+1];

	if (gethostname(host, MAXPATHLEN))
	  return NULL;
	sprintf(namestr, "file:%s", sel->dirname);
	l = strlen(namestr);
	if (namestr[l-1] != '/') {
	  strcat(namestr, "/");
	  l++;
	}
	for (i = 0; i < sel->nFiles; i++) {
	  strcat(strcpy(namestr+l, sel->filenames[i]), "\n");
	  buf = addToBuffer(buf, &bufsz, namestr, 0);
	}
	return buf;
    }
}

/**************************************************************************/

String selName(SelPtr sel)
{
    if (sel->nFiles) {
	char name[MAXPATHLEN+1];

	pathname(name, sel->dirname, sel->filenames[0]);
	return NEWSTRING(name);
    } else
	return NULL;
}
