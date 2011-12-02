
/*************************************************************************/
/* fileops.h: file operations                                            */
/*************************************************************************/

#ifndef FILEOPS_H
#define FILEOPS_H

#include "xplore.h"
#include "dirs.h"

extern int user_umask;
extern String metachars;

void shellTest(String sh_list, String shell);
/* Determine the type of shell. This should be invoked at startup time.
   Sh_list denotes a list of shells with Bourne-compatible parameter
   passing conventions (i.e. a command invoked with -c receives its first
   parameter as $0, not as $1), or the string "AUTO" in which case a quick-
   and-dirty test is done to verify the shell's parameter passing
   conventions. The shell argument denotes the shell to use. If NULL or
   empty, it defaults to the value of the SHELL environment variable or,
   if not set, to /bin/sh. */

Boolean fileNew(DirPtr dir, String name);
Boolean dirNew(DirPtr dir, String name);
/* Create new file/directory in dir. */

extern Boolean backups;
/* create backups of files to be overwritten */

Boolean fileMove(SelPtr sel, String target, Boolean absolute);
Boolean fileCopy(SelPtr sel, String target, Boolean absolute);
Boolean fileLink(SelPtr sel, String target, Boolean absolute);
/* Move/copy/link selected files to target. The absolute parameter
   determines whether the source file(s) should be identified by absolute
   or relative pathnames w.r.t. target (this only has an impact on fileLink()).
   If target is a relative pathname, it is taken relative to sel; ~ in
   target is expanded. */

Boolean fileDelete(SelPtr sel);
/* Delete selected files. */

String *makeArgv(String dirname, String firstarg, SelPtr sel,
		 Boolean absolute);
/* Create an argument vector for use with fileExec(); if firstarg is not
   NULL and not empty, it is assumed to be an absolute pathname which is used
   as the text of the first argument. The rest of the parameter list is built
   from the list of the selected files (if sel != NULL). If absolute is
   False, arguments are translated to relative pathnames w.r.t. dirname.
   Dirname is taken relative to basedir; if it is NULL or empty, it defaults
   to basedir.) */

extern Boolean echo; /* echo commands */

Boolean fileExec(String command, String dirname, String *args);
/* Execute command using $SHELL -c, with args as parameters. Args should be
   either NULL or terminated with a NULL entry; it is freed automatically
   after the command has been spawned. Dirname denotes the name of the
   directory in which the command is to be invoked (taken relative to basedir;
   ~ expands to the user's home directory). If dirname is NULL, the default
   directory is basedir.
   Return value: False if error invoking the command or command exited
   with nonzero exit status, True otherwise. */

Boolean filePush(DirPtr dir, int i, String pusdir, Boolean absolute,
		 Boolean res_link);
/* Execute the push action of file i in dir, or simply exec the denoted
   file if it is an executable. If present, the action is invoked in the
   directory defined by the push action. Otherwise it runs in the directory
   given by pushdir, if non-NULL, or dir otherwise. In the case of a push
   action the name of the file is passed as the first argument. If absolute
   is True, the name of the file is given by an absolute pathname. Otherwise
   it is specified as a relative pathname  w.r.t. the directory in which the
   command is invoked.

   If the res_link flag is True and the denoted entry is a symlink, the push
   action is carried out on the file pointed to instead of the entry itself. */

Boolean fileMenu(DirPtr dir, int i, int j, Boolean absolute, Boolean res_link);
/* Like filePush(), but invokes the file's jth menu action instead. */

Boolean fileDrop(DirPtr dir, int i, SelPtr sel, Boolean absolute,
		 Boolean dir_is_target, Boolean res_link);
/* Drop the selected files onto file i in dir. If appropriate, the target's
   drop action is executed, with the name of the target file being passed as
   the first parameter; otherwise, if the file is an executable, it is invoked
   with the selected files as parameters. The action is executed in the
   directory defined by the drop action if present. Otherwise the command is
   executed in the target directory, if dir_is_target is True, and in the
   directory given by sel otherwise. As with filePush(), arguments are either
   passed as absolute or relative pathnames, depending on the value of
   absolute. */

Boolean fileApply(SelPtr sel, String command, String dirname,
		  Boolean absolute);
/* Execute the given command on the selected files. Analogous to fileDrop,
   but executes the given command in the given directory (interpreted
   relative to basedir; ~ is expanded, and NULL denotes the default value
   basedir), instead of invoking a file's drop action. */

#endif /* FILEOPS_H */
