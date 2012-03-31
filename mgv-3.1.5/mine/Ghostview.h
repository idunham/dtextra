/*
 * Ghostview.h -- Public header file for Ghostview widget.
 * Copyright (C) 1992  Timothy O. Theisen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * See Ghostview.c for additions by mu@trends.net.
 */
#ifndef	GHOSTVIEW_H
#define	GHOSTVIEW_H
MU_HID(ghostview_h, "$Mu: mgv/mine/Ghostview.h 1.28 1998/08/27 03:40:03 $")

#include <stdio.h>

/****************************************************************
 *
 * Ghostview widget
 *
 ****************************************************************/

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 antialias           Antialias          Boolean         True
 arguments	     Arguments		String		NULL
 background	     Background		Pixel		XtDefaultBackground
 border		     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	1
 bottomMargin	     Margin		Int		0
 busyCursor	     Cursor		Cursor		watch
 cursor		     Cursor		Cursor		crosshair
 destroyCallback     Callback		Pointer		NULL
 filename	     Filename		String		"-"
 foreground	     Foreground 	Pixel		XtDefaultForeground
 gsLocale            GsLocale           String          "POSIX"
 height		     Height		Dimension	0
 interpreter	     Interpreter	String		"gs"
 leftMargin	     Margin		Int		0
 llx		     BoundingBox	Int		0
 lly		     BoundingBox	Int		0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 messageCallback     Callback		Pointer		NULL
 notifyCallback	     Callback		Pointer		NULL
 orientation	     Orientation	PageOrientation	Portrait
 outputCallback      Callback		Pointer		NULL
 palette	     Palette		Palette		Color
 quiet		     Quiet		Boolean		True
 rightMargin	     Margin		Int		0
 safer		     Safer		Boolean		True
 topMargin	     Margin		Int		0
 urx		     BoundingBox	Int		612
 ury		     BoundingBox	Int		792
 useBackingPixmap    UseBackingPixmap	Boolean		True
 width		     Width		Dimension	0
 x		     Position		Position	0
 xdpi		     Resolution		Int		**
 y		     Position		Position	0
 ydpi		     Resolution		Int		**

 ** automatically calculated from display parameters if width and
    height are not set.

 */

/*
 * Resources.
 */
#define	GvNantialias		"antialias"
#define	GvNarguments		"arguments"
#define	GvNbusyCursor		"busyCursor"
#define	GvNnotifyCallback	"notifyCallback"
#define	GvNcursor		"cursor"
#define	GvNfilename		"filename"
#define	GvNgsLocale		"gsLocale"
#define	GvNinterpreter		"interpreter"
#define	GvNleftMargin		"leftMargin"
#define	GvNllx			"llx"
#define	GvNlly			"lly"
#define	GvNmessageCallback	"messageCallback"
#define	GvNoutputCallback	"outputCallback"
#define	GvNpalette		"palette"
#define	GvNquiet		"quiet"
#define	GvNrightMargin		"rightMargin"
#define	GvNsafer		"safer"
#define	GvNtopMargin		"topMargin"
#define	GvNurx			"urx"
#define	GvNury			"ury"
#define	GvNuseBackingPixmap	"useBackingPixmap"
#define	GvNxdpi			"xdpi"
#define	GvNydpi			"ydpi"
#define GvNbottomMargin		"bottomMargin"
#define	GvNorientation		"orientation"

/*
 * Resource classes.
 */
#define	GvCAntialias		"Antialias"
#define	GvCArguments		"Arguments"
#define	GvCBoundingBox		"BoundingBox"
#define	GvCFilename		"Filename"
#define	GvCGsLocale		"GsLocale"
#define	GvCInterpreter		"Interpreter"
#define	GvCPalette		"Palette"
#define	GvCQuiet		"Quiet"
#define	GvCResolution		"Resolution"
#define	GvCSafer		"Safer"
#define	GvCUseBackingPixmap	"UseBackingPixmap"
#define	GvCOrientation		"Orientation"

/*
 * Representation types.
 */
#define	GvRPageOrientation	"PageOrientation"
#define	GvRPalette		"Palette"

/*
 * Misc.
 */
#define	GvEportrait	"portrait"
#define	GvElandscape	"landscape"
#define	GvEupsideDown	"upsidedown"
#define	GvEseascape	"seascape"
#define	GvEmonochrome	"monochrome"
#define	GvEgrayscale	"grayscale"
#define	GvEcolor	"color"

/*
 * GvCvtStringToPageOrientation
 *
 * Number represents clockwise rotation of the paper in degrees
 */
typedef enum {
	GvPageOrientationPortrait	=   0,
	GvPageOrientationLandscape	=  90,
	GvPageOrientationUpsideDown	= 180,
	GvPageOrientationSeascape	= 270
} GvPageOrientation;

typedef enum {
	GvPaletteMonochrome,
	GvPaletteGrayscale,
	GvPaletteColor
} GvPalette;

/*
 * declare specific GhostviewWidget class and instance datatypes
 */
typedef struct _GhostviewClassRec	*GhostviewWidgetClass;
typedef struct _GhostviewRec		*GhostviewWidget;

/*
 * declare the class constant
 */
extern WidgetClass ghostviewWidgetClass;

/*
 * The structure returned by the regular callback
 */
typedef struct {
	XEvent	*event;
	int	width, height;
	int	psx, psy;
	int	xdpi, ydpi;
} GhostviewReturnStruct;

/*
 * Public routines
 */
CDECLS_BEGIN
extern	void	GhostviewEnableInterpreter(Widget);
extern	void	GhostviewDisableInterpreter(Widget);
extern	Boolean	GhostviewIsInterpreterReady(Widget);
extern	Boolean	GhostviewIsInterpreterRunning(Widget);
extern	Pixmap	GhostviewGetBackingPixmap(Widget);
extern	Boolean	GhostviewSendPS(Widget, FILE *, off_t, unsigned, Boolean);
extern	Boolean	GhostviewNextPage(Widget);
extern	void	GhostviewPSBBox(Widget, int, int, int, int, GhostviewReturnStruct *);
extern	void	GhostviewInitialize(Widget);
extern	GvPalette GhostviewGetPalette(Widget);
extern	Boolean GhostviewGetAntiAlias(Widget);
extern	GvPageOrientation	GhostviewGetOrientation(Widget);
extern	int	GhostviewGetXDPI(Widget);
extern	int	GhostviewGetYDPI(Widget);
CDECLS_END

#endif
