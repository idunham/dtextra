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
 * FILE NAME:	XbrMenu.c
 * CREATED:	Mon Nov  1 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Routine for creating popup or pulldown menus.
 *
 */

#include <Xm/Xm.h>
#include <Xm/CascadeB.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>
#include <Xm/ToggleB.h>

#include "XbrMenu.h"

#if (XmVERSION < 1) || ((XmVERSION == 1) && (XmREVISION < 2))
#define XmStringCreateLocalized(string) XmStringCreateLtoR(string,XmSTRING_DEFAULT_CHARSET)
#endif

/* Just to make it more readable */
#define MENU		0
#define CASCADE		1

/* XbrMenu:*******************************************************************/
/* XbrMenu: Create a popup or pulldown menu. Details are all contained in    */
/* XbrMenu: menu_data structure - see header file.                           */
/* XbrMenu:*******************************************************************/

Widget *XbrMenu(Widget parent, char *title, int menu_type,
  XbrMenuData menu_data[], int n)
{
    XtAppContext app_context = XtWidgetToApplicationContext(parent);
    Widget *widgets;
    Boolean radio_behaviour = False;
    int i, extras;
    Arg args[10];
    Cardinal num;
    XmString string;
    char name[256];

    /* Number of extra widgets to create - for a popup this is just the popup
       menu but for pulldowns there is the menu and the cascade button.
    */
    if(menu_type == XbrMENU_POPUP)
        extras = 1;
    else
        extras = 2; 
                      
    /* Allocate memory for widgets */
    if((widgets = (Widget *) malloc(sizeof(Widget) * (n+extras))) == NULL) {
        XtAppError(app_context, "Error - out of memory!\n");
        return(NULL);
    }

    /* Create menu */
    switch(menu_type) {
        case XbrMENU_POPUP:
            sprintf(name, "%sPopup", title);
            widgets[MENU] = XmCreatePopupMenu(parent, name, NULL, 0);
            break;
        case XbrMENU_OPTION:
            sprintf(name, "%sPane", title);
            widgets[MENU] = XmCreatePulldownMenu(parent, name, NULL,0);
            num = 0; 
            string = XmStringCreateLocalized(title);
            XtSetArg(args[num], XmNsubMenuId, widgets[MENU]); num++;
            XtSetArg(args[num], XmNlabelString, string); num++;
            sprintf(name, "%sOption", title);
            widgets[CASCADE] = XmCreateOptionMenu(parent, name, args, num);
            XmStringFree(string);
            XtManageChild(widgets[CASCADE]);
            break;
        case XbrMENU_PULLDOWN_RADIO:
            radio_behaviour = True;
        case XbrMENU_PULLDOWN:
            sprintf(name, "%sPulldown", title);
            widgets[MENU] = XmCreatePulldownMenu(parent, name, NULL,0);
            XtVaSetValues(widgets[MENU], XmNradioBehavior, radio_behaviour,
              NULL);

            /* Create menu title */
            sprintf(name, "%sCascade", title);
            widgets[CASCADE]  = XtVaCreateManagedWidget(name,
                          xmCascadeButtonWidgetClass, parent,
                          XmNsubMenuId, widgets[MENU],
                          XtVaTypedArg, XmNlabelString, XmRString,
                          title, sizeof(char *),
                          NULL);
            break;
        default:
            XtAppError(app_context, "Unknown menu type!\n");
    }

    /* Add desired widgets */
    for(i = 0; i < n; i++) {
        sprintf(name, "%s%d", title, i);
        switch(menu_data[i].type) {
            /* Label widget - easy */
            case XbrLABEL:
                string = XmStringCreateLocalized(menu_data[i].label);
                widgets[i+extras] = XtVaCreateManagedWidget(name,
                                        xmLabelWidgetClass, widgets[MENU],
                                        XmNlabelString, string,
                                        NULL);
                XmStringFree(string);
                break;

            /* Push button gadget - include activate callback & data */
            case XbrPB:
                string = XmStringCreateLocalized(menu_data[i].label);
                widgets[i+extras] = XtVaCreateManagedWidget(name,
                          xmPushButtonWidgetClass, widgets[MENU],
                          XmNlabelString, string,
                          NULL);
                XmStringFree(string);
                if(menu_data[i].callback)
                    XtAddCallback(widgets[i+extras], XmNactivateCallback,
                      menu_data[i].callback, menu_data[i].data);
                if(menu_data[i].set && menu_type == XbrMENU_OPTION)
                    XtVaSetValues(widgets[CASCADE], XmNmenuHistory, 
                        widgets[i+extras], NULL);
                break;

            /* Toggle button - include value changed callback & data */
            case XbrTOGGLE:
                num = 0;
                string = XmStringCreateLocalized(menu_data[i].label);
                XtSetArg(args[num], XmNlabelString, string); num++;
                XtSetArg(args[num], XmNindicatorType, XmONE_OF_MANY); num++;
                if(menu_data[i].set != XbrNOSET) {
                    XtSetArg(args[num], XmNset, menu_data[i].set); num++;
                } 
                widgets[i+extras] = XmCreateToggleButton(widgets[MENU], name,
                  args, num);
                XtManageChild(widgets[i+extras]);
                XmStringFree(string);
                if(menu_data[i].callback)
                    XtAddCallback(widgets[i+extras], XmNvalueChangedCallback,
                      menu_data[i].callback, menu_data[i].data);
                break;

            /* Separator to make menu look pretty! */
            case XbrSEPARATOR:
                widgets[i+extras] = XtVaCreateManagedWidget(name,
                           xmSeparatorWidgetClass, widgets[MENU],
                           NULL);
                break;

            /* A sub menu */
            case XbrSUBMENU:
                widgets[i+extras] = (Widget)XbrMenu(widgets[MENU], menu_data[i].title, menu_data[i].stype,
                  menu_data[i].submenu, menu_data[i].n);
                break;
            default:
                XtAppError(app_context, "Unknown menu item!\n");
                break;
        }
    }

    return(widgets);
}
