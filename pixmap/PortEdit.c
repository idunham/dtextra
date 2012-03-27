/*
 * $Id: PortEdit.c,v 1.5 1994/05/13 14:14:05 mallet Exp $
 * 
 * Copyright (c) 1991-1994  Lionel MALLET
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL Lionel MALLET	BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Lionel MALLET shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from Lionel MALLET.
 *
 * Author:  Tim Wise - Scientific & Engineering Software (SES), Inc.
 */

#include <string.h>

Boolean _PORTDEBUG = False;


/*--------------------------------------------------------------------------*/
/*
			p o r t E d i t o r B u t t o n s

	Buttons for port editor.
*/
/*--------------------------------------------------------------------------*/

static ButtonRec portEditorButtons[] = {

/* buttons ids */

#define kSetPort	0
#define kClearPort	1
#define kMovePort	2
#define kPortInfo	3

  /* button recs  */
  /* id,       name,        trap,   widget */

  { kSetPort,   "setPort",   TOGGLE, NULL },
  { kClearPort, "clearPort", TOGGLE, NULL },
  { kMovePort,  "movePort",  TOGGLE, NULL },
  { kPortInfo,  "portInfo",  TOGGLE, NULL }

}; /* portEditorButtons */



/*****************************************************************************/
/*
			Port List Routines 

	A list of ports is kept in the XpmExtension format which is an
	array of strings, one for each port. Convert string format
	to Port structure when needed.
*/
/*****************************************************************************/


#define XpmPortName	"Ports"
#define UNKNOWN		-1

typedef struct {
    int       id;		/* unique number */
    Position  x;		/* location of port */
    Position  y;
    char      info[256]; 	/* port info: name, kind */
} Port;


/*--------------------------------------------------------------------------*/
/*
			N e w P o r t 

	Allocate a new port structure including a string for info.
	Return a pointer to allocated structure.
*/
/*--------------------------------------------------------------------------*/
Port *NewPort()
{
    Port *p;

    p = (Port *) XtNew(Port);
    if (p != NULL) {
        p->id = UNKNOWN;
        p->x  = UNKNOWN;
        p->y  = UNKNOWN;
        p->info[0] = '\0';
    }
    return  p;
}


/*--------------------------------------------------------------------------*/
/*
			S t r T o P o r t

	Convert a string to a port structure. Return a pointer to the
 	port structure. NOTE: Caller must free structure!!
*/
/*--------------------------------------------------------------------------*/
Boolean StrToPort( str, p )
    char *str;
    Port *p;
{
    char *s, *t;
    int  x, y;

    if (str != NULL && p != NULL) {

        sscanf( str, "%d%d%d",&p->id, &x, &y );
        p->x = (Position) x;
        p->y = (Position) y;
        p->info[0] = '\0';

	s = XtMalloc( strlen(str) + 1 );	
        strcpy( s, str );
        t = strtok( s, " ," ); 		/* first token  id */
        t = strtok( NULL, " ," );	/* second token x  */
        t = strtok( NULL, " ," );	/* third token  y  */

        if (t != NULL && t+strlen(t)+1 < s+strlen(str)) 
            strcpy( p->info, t+strlen(t)+1 );/* stuff after tokens */
        
        return True;

    } else 
        return False;
}


/*--------------------------------------------------------------------------*/
/*
			P o r t T o S t r

	Convert a port structure to a string. Return pointer to string.
	Caller must free string.
*/
/*--------------------------------------------------------------------------*/
char *PortToStr( p )
    Port *p;
{
    char id_str[40];
    char x_str[40];
    char y_str[40];
    char *str = NULL;

    if (p != NULL) {
        sprintf( id_str, "%d", p->id );
        sprintf( x_str,  "%d", p->x );
        sprintf( y_str,  "%d", p->y );
	str = XtMalloc( strlen(x_str) + strlen(y_str) + strlen(id_str)
                        + strlen(p->info) + 4 );
        str[0] = '\0';
        strcat( str, id_str  );
        strcat( str, " "     );
        strcat( str, x_str   );
        strcat( str, " "     );
        strcat( str, y_str   );
        strcat( str, " "     );
        strcat( str, p->info );
    }
    return str;
}


/*--------------------------------------------------------------------------*/
/*
			U p d a t e P o r t 

	Replace string version of port i with info given in port structure.
*/
/*--------------------------------------------------------------------------*/
void UpdatePort( ports, i, p )
    XpmExtension *ports;
    int          i;
    Port         *p;
{
    XtFree( ports->lines[i] );
    ports->lines[i] = PortToStr( p );
}


/*--------------------------------------------------------------------------*/
/*
			F i n d P o r t 

	Search a list of ports and find the first at location (x,y).
	If found, return index of port; otherwise return UNKNOWN.
*/
/*--------------------------------------------------------------------------*/
int FindPort( ports, at_x, at_y )
    XpmExtension *ports;
    Position	 at_x;
    Position	 at_y;
{
    int   i;
    Port  p;

    if (strcmp(ports->name, XpmPortName) == 0) {
        for (i=0; i < ports->nlines ; i++) {
            if ( StrToPort(ports->lines[i], &p) 
                 && p.x == at_x && p.y == at_y ) {
                return i;
            }
        }
    }
    return UNKNOWN;
}


/*--------------------------------------------------------------------------*/
/*
 			I s P o r t 

	See if a port exists at location (x,y). 
*/
/*--------------------------------------------------------------------------*/

Boolean IsPort( w, x, y, return_ports, return_port_i )
    Widget       w;
    Position     x;
    Position     y;
    XpmExtension **return_ports;	/* list of ports; can be NULL! */
    int          *return_port_i;	/* index of port x,y; can be NULL! */
{
    Boolean      result = False;
    XpmExtension *ports = NULL;
    int		 i;

    /* if given a list of ports, use them; else get ports from widget */
    if ( return_ports != NULL && *return_ports != NULL ) 
        ports = *return_ports;
    else 
        ports = PWFindExtension( w, XpmPortName );
    
    if ( ports != NULL ) { 
       if ( (i=FindPort(ports, x, y)) >= 0 ) {
           result = True;
       }
    }

    if ( return_port_i != NULL )
        *return_port_i = i;

    if ( return_ports != NULL ) 
        *return_ports = ports;

    else if ( ports != NULL )
        XpmFreeExtensions( ports, 1 );	/* free ports if caller */
                                        /* doesn't want them    */
    return result;
}


/*--------------------------------------------------------------------------*/
/*
			A d d P o r t

	Add a port to end of list. Return index of new port.
*/
/*--------------------------------------------------------------------------*/
int AddPort( ports, at_x, at_y )
    XpmExtension *ports;
    Position	 at_x;
    Position	 at_y;
{
    Port *p;
    char *port_str;
    int  i = UNKNOWN;

    if ((p=NewPort()) != NULL) {

        p->x  = at_x;
        p->y  = at_y;
        p->id = 0;		/* mark as new port */

        if ( (port_str=PortToStr(p)) != NULL ) {

           i = ports->nlines;
           ports->nlines++;

           if (ports->lines != NULL)
               ports->lines = (char **) XtRealloc( 
                                           (char *)ports->lines, 
                                           ports->nlines * sizeof(char*) );
           else 
               ports->lines = (char **) XtNew( char** );

           ports->lines[i] = port_str;
        }
        XtFree((char *)p);
    }
    return i;
} 



/*--------------------------------------------------------------------------*/
/*
			D e l P o r t

	Delete port i. Shift array elements above i to the left.
*/
/*--------------------------------------------------------------------------*/
void DelPort( ports, i )
    XpmExtension *ports;
    int          i;
{
    char **s, **t, **last;

    XtFree( ports->lines[i] );

    last = ports->lines + ports->nlines;	/* last element */
    ports->nlines--;

    /* shift left elements above deletion, if necessary */
    if (ports->nlines > 0) 
        for (t = ports->lines+i, s = ports->lines+i+1; s < last; *t++ = *s++);
    else {
        XtFree( (char *)ports->lines );
        ports->lines = NULL;
    }
} 


/*--------------------------------------------------------------------------*/
/*
			T r a n s l a t e P o r t

	Add offsets dx and dy to location of port i.
*/
/*--------------------------------------------------------------------------*/
void TranslatePort( ports, port_i, dx, dy )
    XpmExtension *ports;
    int port_i;
    int dx;
    int dy;
{
    Port p;

    if ( StrToPort(ports->lines[port_i], &p) ) {
        p.x += dx;
        p.y += dy;
        UpdatePort( ports, port_i, &p );
    }
}


/*--------------------------------------------------------------------------*/
/*
                        P r i n t P o r t s
*/
/*--------------------------------------------------------------------------*/
void PrintPorts( ports )
    XpmExtension *ports;
{
    fprintf( stderr, "ports:\n" );
    if ( ports ) {
        int i;
        for (i = 0; i < ports->nlines; i++) 
            fprintf(stderr, "\t\t%s\n", ports->lines[i]);
    }
}

 

/*****************************************************************************/
/*
		Port Editor Interface Routines 
*/
/*****************************************************************************/


/*--------------------------------------------------------------------------*/
/*
			P o r t S h a p e

	Return an array of points defining a port's shape.
*/
/*--------------------------------------------------------------------------*/

#define NPortPoints 7

XPoint *PortShape( w, x ,y )
    Widget	w;
    Position	x; 
    Position	y;
{
    static XPoint points[NPortPoints];
    float fx = x;
    float fy = y;
 
    points[0].x = PWInWindowX( w, fx );
    points[0].y = PWInWindowY( w, fy );
    points[1].x = PWInWindowX( w, fx + 0.5 );
    points[1].y = PWInWindowY( w, fy + 0.5 );
    points[2].x = PWInWindowX( w, fx );
    points[2].y = PWInWindowY( w, fy + 1 );
    points[3].x = PWInWindowX( w, fx + 1 );
    points[3].y = PWInWindowY( w, fy + 1 );
    points[4].x = PWInWindowX( w, fx + 0.5 );
    points[4].y = PWInWindowY( w, fy + 0.5 );
    points[5].x = PWInWindowX( w, fx + 1 );
    points[5].y = PWInWindowY( w, fy );
    points[6].x = PWInWindowX( w, fx );
    points[6].y = PWInWindowY( w, fy );

    return points;
}


/*--------------------------------------------------------------------------*/
/*
			D r a w P o r t 

	Just draw a port a location (x,y) with given value (set, clear, 
	invert, or highlight). 
*/
/*--------------------------------------------------------------------------*/
#if NeedFunctionPrototypes
void DrawPort(Widget w, Position x, Position y, int value)
#else
void DrawPort(w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    GC gc;

/*
    if (_PORTDEBUG)
        printf( "DrawPort: %d %d \n", x, y );
*/

    gc = (value == Highlight) ? PWHighlightingGC(w) : PWFramingGC(w);

    XFillPolygon( XtDisplay(w), XtWindow(w), gc,
                  PortShape(w, x, y), NPortPoints, 
                  Convex, CoordModeOrigin );
}


/*--------------------------------------------------------------------------*/
/*
			D r a w I f P o r t 

	Draw a port at location (x,y) if there is a port at that 
	location. This routine is used to highlight a port that a
	user is trying to pick.
*/
/*--------------------------------------------------------------------------*/
#if NeedFunctionPrototypes
void DrawIfPort(Widget w, Position x, Position y, int value )
#else
void DrawIfPort( w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    if ( IsPort(w, x, y, NULL, NULL) )
        DrawPort( w, x, y, value );
} 


/*--------------------------------------------------------------------------*/
/*
			S e t P o r t 

	Create and draw a port at (x,y) if there is not already a port
	there. 
*/
/*--------------------------------------------------------------------------*/
#if NeedFunctionPrototypes
void SetPort(Widget w, Position x, Position y, int value )
#else
void SetPort( w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    XpmExtension *ports = NULL;

    if (_PORTDEBUG)
        printf( "SetPort: %d,%d \n", x, y );

    if ( !IsPort(w, x, y, &ports, NULL) ) {
        if (ports == NULL) {
            PWAddExtension( w, XpmPortName );
            ports = PWFindExtension( w, XpmPortName );
        }

        AddPort( ports, x, y );
        DrawPort( w, x, y, value );

	if (_PORTDEBUG)
            PrintPorts( ports );

        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }
} 
 
 
/*--------------------------------------------------------------------------*/
/*
			C l e a r P o r t 

	Delete the port at (x,y) from the port list and erase the port 
	on the screen.
*/
/*--------------------------------------------------------------------------*/
#if NeedFunctionPrototypes
void ClearPort(Widget w, Position x, Position y, int value )
#else
void ClearPort( w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    XpmExtension *ports = NULL;
    int          port_i;

    if (_PORTDEBUG)
        printf("ClearPort : %d,%d \n", x, y);

    if ( IsPort(w, x, y, &ports, &port_i) ) { 

        DelPort( ports, port_i );
        DrawPort( w, x, y, value );	/* erase */

	if (_PORTDEBUG)
            PrintPorts( ports );

        if ( ports->nlines == 0 )
            PWRemoveExtension( w, XpmPortName );
        else
            PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }
}
 

/*--------------------------------------------------------------------------*/
/*
			D r a g P o r t 

	Draw a port while the user is dragging. Draw only if a port existed
	at the starting location of the drag. 
*/
/*--------------------------------------------------------------------------*/

static struct {

    Boolean  b;		/* Are we dragging a port?  */
    Position from_x;	/* starting point of drag   */
    Position from_y;	
    int      port_i;    /* which port is being dragged */

} dragging = { False, UNKNOWN, UNKNOWN, UNKNOWN };


#if NeedFunctionPrototypes
void DragPort(Widget w, Position x, Position y, int value )
#else
void DragPort( w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    int      i;

    /* if beginning of drag a port, ... */
    if ( dragging.b == False && IsPort(w, x, y, NULL, &i) ) {

        dragging.b      = True;
	dragging.from_x = x;
	dragging.from_y = y;
        dragging.port_i = i;
    }

    if (dragging.b) {
        DrawPort( w, dragging.from_x, dragging.from_y, value );/* erase */
        DrawPort( w, x, y, value );/* draw */
    }
}
 
/*--------------------------------------------------------------------------*/
/*
			M o v e P o r t 

	Move a port from one location to another after user has completed
	dragging.
*/
/*--------------------------------------------------------------------------*/
#if NeedFunctionPrototypes
void MovePort(Widget w, Position to_x, Position to_y, int value )
#else
void MovePort( w, to_x, to_y, value )
    Widget   w;
    Position to_x;
    Position to_y;
    int      value;
#endif
{
    XpmExtension *ports = NULL;

    if (_PORTDEBUG)
        printf("MovePort : from %d,%d to %d,%d \n", 
                dragging.from_x, dragging.from_y, to_x, to_y);

    if ( dragging.b
         && !IsPort(w, to_x, to_y, &ports, NULL)  ) {

        DrawPort( w, dragging.from_x, dragging.from_y, value );/* erase old */
        DrawPort( w, to_x, to_y, value );	               /* draw  new */

        TranslatePort( ports, dragging.port_i, 
                       to_x - dragging.from_x, 
                       to_y - dragging.from_y  );

        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
	if (_PORTDEBUG)
            PrintPorts( ports );
    }
    dragging.b      = False;
    dragging.from_x = UNKNOWN;
    dragging.from_y = UNKNOWN;
    dragging.port_i = UNKNOWN;
}


/*--------------------------------------------------------------------------*/
/*
			P o r t I n f o

	Allow the user to edit the port information for the port at (x,y).
*/
/*--------------------------------------------------------------------------*/

#if NeedFunctionPrototypes
void PortInfo(Widget w, Position x, Position y, int value )
#else
void PortInfo( w, x, y, value )
    Widget   w;
    Position x;
    Position y;
    int      value;
#endif
{
    XpmExtension *ports = NULL;
    Port         p;
    int          i;
    char         *newInfo;

    if (_PORTDEBUG)
        printf("PortInfo : %d,%d\n", x, y);

    if ( IsPort(w, x, y, &ports, &i) ) { 
        StrToPort( ports->lines[i], &p );
        if ( PopupDialog( input_dialog, 
                          "Port Info: <name> <kind>",
                           p.info, &newInfo,
                          XtGrabExclusive) == Okay ) {
	    strcpy( p.info, newInfo );
            UpdatePort( ports, i, &p ); 
        }
	if (_PORTDEBUG)
            PrintPorts( ports );
        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }
}

 
/*--------------------------------------------------------------------------*/
/*
			R e d r a w P o r t s

	Redraw all ports of PixmapWidget.
*/
/*--------------------------------------------------------------------------*/

void RedrawPorts( w, value )
    Widget w;
    int    value;
{
    XpmExtension *ports = NULL;
    Port         p;
    int          i;

    if ( (ports=PWFindExtension( w, XpmPortName )) != NULL ) { 
        for ( i=0; i < ports->nlines; i++ ) { 
            StrToPort( ports->lines[i], &p );
            DrawPort( w, p.x, p.y, value ); 
        }
    }
}


/*--------------------------------------------------------------------------*/
/*
			T r a n s l a t e P o r t s 

	For all ports in PixmapWidget, add dx,dy to the ports location.
*/
/*--------------------------------------------------------------------------*/

void TranslatePorts( w, dx, dy )
    Widget	w;
    Position	dx;
    Position	dy;
{
    XpmExtension *ports = NULL;
    Port         p;
    int          i;

    if (_PORTDEBUG)
        printf("TranslatePorts : %d,%d\n", dx, dy);

    if ( (ports=PWFindExtension( w, XpmPortName )) != NULL ) { 
        for ( i=0; i < ports->nlines; i++ ) { 
            StrToPort( ports->lines[i], &p );

            DrawPort( w, p.x, p.y, Clear );		/* erase old */
            PWTranslatePoint( w, &p.x, &p.y, dx, dy ); 
            DrawPort( w, p.x, p.y, Set );		/* draw new */

	    UpdatePort( ports, i, &p );
        }
	if (_PORTDEBUG)
            PrintPorts( ports );
        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }

}


/*--------------------------------------------------------------------------*/
/*
			F l i p P o r t s 

	Flip all ports of a PixmapWidget either horiziontally or vertically.
*/
/*--------------------------------------------------------------------------*/

void FlipPorts( w, axis  )
    Widget 		w;
    enum FlipAxis	axis;
{
    XpmExtension *ports = NULL;
    Port         p;
    int          i;

    if ( (ports=PWFindExtension( w, XpmPortName )) != NULL ) { 
        for ( i=0; i < ports->nlines; i++ ) { 
            StrToPort( ports->lines[i], &p );

            DrawPort( w, p.x, p.y, Clear );		/* erase old */
            PWFlipPoint( w, &p.x, &p.y, axis ); 
            DrawPort( w, p.x, p.y, Set );		/* draw new */

	    UpdatePort( ports, i, &p );
        }
	if (_PORTDEBUG)
            PrintPorts( ports );
        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }
}


/*--------------------------------------------------------------------------*/
/*
			R o t a t e P o r t s 

	Flip all ports of a PixmapWidget either left or right 90 degrees.
*/
/*--------------------------------------------------------------------------*/

void RotatePorts( w, direction )
    Widget			w;
    enum RotateDirection	direction;
{
    XpmExtension *ports = NULL;
    Port         p;
    int          i;

    if ( (ports=PWFindExtension( w, XpmPortName )) != NULL ) { 
        for ( i=0; i < ports->nlines; i++ ) { 
            StrToPort( ports->lines[i], &p );

            DrawPort( w, p.x, p.y, Clear );		/* erase old */
            PWRotatePoint( w, &p.x, &p.y, direction ); 
            DrawPort( w, p.x, p.y, Set );		/* draw new */

	    UpdatePort( ports, i, &p );
        }
	if (_PORTDEBUG)
            PrintPorts( ports );
        PWUpdateExtension( w, ports );
	XpmFreeExtensions( ports, 1 );
    }
}


/*--------------------------------------------------------------------------*/
/*
			P o r t E d i t o r C a l l b a c k

	Callback for all port editor buttons
*/
/*--------------------------------------------------------------------------*/
void PortEditorCallback( w, client_data, call_data )
    Widget w;
    XtPointer client_data, call_data;
{
    int *id = (int *)client_data;

    switch (*id) {


    case kSetPort:
	if (_PORTDEBUG)
            printf( "PortEditorCallback: %s\n", portEditorButtons[*id].name );
        PWRemoveAllRequests( pixmap_widget );

	PWSetPickPixelDrawProc( pixmap_widget, DrawPort );
	PWSetPickPixelCompleteProc( pixmap_widget, SetPort );

        PWEngageRequest( pixmap_widget, PickPixelRequest, True, Plain );
        break;

    case kClearPort:
	if (_PORTDEBUG)
            printf( "PortEditorCallback: %s\n", portEditorButtons[*id].name );
        PWRemoveAllRequests( pixmap_widget );

	PWSetPickPixelDrawProc( pixmap_widget, DrawIfPort );
	PWSetPickPixelCompleteProc( pixmap_widget, ClearPort );

        PWEngageRequest( pixmap_widget, PickPixelRequest, True, Plain );
        break;

    case kMovePort:
	if (_PORTDEBUG)
            printf( "PortEditorCallback: %s\n", portEditorButtons[*id].name );
        PWRemoveAllRequests( pixmap_widget );

	PWSetPickPixelDrawProc( pixmap_widget, DragPort );
	PWSetPickPixelCompleteProc( pixmap_widget, MovePort );

        PWEngageRequest( pixmap_widget, PickPixelRequest, True, Plain );
	break;

    case kPortInfo:
	if (_PORTDEBUG)
            printf( "PortEditorCallback: %s\n", portEditorButtons[*id].name );
        PWRemoveAllRequests( pixmap_widget );

	PWSetPickPixelDrawProc( pixmap_widget, DrawIfPort );
	PWSetPickPixelCompleteProc( pixmap_widget, PortInfo );

        PWEngageRequest( pixmap_widget, PickPixelRequest, True, Plain );
	break;

    default:
        printf( "PortEditorCallback: invalid id = %d\n", *id );
    } /* switch */

} /* PortEditorCallback */



/*--------------------------------------------------------------------------*/
/*
		C r e a t e P o r t E d i t o r B u t t o n s

	Create buttons of port editor.
*/
/*--------------------------------------------------------------------------*/
void CreatePortEditorButtons( parentW, radioGroupW )
    Widget	parentW;	/* parent widget of port editor buttons */
    Widget	radioGroupW;	/* radio group to add port editor buttons */
{
    int    i;
    Widget w;

    for (i = 0; i < XtNumber(portEditorButtons); i++) {

#ifndef USE_ATHENA
	w = XmCreateToggleButtonGadget(parentW, portEditorButtons[i].name,
				       NULL, 0);
	XtManageChild(w);
        portEditorButtons[i].widget = w;
 
        XtAddCallback(w, XmNvalueChangedCallback, PortEditorCallback,
		      &portEditorButtons[i].id );
	
#else
        w = XtCreateWidget( portEditorButtons[i].name,
                            toggleWidgetClass,
                            parentW, NULL, 0 );

        portEditorButtons[i].widget = w;
 
        XtAddCallback( w, 
		       XtNcallback, PortEditorCallback,
                       &portEditorButtons[i].id );

	/* add to button to radio group */
	XtVaSetValues( w,
		       XtNradioGroup, radioGroupW,
		       NULL );
#endif
    } 
} 



/*--------------------------------------------------------------------------*/
/*
		M a n a g e P o r t E d i t o r B u t t o n s

	Manage or unmanage the buttons of port editor depending on flag.
*/
/*--------------------------------------------------------------------------*/
void ManagePortEditorButtons( manage )
    Boolean	manage;		/* if true, manage; otherwise unmanage */
{
    int    i;

    for (i = 0; i < XtNumber(portEditorButtons); i++) {
	if (manage) 
            XtManageChild( portEditorButtons[i].widget );
        else 
            XtUnmanageChild( portEditorButtons[i].widget );
    } 
}
