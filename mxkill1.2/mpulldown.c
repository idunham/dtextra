/*
 *  MBuildPulldownMenu() - automated production of pulldown menus
 *
 *  RCS:
 *      $Revision: 1.1 $
 *      $Date: 1995/05/07 19:34:10 $
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      MBuildPulldownMenu() automates the createion of pulldwon menus
 *      if the user has already set up the menu item structure as
 *      required by this routine.
 *      NOTE: This function was copied verbatim from
 *              Motif Programming Manual for OSF/Motif V 1.1
 *              Volume 6
 *              Dan Heller
 *              O'Reilly and Associates, Inc.  September 1991
 *              page 554-5
 *
 *  Input Parameters:
 *      type    identifier  description
 *      Widget  parent      parent widget to connect pulldown menu to
 *      char    *menu_title title given to pulldown menu
 *      char    *menu_mnemonic
 *                          mnemonic that can be used to access menu
 *      MENUITEM *items     pointer to array structure holding info
 *                          for menu to be created.
 *
 *  Output Parameters:
 *      type    identifier  description
 *      None
 *
 *  Return Values:
 *      value   description
 *      cascade widget containing pulldown cascade menu
 *
 *  Side Effects:
 *      None
 *
 *  Limitations and Comments:
 *      None
 *
 *  Development History:
 *      when        who why
 *      06-Apr-1992 um  function creation
 *      11/14/93,   mm, modified to add sensitivity to menu items
 */
#include "xhead.h"
#include "mxkill.h"
#include "menu.h"

Widget MBuildPulldownMenu (parent, menu_title, menu_mnemonic, items)
Widget
    parent;             /* parent widget to connect to */

char
    *menu_title;        /* title for menu */
int
    menu_mnemonic;      /* mnemonic for selecting menu */

MenuItem
    *items;             /* menu information */
{

Widget
    PullDown,           /* pulldown menu created */
    cascade,            /* cascade widget created */
    widget;             /* intermediary widgets created */

int
    i;

XmString
    str;

  PullDown = XmCreatePulldownMenu (parent, "_pulldown", NULL, 0);

  /*
  ** turn on tearoff model
  */
  XtVaSetValues(PullDown,
    XmNtearOffModel, XmTEAR_OFF_ENABLED,
    NULL);

  str = XmStringCreateSimple (menu_title);
  cascade = XtVaCreateManagedWidget (menu_title,
     xmCascadeButtonWidgetClass, parent,
     XmNsubMenuId, PullDown,
     XmNlabelString, str,
     XmNmnemonic, menu_mnemonic,
     NULL);

  XmStringFree (str);

  /*
  ** Now add the menu items
  */


  for (i = 0; items[i].label != NULL; i++)
  {

    /*
     * If subitems exist, create the pullrigth menu by calling this
     * function recursively.  Since the function returns a cascade
     * button, the widget returned is used.
     */

        if (items[i].subitems)
        {
          widget = MBuildPulldownMenu (PullDown, items[i].label,
              items[i].mnemonic, items[i].subitems);
        }
        else
        {
             widget = XtVaCreateManagedWidget (items[i].label,
                *items[i].class, PullDown,
                NULL);
        } /* endif subitems */

        /*
         * Wether the item is a real item of a cascade button with a
         * menu, it can still have a mnemonic.
         */

        if (items[i].mnemonic)
        {
             XtVaSetValues (widget, XmNmnemonic, items[i].mnemonic,
                 NULL);
        } /* endif  mnemonic */

    /*
     * Any item can have an accelerator, except cascade menus.  But,
     * we don't worry about tht; we know better in our declarations.
     */

        if (items[i].accelerator)
        {
            str = XmStringCreateSimple (items[i].accel_text);
            XtVaSetValues (widget,
                 XmNaccelerator, items[i].accelerator,
                 XmNacceleratorText, str,
                 NULL);
            XmStringFree (str);
        }/* endif accelerator */

        /*
        ** if the class in Toggle, by default for menuitems XmNvisibleWhenOff
        ** is False, we need to set it to true
        */
  
        if (items[i].class == &xmToggleButtonWidgetClass ||
            items[i].class == &xmToggleButtonGadgetClass)
        {
            Gtoggle_menuW[i]=widget;
            XtVaSetValues(widget,
                XmNvisibleWhenOff, True,
                NULL);
        }
    /*
     * Again, anyone can have a callback -- however, this is an
     * activate-callback. This may not be appropriate for all
     * items.
     */

        if (items[i].callback)
        {
          XtAddCallback (widget, 
            (items[i].class == &xmToggleButtonWidgetClass ||
             items[i].class == &xmToggleButtonGadgetClass) ?
             XmNvalueChangedCallback :
             XmNactivateCallback,
             items[i].callback, items[i].callback_data);
        } /* endif callback */


        if (items[i].class == &xmToggleButtonWidgetClass ||
            items[i].class == &xmToggleButtonGadgetClass)
        {
            if (items[i].toggle_set == 1)
                XtVaSetValues (widget,
                    XmNset, True,
                    NULL);
        }

        if (!items[i].sensitiv) /* not sensitive */
            XtSetSensitive (widget, False);

  } /* endfor */

  return (cascade);

}   /* end MBuildPulldownMenu */
