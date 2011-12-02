/*
 *  Header file for mxkill
 *
 *  RCS:
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
 *      muquit@semcor.com   Sep-04-95   first cut
 */

#ifndef _MXKILL_H
#define _MXKILL_H

#ifndef True
#define True 1
#endif

#ifndef False
#define False 0
#endif

#define LF  10


#if __STDC__ || defined(sgi) || defined(_AIX)
#define _Declare(formal_parameters) formal_parameters
#else
#define _Declare(formal_parameters) ()
#endif

#ifdef Extern
#undef Extern
#endif

#ifndef __Main__
#define Extern extern
#else
#define Extern
#endif

#define Version "1.1"
#define Author          "muquit@semcor.com"
#define Url             "http://www.semcor.com/~muquit/"

#define Min(x,y)  (((x) < (y)) ? (x) : (y))

#define MaxTextLength   2048

Extern Widget
    Gtoggle_menuW[6],
    GtoplevelW;

Extern Display
    *Gdisplay;

Extern Colormap
    Gcolormap;

Extern GC
    Ggc;

Extern unsigned long
    Gblackpixel,
    Gwhitepixel;

Extern Window
    Grootwindow;

Extern int
    GdisplayWidth,
    GdisplayHeight,
    Gdepth;

Extern int
    Gsignal_no;

Extern Boolean
    Giconic_state;

Extern char
    GpsCommand[400],
    GpsdefOption[500],
    GpsAllOption[500],
    GpsAllbyUserOption[500],
    GpsAllbyPidOption[500];
/*
** prototype declaration
*/
int
    MXError _Declare ((Display *, XErrorEvent *));

void
    Error _Declare ((char *, char *));
    
void
    Warning _Declare ((char *, char *));

void
    NullCb _Declare ((Widget,XtPointer,XtPointer));

void
    CreateMainWindow _Declare ((Widget));

void
    QuitCb _Declare ((Widget,XtPointer,XtPointer));

void
    SignalSetCb _Declare ((Widget,XtPointer,XtPointer));

void
    CreateKillPixmaps _Declare ((Pixel,Pixel));

void
    FreeKillPixmaps _Declare ((void));

void
    SetPixmap _Declare ((Pixmap,Widget));

void
    SetKillPixmap _Declare ((int));

void
    PsCb _Declare ((Widget,XtPointer,XtPointer));

unsigned int
    GetpsInfo _Declare ((char *));


int
    StrIndex _Declare ((char *, char *));

char
    *GetpsOptions _Declare ((void));

void
    psOptionTextCb _Declare ((Widget,XtPointer,XtPointer));

void
    PutListOnScreen _Declare ((void));

void
    SetScrollbarColors _Declare ((Widget,Widget,Pixel));

void
    KillProcessCb _Declare ((Widget,XtPointer,XtPointer));

void
    GetListPositions _Declare ((void));

void
    PatternMatchCb _Declare ((Widget,XtPointer,XtPointer));

void
    SetResources _Declare ((Widget));

char
    *mystrdup _Declare ((char *));

void
    psOptionCb _Declare ((Widget,XtPointer,XtPointer));

void
    AboutdCb _Declare ((Widget,XtPointer,XtPointer));

void
    CreateAboutD _Declare ((Widget));

unsigned long
    AllocateNamedColor _Declare ((Display*,char *));

void
    BestPixel _Declare ((Display *, Colormap, XColor *, unsigned int,
        XColor *));

void
    HandleIconicStates _Declare ((Widget,XtPointer,XEvent *));

void
    DoStderr _Declare ((XtPointer,int,XtInputId *));

void
    SetbusyCursorState _Declare ((Widget,int));

void
    WriteMessage _Declare ((char *,int));

#endif /* _MXKILL_H */
