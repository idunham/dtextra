/*
** ElecDisp.h for XmtdsElecDisp widget in .
** 
** Made by MaxTheDogSays (dubray_f@epita.fr && rancur_v@epita.fr
** Login   <vr@epita.fr>
** 
** Started on  Mon May 16 22:17:04 1994 vr
** Last update Mon Nov 21 10:50:36 1994 hydrix
*/

#ifndef _XMTDSELECDISP_H_
#define _XMTDSELECDISP_H_       1

#include <X11/StringDefs.h>
#include "Xmtds.h"

extern void             XmtdsEDMessageFunc();
extern void             XmtdsEDSprayFunc();
extern void             XmtdsEDPutPixelFunc();
extern void             XmtdsEDStopFunc();
extern void             XmtdsEDRestartFunc();
extern void             XmtdsEDScrollRightFunc();
extern void             XmtdsEDScrollLeftFunc();
extern void             XmtdsEDScrollUpFunc();
extern void             XmtdsEDScrollDownFunc();
extern void             XmtdsEDSetFontFunc();
extern void             XmtdsEDClearFunc();
extern void             XmtdsEDSpeedFunc();

extern WidgetClass                              xmtdsElecDispWidgetClass;

typedef struct _XmtdsElecDispClassRec           *XmtdsElecDispWidgetClass;
typedef struct _XmtdsElecDispRec                *XmtdsElecDispWidget;

#endif /* _XMTDSELECDISP_H_ */
