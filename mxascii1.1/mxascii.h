#ifndef MXASCII_H
#define MXASCII_H   1
/*
 *  mxascii.h  -   header file for mxascii
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Description:
 *      text
 *
 *  Dependencies:
 *      file1.h
 *
 *  Comments:
 *      text
 *
 *  Development History:
 *      who                 who                 why
 *      MA_Muquit@fccc.edu  Jun-18-1996         first cut
 */

#ifndef True
#define True    1
#endif

#ifndef False
#define False 0
#endif

#ifdef Extern
#undef Extern
#endif

#ifndef INCLUDED_FROM_MAIN
#define Extern  extern
#else
#define Extern
#endif /* INCLUDED_FROM_MAIN */


/*
** unfortunate Globals
*/
Extern Widget
    GtoplevelW,
    GmainwW,

    Gvalue_labelW[6],
    Gascii_labelW[6],

    Gtoggle_menuW[3];




/*
** function prototypes
*/
void    CreateUserInterface     (Widget);
void    QuitCb                  (Widget,XtPointer,XtPointer);
void    NullCb                  (Widget,XtPointer,XtPointer);
void    SetCb                   (Widget,XtPointer,XtPointer);
void    MakeTable               (void);
void    CreateAboutD            (Widget);
void    AboutdCb                 (Widget,XtPointer,XtPointer);

#endif /* MXASCII_H */
