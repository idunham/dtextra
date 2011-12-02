/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	XbrList.c
 * CREATED:	Wed Oct 27  1993
 * AUTHOR:	Rik Turnbull
 * DESCRIPTION:	Routines for handling the Motif XmList widget.
 *
 */

/*
 * Mon Aug 15 1994
 * Added XbrListSelectionFree() function.
 *
 */

#include <Xm/Xm.h>
#include <Xm/List.h>

#include "XbrList.h"

#if (XmVERSION < 1) || ((XmVERSION == 1) && (XmREVISION < 2))
#define XmStringCreateLocalized(string) XmStringCreateLtoR(string,XmSTRING_DEFAULT_CHARSET)
#endif

void XbrListClear(Widget);
void XbrListAddItem(Widget, char *, int);
char **XbrListGetSelection(Widget);
void XbrListSelectionFree(char **);
int XbrListItemSelected(Widget, int *);
void XbrListTop(Widget);
int XbrListNumItems(Widget);

/* XbrListClear:**************************************************************/
/* XbrListClear: In Motif1.1 if you do not set the list to the top before    */
/* XbrListClear: deleting all the items it core dumps! Don't know about      */
/* XbrListClear: Motif1.2.                                                   */
/* XbrListClear:**************************************************************/

void XbrListClear(Widget w)
{
    /* To get round a bug in Motif1.1 */
    XmListSetPos(w, 1);
    XmListDeleteAllItems(w);
}

/* XbrListAddItem:************************************************************/
/* XbrListAddItem: Adds an unselected string to the list widget              */
/* XbrListAddItem:************************************************************/

void XbrListAddItem(Widget w, char *item, int show)
{
    XmString MyString;

    MyString = XmStringCreateLocalized(item);
    XmListAddItemUnselected(w, MyString, 0);
    XmStringFree(MyString);

    if(show == XbrLIST_SHOWTOP)
        XmListSetPos(w, 1);
    else if(show == XbrLIST_SHOWBOTTOM)
        XmListSetPos(w, 0);
}

/* XbrListGetSelection:*******************************************************/
/* XbrListGetSelection: Returns the selectItems resource of a  list. This is */
/* XbrListGetSelection: because it must be converted into string format from */
/* XbrListGetSelection: XString format.The calling routine is responsible    */
/* XbrListGetSelection: for freeing memory.                                  */
/* XbrListGetSelection:*******************************************************/

char **XbrListGetSelection(Widget w)
{
    XmStringTable items;
    char *item, **selection = NULL;
    int count, i, j;

    /* Get the number of items selected */
    XtVaGetValues(w, XmNselectedItemCount, &count, NULL);

    /* Make sure we have more than one! */
    if(count <= 0)
        return(NULL);

    /* Allocate memory for the items */
    if((selection = (char **) malloc(sizeof(char *) * (count + 1))) == NULL)
        return(NULL);

    /* Get the selectedItems resource - an array of XmStrings! */
    XtVaGetValues(w, XmNselectedItems, &items, NULL);

    /* For each one convert it to (char *) format allocating memory as we
       go
    */
    for(i = 0; i < count; i++) {
        XmStringGetLtoR(items[i], XmSTRING_DEFAULT_CHARSET, &item);
        if((selection[i] = (char *) malloc(strlen(item) + 1)) == NULL) {
            /* Messed up - must free up all memory */
            for(j = 0; selection[j]; j++)
                free(selection[j]);
            free(selection);
            return(NULL);
        }
        strcpy(selection[i], item);
    }

    selection[i] = NULL;

    return(selection);
}

/*----------------------------------------------------------------------------
  XbrListSelectionFree()
  This frees up the memory previously alloced by XbrListGetSelection(). It
  basically frees up an array of strings - maybe I should put it in the
  commonlib and make this a #define?
  ----------------------------------------------------------------------------*/
void XbrListSelectionFree(char **selections)
{
    int i;

    for(i = 0; selections[i] != NULL; i++)
        free(selections[i]);
    free(selections);
}

/* XbrListItemSelected:*******************************************************/
/* XbrListItemSelected: Returns true or false if an item is selected in the  */
/* XbrListItemSelected: given list widget.                                   */
/* XbrListItemSelected:*******************************************************/

/*----------------------------------------------------------------------------
  XbrListItemSelected()
  Returns the number of words selected in the XmList. If 'length' is set we
  also return the number of characters selected.
  
  Widget w	XmList to check
  int *length	Where to save number of characters
  ----------------------------------------------------------------------------*/
int XbrListItemSelected(Widget w, int *length)
{
    int count = 0, numchars = 0;
    char **selections;

    /* If we are only interested in the number of words, do it the easy way. */
    if(length == NULL) {
        /* Get the number of items selected */
        XtVaGetValues(w, XmNselectedItemCount, &count, NULL);
    } else {
	if((selections = XbrListGetSelection(w)) != NULL) {
	    for(count = 0; selections[count] != NULL; count++)
		numchars += strlen(selections[count]);
	}
	*length += numchars;
    }
    
    return(count);
}

/* XbrListTop:****************************************************************/
/* XbrListTop: For a list inside a scrolled window, scroll the scroll bar to */
/* XbrListTop: the top.                                                      */
/* XbrListTop:****************************************************************/

void XbrListTop(Widget w)
{
    Widget scrollbar;
    Arg args[10];
    Cardinal n;
    int value_return, slider_size_return, increment_return;
    int page_increment_return, minimum;

    n = 0;
    XtSetArg(args[n], XmNhorizontalScrollBar, &scrollbar);
    XtGetValues(XtParent(w), args, n);

    n = 0;
    XtSetArg(args[n], XmNminimum, &minimum);
    XtGetValues(XtParent(w), args, n);

    XmScrollBarGetValues(scrollbar, &value_return, &slider_size_return,
      &increment_return, &page_increment_return);    
    XmScrollBarSetValues(scrollbar, minimum, slider_size_return,
      increment_return, page_increment_return);    
}

/*----------------------------------------------------------------------------
  XbrListNumItems()
  Return the number of items in the XmList.
  ----------------------------------------------------------------------------*/
int XbrListNumItems(Widget w)
{
   int count;
   
   XtVaGetValues(w, XmNitemCount, &count, NULL);
   
   return(count);
}
