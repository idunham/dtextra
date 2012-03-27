/* * Last edited: Sep 12 16:59 1991 (mallet) */
/*
 * $Id: Requests.h,v 1.4.1.1 1992/08/05 15:26:04 mallet Exp $
 * 
 * Copyright 1991 Lionel Mallet
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appears in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Lionel MALLET not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Lionel MALLET makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * Lionel MALLET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL Lionel MALLET BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  This software is opened and free. Furthermore, everybody is kindly
 * invited to participate to improve it for the benefit of all.
 * Improvements can be new features, bugs fixes and porting issues
 * resolution.
 *
 * Author:  Lionel Mallet, SIMULOG
 */

/*
 * $XConsortium: Requests.h,v 1.2 90/06/09 20:20:44 dmatic Exp $
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Davor Matic, MIT X Consortium
 */



typedef struct {
    Boolean  success;
    Position at_x, at_y;
    Position from_x, from_y,
             to_x, to_y;
    void   (*draw)();
    Pixel    value;
    Time     time;
    int      state;
} PWStatus;

void OnePointEngage();
void OnePointTerminate();
void OnePointTerminateTransparent();
void DragOnePointEngage();
void DragOnePointTerminate();
void TwoPointsEngage();
void TwoPointsTerminate();
void TwoPointsTerminateTransparent();
void TwoPointsTerminateTimed();
void DragTwoPointsEngage();
void DragTwoPointsTerminate();
void Interface();
void Paste();

static PWRequestRec requests[] =
{
{MarkRequest, sizeof(PWStatus),
     TwoPointsEngage, (XtPointer) PWDrawRectangle,
     TwoPointsTerminateTimed, (XtPointer) PWSelect,
     NULL, (XtPointer) NULL},
{RestoreRequest, sizeof(PWStatus),
     OnePointEngage, (XtPointer) PWDragStored,
     OnePointTerminate, (XtPointer) PWRestore,
     NULL, (XtPointer) NULL},
{ImmediateCopyRequest, sizeof(PWStatus),
     OnePointEngage, (XtPointer) PWDragMarked,
     OnePointTerminate, (XtPointer) PWCopy,
     NULL, (XtPointer) NULL},
{ImmediateMoveRequest, sizeof(PWStatus),
     OnePointEngage, (XtPointer) PWDragMarked,
     OnePointTerminate, (XtPointer) PWMove,
     NULL, (XtPointer) NULL},
{CopyRequest, sizeof(PWStatus),
     DragOnePointEngage, (XtPointer) Paste,
     DragOnePointTerminate, (XtPointer) ImmediateCopyRequest,
     Interface, (XtPointer) PWUnmark},
{MoveRequest, sizeof(PWStatus),
     DragOnePointEngage, (XtPointer) Paste,
     DragOnePointTerminate, (XtPointer) ImmediateMoveRequest,
     Interface, (XtPointer) PWUnmark},
{PointRequest, sizeof(PWStatus),
     DragOnePointEngage, (XtPointer) PWDrawPoint,
     DragOnePointTerminate, (XtPointer) PWDrawPoint,
     NULL, (XtPointer) NULL},
{CurveRequest, sizeof(PWStatus),
     DragTwoPointsEngage, (XtPointer) PWBlindLine,
     DragTwoPointsTerminate, (XtPointer) PWBlindLine,
     NULL, (XtPointer) NULL},
{LineRequest, sizeof(PWStatus), 
     TwoPointsEngage, (XtPointer) PWDrawLine, 
     TwoPointsTerminate, (XtPointer) PWDrawLine,
     NULL, (XtPointer) NULL},
{RectangleRequest, sizeof(PWStatus), 
     TwoPointsEngage, (XtPointer) PWDrawRectangle,
     TwoPointsTerminate, (XtPointer) PWDrawRectangle,
     NULL, (XtPointer) NULL},
{FilledRectangleRequest, sizeof(PWStatus), 
     TwoPointsEngage, (XtPointer) PWDrawRectangle,
     TwoPointsTerminate, (XtPointer) PWDrawFilledRectangle,
     NULL, (XtPointer) NULL},
{CircleRequest, sizeof(PWStatus), 
     TwoPointsEngage, (XtPointer) PWDrawCircle,
     TwoPointsTerminate, (XtPointer) PWDrawCircle,
     NULL, (XtPointer) NULL},
{FilledCircleRequest, sizeof(PWStatus), 
     TwoPointsEngage, (XtPointer) PWDrawCircle, 
     TwoPointsTerminate, (XtPointer) PWDrawFilledCircle,
     NULL, (XtPointer) NULL},
{FloodFillRequest, sizeof(PWStatus),
     OnePointEngage, (XtPointer) NULL,
     OnePointTerminate, (XtPointer) PWFloodFill,
     NULL, (XtPointer) NULL},
{HotSpotRequest, sizeof(PWStatus),
     OnePointEngage, (XtPointer) PWDrawHotSpot,
     OnePointTerminate, (XtPointer) PWDrawHotSpot,
     NULL, (XtPointer) NULL},
{PickPixelRequest, sizeof(PWStatus),
     OnePointEngage,    (XtPointer) PWPickPixelDraw,
     OnePointTerminate, (XtPointer) PWPickPixelComplete,
     NULL, (XtPointer) NULL},
{ZoomInRequest, sizeof(PWStatus),
     TwoPointsEngage, (XtPointer) PWDrawRectangle,
     TwoPointsTerminate, (XtPointer) PWZoomIn,
     NULL, (XtPointer) NULL},
{TextRequest, sizeof(PWStatus),
     OnePointEngage,    (XtPointer) PWDragText,
     OnePointTerminate, (XtPointer) PWDrawText,
     NULL, (XtPointer) NULL},
};


