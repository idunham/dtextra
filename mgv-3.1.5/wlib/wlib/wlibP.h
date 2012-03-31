/*
 * wlibP.h
 *	Private wlib routines.
 *
 * Copyright (C) 1996  Matthew D. Francey
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	WLIBP_H
#define	WLIBP_H
MU_HID(wlibP_h, "$Mu: wlib/wlib/wlibP.h 1.31 1998/08/30 03:11:06 $")

#include	<stdio.h>

typedef struct {
	String	chapter;
	String	section;
} WLP_HELP;

/*
 * Internal String resources.
 */
enum {
	WlpShelpBrowser,
	WlpShelpPrefix,
	WlpScantGet,
	WlpSemptyLog,
	WlpScantBrowse,
	WlpSnotSupported,
	WlpSbadVersion,
	WlpSunknown
};

CDECLS_BEGIN

/*
 * Remember that "w" might be a Gadget so you should probably use
 * "XtDisplay(wl_top(w))" if you need to get a Display.
 */
typedef	Boolean	(*WL_HELPFN)(Widget w, WLP_HELP *h);

extern	Boolean	wlp_help_html(Widget, WLP_HELP *);
extern	Boolean	wlp_help_remote(Widget, WLP_HELP *);
extern	Boolean	wlp_help_libhelp(Widget, WLP_HELP *);

/*
 * Xpm/Xbm wrappers.
 */
extern	Pixmap	wlp_xpm_from_file(Widget, Display *, Pixel, char *);
extern	Pixmap	wlp_xpm_from_data(Widget, Display *, Pixel, char **);

extern	Pixmap	wlp_xbm_from_file(Widget, Display *, char *);
extern	Pixmap	wlp_xbm_from_data(Widget, Display *, Pixel, Pixel, char *, int, int);

/*
 * misc
 */
extern	void	wlp_fgbg(Widget, Pixel *, Pixel *);
extern	char	*wlp_s(Widget, int);

CDECLS_END

#endif
