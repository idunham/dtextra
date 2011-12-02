
/**************************************************************************/
/* icons.h: creating and deleting icons                                   */
/**************************************************************************/

#ifndef ICONS_H
#define ICONS_H

#include "xplore.h"

/* built-in icons */

typedef enum _IconNum {
    /* standard file icons */
    ICON_FILE, ICON_FILE_BAD, ICON_FILE_LINK, ICON_FILE_LINK_BAD,
    ICON_DIR, ICON_DIR_BAD, ICON_DIR_UP, ICON_DIR_LINK,
    ICON_EXEC, ICON_EXEC_LINK,
    /* small standard file icons */
    ICON_FILE_SMALL, ICON_FILE_BAD_SMALL, ICON_FILE_LINK_SMALL,
    ICON_FILE_LINK_BAD_SMALL,
    ICON_DIR_SMALL, ICON_DIR_BAD_SMALL, ICON_DIR_UP_SMALL, ICON_DIR_LINK_SMALL,
    ICON_EXEC_SMALL, ICON_EXEC_LINK_SMALL,
    /* misc */
    ICON_FILES, ICON_APPLY, ICON_XPLORE, ICON_XPLORE_LOGO,
    /* number of built-in icons */
    N_ICONS
} IconNum;

typedef struct _IconRec {
    Pixmap pixmap, mask;
    Dimension width, height;
} IconRec, *IconPtr;

extern IconRec std_icons[N_ICONS];

/* dynamically loaded icons */

extern int n_icons;
extern IconPtr *icons;

void init_icons(Display *display, int closeness, String search_path);
/* initialize the icons table; this operation should be invoked before any
   other
   - display: the display on which the icons are to be created
   - closeness: allowable RGB deviation for colors (if ANY, set to maximum
     value)
   - search_path: search path for pixmap files (used in read_icon) */

IconPtr xplore_logo(void);
/* xplore logo (loaded on demand) */

IconPtr read_icon(String name);
/* load the pixmap in file name */

void free_icons(void);
/* dispose all icons; to be invoked when program terminates */

void reinit_icons(void);
/* reinitialize dynamically loaded icons; this keeps a copy of the currently
   loaded icons until refresh_icons is called */

void refresh_icons(void);
/* disposes of previously loaded icons (to be called when the process
   initiated by reinit_icons is finished) */

Pixmap combine_pixmap(IconPtr icon, Pixel background);
/* combine pixmap and shape mask of an icon, setting the background color to
   the specified value; the caller should free the returned pixmap when it
   is no longer needed */

#endif /* ICONS_H */

