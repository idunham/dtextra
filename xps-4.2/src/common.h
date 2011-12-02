/* Prototypes and external variables in common.c */
/* $Id: common.h,v 1.2 2003/04/24 00:21:29 rockyb Exp $ */
#ifndef COMMON_H
#define COMMON_H

#include "user_config.h"
#include "xpstypes.h"

/* Give status in message area. */
extern void    updateMessage(void);

/* Draw the process forest/tree. */
extern void    draw(Display *display, Window window);

/* 
   Find uid in base.uid[] and return the index of this. 
   If not in table return the index of a free index. 
   If the table is full and uid is not found then return BOGUS_UID.
*/
extern user_color_index find_uid_color_index(uid_t uid) ;


extern Pixel   GetUID_Color (uid_t uid);

/* Figure out which node was selected with the mouse and
  ask user to confirm that the corresponding process is to be killed.
  Taken:       mouse coordinates
  Returned:    True if clicked on a node (and update pid arg),
               or False if no node was found at the mouse position.
 */
extern Boolean find_pid_under_mouse(Display *display, Window window, 
				    int x, int y, pid_t *pid);

/* Top-level routine to gather process information, sort processes
   layout tree and cause it to be displayed. */
extern void display(
     Boolean forceRedisplay,
     Display *display,
     Window window);

/* Print usage help and exit */
extern void usage(int exitcode);

/* Print version number and exit */
extern void show_version(void);

/* Initializations common to all platforms */
extern void common_init(int argc, char *argv[]);

#ifndef HAVE_IDTOUSER
extern char * IDtouser(uid_t uid);
#endif

/* 
   Get uid from user name "name." 
   Returns True if found, and sets uid. 
*/
extern Boolean usertoID(const char *name, /*out*/ uid_t *uid );

#ifndef MIN
#define MIN(a,b) ((a<b)?a:b)
#endif
#ifndef MAX
#define MAX(a,b) ((a>b)?a:b)
#endif

/* Parameters concerning how this program was built. */
extern char config_info[];

/* Options set on the command line. */
typedef struct {
  Boolean      version;           /* give version and exit?            */
  Boolean      help;              /* give usage help and exit?         */
  Boolean      me_too;            /* show this program as running?     */
} cmd_opt;

static cmd_opt cmd_options;

#endif /*COMMON_H*/
