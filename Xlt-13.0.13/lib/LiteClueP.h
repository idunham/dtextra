/* 
LiteClueP.h - Private definitions for LiteClue widget
	See LiteClue documentation

Copyright 1996 COMPUTER GENERATION, INC.,

The software is provided "as is", without warranty of any kind, express
or implied, including but not limited to the warranties of
merchantability, fitness for a particular purpose and noninfringement.
In no event shall Computer Generation, inc. nor the author be liable for
any claim, damages or other liability, whether in an action of contract,
tort or otherwise, arising from, out of or in connection with the
software or the use or other dealings in the software.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.

Author:
Gary Aviv 
Computer Generation, Inc.,
gary@compgen.com

*/
/* Revision History:
$Log: LiteClueP.h,v $
Revision 1.1  2005/04/18 15:34:52  dannybackx
Add the old version of LiteClue to keep track of the original verson.
Michel will be updating this soon.


$log
$log
*/

#ifndef _DEF_LiteClueP_h
#define _DEF_LiteClueP_h

#include <X11/ShellP.h>
/* Include public header file for this widget. */
#include <LiteClue.h>

/* Doubly Linked List Processing */
struct list_thread_str
{
	struct list_thread_str * forw;	/* next pointer */
	struct list_thread_str * back;	/* prev pointer */
};
typedef struct list_thread_str  ListThread; 


typedef struct {
	int	nothing;	/* place holder */
} LiteClueClassPart;

/* Full class record declaration */
typedef struct _LiteClueClassRec {
	CoreClassPart	core_class;
	CompositeClassPart  composite_class;
	ShellClassPart  shell_class;
	OverrideShellClassPart  override_shell_class;
	LiteClueClassPart	LiteClue_class;
} LiteClueClassRec;

extern LiteClueClassRec xcgLiteClueClassRec;

/* New fields for the LiteClue widget record */
typedef struct {
	/* resources */
	Pixel foreground;
	XFontSet fontset;	/* the font for text in box */
	/* -------- private state --------- */
	ListThread widget_list; 	/* list of widgets we are liteClue-ing */
	Dimension font_width;	/* width of '1' character */
	Dimension font_height;	/* height of font, rows are spaced using this */
	Dimension font_baseline;	/* relative displacement to baseline from top */
	GC text_GC;		/* for drawing text */
} LiteCluePart;


/*
 * Full instance record declaration
 */
typedef struct _LiteClueRec {
	CorePart		core;
	CompositePart  composite;
	ShellPart 	shell;
	OverrideShellPart override;
	LiteCluePart	liteClue;
} LiteClueRec;


#endif /* _DEF_LiteClueP_h */

