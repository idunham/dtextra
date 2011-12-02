/*
 *	mtools.h	- This file contains the structures and definitions
 *				  to run motif applications.
 *
 *	RCS:
 *		$Revision: 1.1 $
 *		$Date: 1995/05/07 19:34:10 $
 *
 *	Security:
 *		Unclassified
 *
 *	Description:
 *		This include file contains the structures and definitions
 *		needed to be able to use the functions that standardize 
 *		creation of some motif widgets.
 *
 *	Dependencies:
 *		None
 *
 *	Comments:
 *		None	
 *
 *	Development History:
 *		when		who	why
 */

#ifndef MENU_H
#define MENU_H

/*
** according to dan's book
*/

typedef struct _menu_item
{
	char		*label;			/* the label for the item */
	WidgetClass	*class;			/* pushbutton, label, separator, ... */
	char		mnemonic;		/* mnemonic: NULL if none */
	char		*accelerator;	/* accelerator; NULL if none */
	char		*accel_text;	/* to be converted to compound string */
	void		(*callback)();	/* routine to call; NULL if none */
	XtPointer	callback_data;	/* client_data for callback() */
	struct _menu_item *subitems; /* pullright menu items,if not NULL */
	int			sensitiv;		/* if the item is sensitive mm 11/14/93*/
	int			index;			/* index of menu items */
    int         toggle_set;     /* 0 or 1 for xmToggleButton..*/
} MenuItem;

Widget
	MBuildPulldownMenu _Declare ((Widget, char *, int, MenuItem *));
	
#endif /*MENU_H*/
