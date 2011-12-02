/*
 *  menu items struct
 *
 *  RCS:
 *      $Revision$
 *      $Date$
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
 *      who                  when       why
 *      MA_Muquit@fccc.edu   31-May-96  first cut
 */

#include "xhead.h"
#include "mxascii.h"
#include "menu.h"


MenuItem file_items[]=
{
    {"Exit",&xmPushButtonGadgetClass,'x',
        "Meta<Key>Q","Alt+Q",
    QuitCb, (XtPointer) NULL, (MenuItem *) NULL,1,1,0},
    {NULL},
};

MenuItem options_items[] =
{
    {"Decimal",&xmToggleButtonWidgetClass,'D',
        "Meta<Key>D","Alt+D",
    SetCb,(XtPointer)0,(MenuItem *) NULL,1,1,0},
    {"Hexadecimal",&xmToggleButtonWidgetClass,'H',
        "Meta<Key>H","Alt+H",
    SetCb,(XtPointer)1,(MenuItem *) NULL,1,1,0},
    {"Octal",&xmToggleButtonWidgetClass,'O',
        "Meta<Key>O","Alt+O",
    SetCb,(XtPointer)2,(MenuItem *) NULL,1,1,0},
    {NULL},
};

MenuItem help_items[]=
{
    {"About",&xmPushButtonGadgetClass,'b',
        "Ctrl<Key>H","Ctrl+H",
    AboutdCb,(XtPointer) NULL, (MenuItem *) NULL,1,1,0},
    {NULL},
};
