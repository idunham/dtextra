
/*************************************************************************/
/* dirs.h: directory data structure                                      */
/*************************************************************************/

#ifndef DIRS_H
#define DIRS_H

#include "xplore.h"
#include "icons.h"

/* option flags for directories: */

#define CHECK_FILES     (1L<<0)   /* check dir contents to detect mods */
#define SORT_BY_NAME    (1L<<1)   /* sort files by name */
#define SORT_BY_SIZE    (1L<<2)   /* sort files by size */
#define SORT_BY_DATE    (1L<<3)   /* sort files by date */
#define INCLUDE_UPDIR   (1L<<4)   /* include .. */
#define INCLUDE_HIDDEN  (1L<<5)   /* include hidden files */
#define INCLUDE_FILES   (1L<<6)   /* include non-directory files */
#define INCLUDE_DIRS    (1L<<7)   /* include directories */
#define REVERSE         (1L<<8)   /* sort files in reverse order */
#define DIRS_FIRST      (1L<<9)   /* list dirs first */
#define NOMAGIC         (1L<<10)  /* don't use magic headers to determine file
				     types */
#define NOMOUNT         (1L<<11)  /* don't mount device */
#define MODIFIED        (1L<<12)  /* assume dir is modified */

/* mask for sort options: */
#define SORT_MASK       (SORT_BY_NAME|SORT_BY_SIZE|SORT_BY_DATE)

/* data structures: */

typedef struct stat StatRec, *StatPtr;

/* directories: */

typedef struct _DirRec {
    String name, getwd;
    int dev;
    StatRec stats, lstats;
    String filter;
    int options;
    Boolean isReadable, hasSubdirs, hasSubdirInfo, hasFileInfo;
    int nSubdirs, nBytes, nFiles, nBytesSelected, nFilesSelected;
    int type;
    IconPtr large_icon, small_icon;
    struct _DirRec *parent;
    struct _DirRec **subdirs;
    struct _FileRec **files;
} DirRec, *DirPtr, **DirList;

/* files: */

typedef struct _FileRec {
    String name;
    StatRec stats, lstats;
    Boolean selected;
    int type;
    IconPtr large_icon, small_icon;
} FileRec, *FilePtr, **FileList;

/* selection lists: */

typedef struct _SelRec {
    String dirname;
    int nFiles;
    String *filenames;
} SelRec, *SelPtr;

/* directory ops: ********************************************************/

/* creation/deletion: */

DirPtr dirCreate(String name, String filter, int options);
/* Create a directory structure:
   - name: pathname of directory (~ is expanded)
   - filter: either NULL or a string consisting of patterns to be matched
   - options: default options to use for the directory (this setting can
     be overridden by a device entry)
   - return value: pointer to directory structure
   This operation merely creates the directory structure and checks whether
   it is readable, but does not read any subdirectory or files information.
   For this purpose the operations dirReadSubdirInfo() and dirReadFileInfo()
   must be used, see below.
   If the directory is readable, dirIsReadable(dir) is set to True. Otherwise
   an appropriate error code is passed to error.c and dirIsReadable(dir) is
   set to False. */

DirPtr dirFree(DirPtr dir);
/* free directory and all its components, return NULL */

/* modification ops (return given DirPtr if successful, NULL otherwise) */

DirPtr dirReadSubdirInfo(DirPtr dir);
/* (Re)Read the directory's subdirectory information. In case of an error
   condition an appropriate error code is passed to error.c and
   dirReadable(dir) is set to False. Otherwise dirHasSubdirInfo(dir) is set
   to True. Return value: dir (unchanged) if operation successful, NULL
   otherwise. */

DirPtr dirUpdateSubdirInfo(DirPtr dir);
/* Like dirReadSubdirInfo, but only reread information for subdirectories
   which have been modified */

DirPtr dirReadFileInfo(DirPtr dir);
/* (Re)Read the directory's file information. In case of an error
   condition an appropriate error code is passed to error.c and
   dirReadable(dir) is set to False. Otherwise dirHasFileInfo(dir) is set to
   True. Return value: dir (unchanged) if operation successful, NULL
   otherwise. */

DirPtr dirFreeSubdirInfo(DirPtr dir);
/* Dispose the subdirectory information associated with a directory structure,
   return dir. */

DirPtr dirFreeFileInfo(DirPtr dir);
/* Dispose files information in a directory structure, return dir. */

DirPtr dirReread(DirPtr dir);
/* reread directory info */

DirPtr dirUnmountAll(DirPtr dir);
/* unmount all devices mounted on directories in dir */

DirPtr dirRestat(DirPtr dir);
/* reread directory info after all devices have been unmounted */

DirPtr dirUpdate(DirPtr dir);
/* reread directory information if directory has been modified */

DirPtr dirSetFilter(DirPtr dir, String filter);
/* filter directory */

DirPtr dirSetOptions(DirPtr dir, int options);
/* set the given option flags and update directory information accordingly */

DirPtr dirClearOptions(DirPtr dir, int options);
/* clear the specified option flags and update directory information
   accordingly */

DirPtr dirSelect(DirPtr dir, int i, String pattern);
/* select file i in directory (all files if i=ALL, or all matching files
   if pattern != NULL) */

DirPtr dirDeselect(DirPtr dir, int i, String pattern);
/* deselect file(s) in directory */

DirPtr dirInvert(DirPtr dir, int i, String pattern);
/* invert selection status of specified file(s) */

/* value ops: */

/* the following items can be accessed for any directory structure: */

String dirName(DirPtr dir);
/* name of directory */

String dirFilter(DirPtr dir);
/* return current filter pattern */

int dirOptions(DirPtr dir);
/* return current option settings */

DirPtr dirParent(DirPtr dir);
/* parent directory (NULL if none) */

DirPtr dirTop(DirPtr dir);
/* topmost directory containing dir as a descendant */

DirPtr dirFindSubdir(DirPtr dir, String dirname, DirPtr *ret);
/* Find subdirectory given by absolute path dirname in dir and set *ret to
   the deepest subdirectory in dir which is an ancestor of dirname (NULL
   if none). Returns the appropriate directory structure if dirname is found
   in dir and NULL otherwise. If ret is NULL it is ignored. */

DirPtr dirExpandSubdir(DirPtr dir, String dirname, DirPtr *ret);
/* Like dirFindSubdir(), but also expand subdirectories on the path to
   dirname as needed. */

int dirType(DirPtr dir);
/* type of directory (NONE if not readable) */

IconPtr dirLargeIcon(DirPtr dir);
/* large icon for directory */

IconPtr dirSmallIcon(DirPtr dir);
/* small icon for directory */

Boolean dirIsReadable(DirPtr dir);
/* check whether directory is readable */

Boolean dirHasSubdirs(DirPtr dir);
/* check whether directory is readable and has subdirectories */

Boolean dirHasSubdirInfo(DirPtr dir);
/* check whether subdirectory information has been generated */

Boolean dirHasFileInfo(DirPtr dir);
/* check whether file information has been generated */

Boolean dirModified(DirPtr dir);
/* check whether directory has been modified (i.e. time stamps have changed) */

/* the following items can only be accessed when the directory is readable
   (i.e. dirIsReadable(dir) == True): */

String dirGetwd(DirPtr dir);
/* normalized name (as returned by getwd()) */

int dirDev(DirPtr dir);
/* device as returned by devmount */

StatPtr dirStats(DirPtr dir);
StatPtr dirLstats(DirPtr dir);
/* stats of directory when last read */

/* for the following items there will be zero entries unless the directory
   is readable and subdirectory or file information has been read,
   respectively: */

int dirNSubdirs(DirPtr dir);
/* number of subdirectories in directory */

int dirNFiles(DirPtr dir);
/* number of files in directory */

int dirNBytes(DirPtr dir);
/* total byte size of files in directory */

int dirNFilesSelected(DirPtr dir);
/* number of selected files in directory */

int dirNBytesSelected(DirPtr dir);
/* total byte size of selected files in directory */

DirPtr dirSubdir(DirPtr dir, int i);
/* subdir no. i, 0<=i<dirNSubdirs(dir) */

FilePtr dirFile(DirPtr dir, int i);
/* file no. i, 0<=i<dirNFiles(dir) */

/* file ops: *************************************************************/

/* value ops: */

String fileName(FilePtr file);
/* name of file */

StatPtr fileStats(FilePtr file);
StatPtr fileLstats(FilePtr file);
/* stats of file when directory was last read */

Boolean fileSelected(FilePtr file);
/* selection status of file */

int fileType(FilePtr file);
/* type of file */

IconPtr fileLargeIcon(FilePtr file);
IconPtr fileSmallIcon(FilePtr file);
/* icons for file */

/* selection ops: ********************************************************/

/* creation/deletion: */

SelPtr selFromDir(DirPtr dir);
/* retrieve the selections from dir */

SelPtr selFromBuffer(String buf);
/* Make a selection list from a string generated with selBuffer(). */

SelPtr selFromName(String name);
/* Make a selection list from a single file given by name (if name is
   not absolute it is interpreted as relative to basedir). */

SelPtr selFree(SelPtr sel);
/* free all data allocated in sel, return NULL */

/* value ops: */

String selDirname(SelPtr sel);
/* return the directory name for the selection */

int selNFiles(SelPtr sel);
/* return the number of selected files */

String selFilename(SelPtr sel, int i);
/* return the name of the ith selected file (0 <= i < selNFiles(sel)) */

String selBuffer(SelPtr sel);
/* create a string representing the filenames in a selection list which is
   suitable for transfer via drag and drop (the string is allocated
   dynamically and freeing it is the responsibility of the caller) */

String uriBuffer(SelPtr sel);
/* same as selBuffer, but the string is created in the standard text/uri-list
   format used by the xdnd protocol (KDE, GNOME) */

String selName(SelPtr sel);
/* return the absolute filename of the first selection in sel (the string
   is allocated dynamically and freeing it is the responsibility of the
   caller) */

#endif /* DIRS_H */
