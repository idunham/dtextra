
#include "xplore.h"
#include "icons.h"

#include "util.h"

#include "file.xpm"
#include "file_bad.xpm"
#include "file_link.xpm"
#include "file_link_bad.xpm"
#include "folder.xpm"
#include "folder_bad.xpm"
#include "folder_up.xpm"
#include "folder_link.xpm"
#include "app.xpm"
#include "app_link.xpm"

#include "file_small.xpm"
#include "file_bad_small.xpm"
#include "file_link_small.xpm"
#include "file_link_bad_small.xpm"
#include "folder_small.xpm"
#include "folder_bad_small.xpm"
#include "folder_up_small.xpm"
#include "folder_link_small.xpm"
#include "app_small.xpm"
#include "app_link_small.xpm"

#include "files.xpm"
#include "exec.xpm"

#define CLOSENESS_MAX 65535

IconRec std_icons[N_ICONS];
int n_icons = 0;
IconPtr *icons = NULL;

static char search_path[MAXPATHLEN+1];
static Display *display;
int closeness;

static char *names[] = {
    "file.xpm", "file_bad.xpm", "file_link.xpm", "file_link_bad.xpm",
    "folder.xpm", "folder_bad.xpm", "folder_up.xpm", "folder_link.xpm",
    "app.xpm", "app_link.xpm",

    "file_small.xpm", "file_bad_small.xpm", "file_link_small.xpm",
    "file_link_bad_small.xpm",
    "folder_small.xpm", "folder_bad_small.xpm", "folder_up_small.xpm",
    "folder_link_small.xpm",
    "app_small.xpm", "app_link_small.xpm",

    "files.xpm", "exec.xpm", "folder.xpm", NULL,
};

static char **pixmaps[] = {
    file_xpm, file_bad_xpm, file_link_xpm, file_link_bad_xpm,
    folder_xpm, folder_bad_xpm, folder_up_xpm, folder_link_xpm,
    app_xpm, app_link_xpm,

    file_small_xpm, file_bad_small_xpm, file_link_small_xpm,
    file_link_bad_small_xpm,
    folder_small_xpm, folder_bad_small_xpm, folder_up_small_xpm,
    folder_link_small_xpm,
    app_small_xpm, app_link_small_xpm,

    files_xpm, exec_xpm, folder_xpm, NULL,
};

void init_icons(Display *display0, int closeness0, String search_path0)
{
    Window win;
    XpmAttributes xpm_attr;
    int i;
    char fullname[MAXPATHLEN+1];

    display = display0;
    closeness = (closeness0 == ANY)?CLOSENESS_MAX:closeness0;
    win = DefaultRootWindow(display);
    xpm_attr.valuemask = XpmReturnPixels | XpmCloseness;
    xpm_attr.closeness = closeness;
    strcpy(search_path, search_path0);
    /* ICON_XPLORE_LOGO is read on demand */
    for (i = 0; i < N_ICONS-1; i++)
      if (searchpath(fullname, search_path, names[i]) &&
	  XpmReadFileToPixmap(display, win, fullname,
			      &std_icons[i].pixmap, &std_icons[i].mask,
			      &xpm_attr) == XpmSuccess) {
	std_icons[i].width = xpm_attr.width;
	std_icons[i].height = xpm_attr.height;
      } else {
	XpmCreatePixmapFromData(display, win, pixmaps[i], &std_icons[i].pixmap,
				&std_icons[i].mask, &xpm_attr);
	std_icons[i].width = xpm_attr.width;
	std_icons[i].height = xpm_attr.height;
      }
}

IconPtr xplore_logo(void)
{
  static Boolean xplore_logo_read = False, xplore_logo_success = False;
  int i = ICON_XPLORE_LOGO;

  if (!xplore_logo_read) {
    char fullname[MAXPATHLEN+1];
    Window win;
    XpmAttributes xpm_attr;

    xplore_logo_read = True;
    win = DefaultRootWindow(display);
    xpm_attr.valuemask = XpmReturnPixels | XpmCloseness;
    xpm_attr.closeness = closeness;
    if (searchpath(fullname, search_path, "xplore_logo.xpm") &&
	XpmReadFileToPixmap(display, win, fullname, &std_icons[i].pixmap,
			    &std_icons[i].mask, &xpm_attr) ==
	XpmSuccess) {
      std_icons[i].width = xpm_attr.width;
      std_icons[i].height = xpm_attr.height;
      xplore_logo_success = True;
    } else
      xplore_logo_success = False;
  }
  if (xplore_logo_success)
    return &std_icons[i];
  else
    return NULL;
}

IconPtr read_icon(String name)
{
    IconPtr icon = (IconPtr) MALLOC(sizeof(IconRec));
    char fullname[MAXPATHLEN+1];
    Window win;
    XpmAttributes xpm_attr;

    win = DefaultRootWindow(display);
    xpm_attr.valuemask = XpmReturnPixels | XpmCloseness;
    xpm_attr.closeness = closeness;
    if (searchpath(fullname, search_path, name) &&
	XpmReadFileToPixmap(display, win, fullname,
			    &icon->pixmap, &icon->mask, &xpm_attr) ==
	XpmSuccess) {
	icon->width = xpm_attr.width;
	icon->height = xpm_attr.height;
	icons = (IconPtr*) REALLOC(icons, (n_icons+1)*sizeof(IconPtr));
	icons[n_icons++] = icon;
	return icon;
    } else {
	FREE(icon);
	return NULL;
    }
}

void free_icons(void)
{
    int i;

    for (i = 0; i < N_ICONS; i++) {
	if (std_icons[i].pixmap) XFreePixmap(display, std_icons[i].pixmap);
	if (std_icons[i].mask) XFreePixmap(display, std_icons[i].mask);
    }
    for (i = 0; i < n_icons; i++) {
      if (icons[i]->pixmap) XFreePixmap(display, icons[i]->pixmap);
      if (icons[i]->mask) XFreePixmap(display, icons[i]->mask);
      FREE(icons[i]);
    }
    FREE(icons);
    icons = NULL;
    n_icons = 0;
}

static int n_old_icons = 0;
static IconPtr *old_icons = NULL;

void reinit_icons(void)
{
  if (old_icons) refresh_icons();
  n_old_icons = n_icons;
  old_icons = icons;
  icons = NULL;
  n_icons = 0;
}

void refresh_icons(void)
{
  int i;
  
  for (i = 0; i < n_old_icons; i++) {
    if (old_icons[i]->pixmap) XFreePixmap(display, old_icons[i]->pixmap);
    if (old_icons[i]->mask) XFreePixmap(display, old_icons[i]->mask);
    FREE(old_icons[i]);
  }
  FREE(old_icons);
  old_icons = NULL;
  n_old_icons = 0;
}

Pixmap combine_pixmap(IconPtr icon, Pixel background)
{
  Pixmap pixmap;
  char **data;
  Window win = DefaultRootWindow(display);
  XpmAttributes xpm_attr;
  static XpmColorSymbol none_color = { NULL, "None", (Pixel)0 };

  XpmCreateDataFromPixmap(display, &data, icon->pixmap, icon->mask, NULL);
  xpm_attr.valuemask = XpmReturnPixels | XpmColorSymbols;
  xpm_attr.colorsymbols = &none_color;
  xpm_attr.numsymbols = 1;
  none_color.pixel = background;
  XpmCreatePixmapFromData(display, win, data, &pixmap, NULL, &xpm_attr);
  XpmFree(data);
  return pixmap;
}
