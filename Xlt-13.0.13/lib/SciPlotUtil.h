/* $Id: SciPlotUtil.h,v 1.4 2001/06/09 18:38:57 amai Exp $ */
/* 
 * Copyright © 1999-2001 by the LessTif developers.
 */

#ifndef _SCIPLOTUTIL_H
#define _SCIPLOTUTIL_H

#ifdef __cplusplus
extern "C" 
{
#endif

/* SciPlotUtil.c */
Widget SciPlotDialog (Widget parent, char *title);
void SciPlotDialogPopup (Widget w);
void SciPlotReadDataFile (Widget parent, FILE *fd);

#ifdef __cplusplus
};
#endif

#endif /* _SCIPLOTUTIL_H */
