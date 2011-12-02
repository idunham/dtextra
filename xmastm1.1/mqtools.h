/*
 *	mtools.h	- This file contains the structures and definitions
 *				  to run motif applications.
 *
 *	RCS:
 *		$Revision$
 *		$Date$
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

#ifndef MQTOOLS_H
#define MQTOOLS_H

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
} MENUITEM;

Widget
	MBuildPulldownMenu _D ((Widget, char *, int, MENUITEM *));
	
#endif /*MQTOOLS_H*/
