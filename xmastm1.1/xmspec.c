/*
 *  Contains menu items
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      who                 when        why
 *      muquit@semcor.com   21-Apr-95   first cut
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"
#include "mqtools.h"

MENUITEM file_items[] =
{
    {"Restart",  &xmPushButtonGadgetClass, 'R',
        "Meta<Key>R", "Meta+R",
     FileMenuCB, (XtPointer) RESTART, (MENUITEM *) NULL,
     1, ALWAYS_SENSITIVE},
     {"", &xmSeparatorGadgetClass, (int) NULL,
         NULL, NULL,
      NULL, NULL, (MENUITEM *) NULL, 1, ALWAYS_SENSITIVE},
     {"Quit", &xmPushButtonGadgetClass, 'Q',
        "Meta<Key>Q", "Meta+Q",
     FileMenuCB, (XtPointer) QUIT, (MENUITEM *) NULL,
     1,ALWAYS_SENSITIVE},
     {NULL},

};

MENUITEM help_items[] =
{
    {"About ...", &xmPushButtonGadgetClass, 'A',
        "Meta<Key>A", "Meta+A",
    HelpCB, (XtPointer) ABOUT_HELP, (MENUITEM *) NULL,
    1, ALWAYS_SENSITIVE},
    {NULL},
};
