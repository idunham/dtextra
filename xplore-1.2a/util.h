
/****************************************************************************/
/* util.h: utility functions                                                */
/****************************************************************************/

#ifndef UTIL_H
#define UTIL_H

#include "xplore.h"

int mysetenv(String name, String val, int overw /* ignored */);
/* set an environment variable */

String split(String s, char c);
/* split a string into substrings delimited by a given character */

String split2(String s, char c);
/* like split, but handles embedded escape characters (\); if an occurrence
   of the delimiter is prefixed with \, it is treated like an ordinary
   character */

String split_type(String *s);
/* split a type pattern string into magic and file pattern components */

String quote(String s, String t, String c);
/* escape occurrences in t of characters in c with \; return result in s */

String unquote(String s, String t);
/* remove \ from characters in c and replace C-like escapes (\{n,r,t,b,f});
   return result in s */

Boolean prefix(String s, String t);
/* return True iff s is a prefix of t */

Boolean suffix(String s, String t);
/* return True iff s is a suffix of t */

String fnexpand(String result, String name);
/* expand ~ in filenames; this requires that the HOME environment variable
   is set properly */

String pathname(String result, String dirname, String filename);
/* construct a full pathname from the given directory and file name;
   return the result in result */

String abspath(String result, String basename, String filename);
/* translate filename to an absolute pathname; ~ in filename is expanded,
   and if the resulting pathname is still relative, basename is prepended */

String relpath(String result, String basename, String pathname);
/* translate a pathname to a relative pathname w.r.t. basename; if pathname
   is already relative (i.e. it doesn't start with a /), it is returned
   unchanged */

String shortestpath(String result, String pathname);
/* return the shortest path equivalent to pathname (remove . and ..) */

String resolve(String result, String pathname);
/* like shortestpath, but requires that pathname is pathname and resolves
   links */

String dirpart(String result, String pathname);
/* return the directory part of pathname */

String filepart(String result, String pathname);
/* return the filename part of pathname */

String bakname(String result, String pathname);
/* create a unique numbered backup file name for the given pathname (a la
   GNU mv -b -V numbered) */

Boolean exists(char *name);
/* check whether a file exists */

Boolean identical(char *name1, char *name2);
/* check whether two files are identical (refer to the same inode) */

String searchpath(String result, String path, String name);
/* Search path for file name. A ~ in path is expanded. If name is absolute
   it is returned unchanged. Otherwise the absolute name is returned in
   result. If name is not found on path, NULL is returned. */

Boolean fnmatch(String pattern, String fn);
/* match a space-delimited list of patterns with a filename, returning True
   if the match was successful; allowed wildcards: ?, *, and [...]
   (character classes); \ can be used to escape wildcard characters */

Boolean fnmatch1(String pattern, String fn);
/* match against a single pattern */

Boolean fnxmatch(String pattern, String fn);
/* like fnmatch, but only finds exact matches against the list of patterns
   (i.e. no wildcards are recognized) */

Boolean fnmatchnodot(String pattern, String fn);
/* variant of fnmatch which matches the . and .. dirs only if specified
   explicitly; equivalent to fnmatch if fn != ".", "..", and to fnxmatch
   otherwise */

String resname(String result, String label);
/* translate a label to a valid resource name by mapping blanks to
   underscores and discarding all remaining characters which are not allowed
   in a resource name */

#endif /* UTIL_H */

