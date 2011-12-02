/*
 *  structure for menu
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
 *      muquit@semcor.com   04-Sep-95   fist cut
 */

#include "xhead.h"
#include "mxkill.h"
#include "menu.h"

MenuItem file_items[]=
{
    {"Quit",&xmPushButtonGadgetClass,'Q',
        "Meta<Key>Q","Alt+Q",
    QuitCb,(XtPointer)NULL,(MenuItem *) NULL,1,1,0},
    {NULL},
};

MenuItem signal_items[]=
{
    {"Hangup (1)",&xmToggleButtonWidgetClass,'H',
        "Meta<Key>H","Alt+H",
    SignalSetCb,(XtPointer)1,(MenuItem *) NULL,1,1,0},
    {"Quit (3)",&xmToggleButtonWidgetClass,'Q',
        "Meta<Key>Q","Alt+Q",
    SignalSetCb,(XtPointer)3,(MenuItem *) NULL,1,1,0},
    {"Kill (9)",&xmToggleButtonWidgetClass,'K',
        "Meta<Key>K","Alt+K",
    SignalSetCb,(XtPointer)9,(MenuItem *) NULL,1,1,1},
    {"Terminate (15)",&xmToggleButtonWidgetClass,'T',
        "Meta<Key>T","Alt+T",
    SignalSetCb,(XtPointer)15,(MenuItem *) NULL,1,1,0},
    {"Stop (17)",&xmToggleButtonWidgetClass,'o',
        "Meta<Key>7","Alt+7",
    SignalSetCb,(XtPointer)17,(MenuItem *) NULL,1,1,0},
    {"Continue (19)",&xmToggleButtonWidgetClass,'C',
        "Meta<Key>C","Alt+C",
    SignalSetCb,(XtPointer)19,(MenuItem *) NULL,1,1,0},
    {NULL},
};

MenuItem ps_items[]=
{
    {"Default",&xmPushButtonGadgetClass,'D',
        "Meta<Key>D","Alt+D",
    psOptionCb,(XtPointer) 0,(MenuItem *) NULL,1,1,0},
    {"All",&xmPushButtonGadgetClass,'A',
        "Meta<Key>A","Alt+A",
    psOptionCb,(XtPointer) 1,(MenuItem *) NULL,1,1,0},
    {"Sort All by User",&xmPushButtonGadgetClass,'U',
        "Meta<Key>U","Alt+U",
    psOptionCb,(XtPointer) 2,(MenuItem *) NULL,1,1,0},
    {"Sort All by pid",&xmPushButtonGadgetClass,'P',
        "Meta<Key>P","Alt+P",
    psOptionCb,(XtPointer) 3,(MenuItem *) NULL,1,1,0},
    {NULL},
};

MenuItem help_items[]=
{
    {"About ...",&xmPushButtonGadgetClass,'b',
        "<Key>F1","F1",
    AboutdCb,(XtPointer)NULL,(MenuItem *) NULL,1,1,0},
    {NULL},

};
