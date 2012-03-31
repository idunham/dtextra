/*
 * GhostviewP.h -- Private header file for Ghostview widget.
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
#ifndef GHOSTVIEWP_h
#define GHOSTVIEWP_h
MU_HID(ghostviewp_h, "$Mu: mgv/mine/GhostviewP.h 1.22 1998/08/27 03:40:03 $")

#include <mine/Ghostview.h>

typedef struct {
	XtPointer	extension;
} GhostviewClassPart;

typedef struct _GhostviewClassRec {
	CoreClassPart		core_class;
	GhostviewClassPart	ghostview_class;
} GhostviewClassRec;

extern GhostviewClassRec ghostviewClassRec;

/*
 * structure to describe section of file to send to ghostscript
 */
typedef struct record_list_s GVFile;
struct record_list_s {
	FILE		*fp;
	off_t		begin;
	unsigned	len;
	Boolean		seek_needed;
	Boolean		close;
	GVFile		*next;
};

typedef struct {
	/*
	 * resources
	 */
	Pixel			foreground;
	Cursor			cursor;
	Cursor			busy_cursor;
	XtCallbackList		notify_callback;
	XtCallbackList		message_callback;
	XtCallbackList		output_callback;
	String			interpreter;
	String			arguments;
	String			filename;
	Boolean			quiet;
	Boolean			safer;
	Boolean			use_bpixmap;
	GvPageOrientation	orientation;
	GvPalette		palette;
	int			xdpi, ydpi;
	int			llx, lly, urx, ury;
	int			left_margin, right_margin;
	int			bottom_margin, top_margin;
	String			gs_locale;
	Boolean			antialias;

	/*
	 * private state
	 */
	GC		gc;			/* GC used to clear window    */
	Window		mwin;			/* destination of gs messages */
	Boolean		disable_start;		/* whether to fork gs         */
	int		interpreter_pid;	/* pid of gs, -1 if none      */
	GVFile		*ps_input;		/* pointer it gs input queue  */
	char		*input_buffer;		/* pointer to input buffer    */
	unsigned	bytes_left;		/* bytes left in section      */
	char		*input_buffer_ptr;	/* pointer into input buffer  */
	unsigned	buffer_bytes_left;	/* bytes left in buffer       */
	int		interpreter_input;	/* fd gs stdin, -1 if None    */
	XtInputId	interpreter_input_id;	/* XtInputId for above        */
	int		interpreter_output;	/* fd gs stdout, -1 if None   */
	XtInputId	interpreter_output_id;	/* XtInputId for above        */
	int		interpreter_error;	/* fd gs stderr, -1 if None   */
	XtInputId	interpreter_error_id;	/* XtInputId for above        */
	Dimension	gs_width;		/* last width of window       */
	Dimension	gs_height;		/* last height of window      */
	Boolean		busy;			/* Is gs busy drawing?        */
	Boolean		changed;		/* changes since last Setup() */

	Atom	ghostview;
	Atom	gv_colors;
	Atom	next;
	Atom	page;
	Atom	done;
} GhostviewPart;

typedef struct _GhostviewRec {
	CorePart	core;
	GhostviewPart	ghostview;
} GhostviewRec;

#endif
