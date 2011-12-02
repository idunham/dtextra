
/* Derived from Robert Gasch's xfsm utility (available from ftp.x.org), which
   is also distributed under the GPL (see copyright notice below). */

/* ************************************************************************* *
   xfsm - (C) Copyright 1993-1996 Robert Gasch (Robert_Gasch@peoplesoft.com)

   Permission to use, copy, modify and distribute this software for any 
   purpose and without fee is hereby granted, provided that this copyright
   notice appear in all copies as well as supporting documentation. All
   work developed as a consequence of the use of this program should duly
   acknowledge such use.

   No representations are made about the suitability of this software for
   any purpose. This software is provided "as is" without express or implied 
   warranty.

   See the GNU General Public Licence for more information.
 * ************************************************************************* */

#ifndef _MNTTABLE_H
#define _MNTTABLE_H

int get_mnt_table(void);
/* Get the table of mounted devices (returns nonzero unless some error
   occurred). */

int check_mnt_table(void);
/* Returns nonzero iff the mount table needs updating. This is done by
   looking at the time stamp of the /etc/mtab file (or the like), which
   doesn't work on some systems because they have the mount table info
   in some special database or in memory (e.g., AIX, ULTRIX, OSF1). In such
   cases check_mnt_table() always returns 1. This also happens if, for
   some reason, we cannot stat the mtab file. */

int n_mnt_entries(void);
/* Return the number of entries in the mount table */

char *mnt_fsname(int i);
char *mnt_devname(int i);
/* Return the file system and device name for the ith entry (0 <= i <
   n_mnt_entries()), respectively. */

int search_mnt_table(char *fsname);
/* Search for the entry for the filesystem named fsname. Returns the index
   in the table, or -1 if no matching entry is found. */

#endif /* _MNT_TABLE_H */
