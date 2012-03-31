/*
 * strdefs.h
 *	Extra string defs for MGv.
 *
 * Copyright (C) 1996  Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Matthew D. Francey
 *		Eric A. Howe (mu@trends.net)
 */
#ifndef	MGV_STRDEFS_H
#define	MGV_STRDEFS_H
MU_HID(strdefs_h, "$Mu: mgv/mine/strdefs.h 1.56 1998/08/16 09:45:56 $")

extern const char mgv_strdefs[];

#define	MgvNantialiasEPS	"antialiasEPS"
#define	MgvCAntialiasEPS	"AntialiasEPS"

#define	MgvNautoHeight	"autoHeight"
#define	MgvCAutoHeight	"AutoHeight"

#define	MgvNautoWidth	"autoWidth"
#define	MgvCAutoWidth	"AutoWidth"

#define MgvNbunzip	"bunzip"
#define MgvCBunzip	"Bunzip"

#define	MgvNbzip	"bzip"
#define	MgvCBzip	"Bzip"

#define	MgvNcenter	"center"
#define	MgvCCenter	"Center"

#define	MgvNconfirmExit	"confirmExit"
#define	MgvCConfirmExit	"ConfirmExit"

#define	MgvNdecompress	"decompress"
#define	MgvCDecompress	"Decompress"

#define	MgvNcompress	"compress"
#define	MgvCCompress	"Compress"

#define	MgvNdefaultPageMedia	"defaultPageMedia"
#define	MgvCDefaultPageMedia	"DefaultPageMedia"
#define	MgvRPageMedia		"PageMedia"

#define MgvNgunzip	"gunzip"
#define MgvCGunzip	"Gunzip"

#define	MgvNgzip	"gzip"
#define	MgvCGzip	"Gzip"

#define	MgvNhideLog	"hideLog"
#define	MgvCHideLog	"HideLog"

#define	MgvNhideMenuBar	"hideMenuBar"
#define	MgvCHideMenuBar	"HideMenuBar"

#define	MgvNhidePageList	"hidePageList"
#define	MgvCHidePageList	"HidePageList"

#define	MgvNhideStatusLine	"hideStatusLine"
#define	MgvCHideStatusLine	"HideStatusLine"

#define	MgvNhideToolBar	"hideToolBar"
#define	MgvCHideToolBar	"HideToolBar"

#define	MgvNkillChildrenAtExit	"killChildrenAtExit"
#define	MgvCKillChildrenAtExit	"KillChildrenAtExit"

#define	MgvNmagicResistance	"magicResistance"
#define	MgvCMagicResistance	"MagicResistance"

#define	MgvNprintCommand	"printCommand"
#define	MgvCPrintCommand	"PrintCommand"

#define	MgvNresetScrollBars	"resetScrollBars"
#define	MgvCResetScrollBars	"ResetScrollBars"
#define	MgvRResetScrollBars	"ResetScrollBars"

#define	MgvNreticuleForeground	"reticuleForeground"
#define	MgvCReticuleForeground	"ReticuleForeground"

#define	MgvNreticuleLineWidth	"reticuleLineWidth"
#define	MgvCReticuleLineWidth	"ReticuleLineWidth"

#define	MgvNshowLabels	"showLabels"
#define	MgvCShowLabels	"ShowLabels"

#define	MgvNshowPageNumbers	"showPageNumbers"
#define	MgvCShowPageNumbers	"ShowPageNumbers"

#define	MgvNtempDir	"tempDir"
#define	MgvCTempDir	"TempDir"

#define	MgvNmagicScrolling	"magicScrolling"
#define	MgvCMagicScrolling	"MagicScrolling"

#define	MgvNsmoothScrolling	"smoothScrolling"
#define	MgvCSmoothScrolling	"SmoothScrolling"

#define	MgvNsmoothness	"smoothness"
#define	MgvCSmoothness	"Smoothness"

#define	MgvNmagstep	"magstep"
#define	MgvCMagstep	"Magstep"

#define	MgvNsmartSizing	"smartSizing"
#define	MgvCSmartSizing	"SmartSizing"

#define	MgvNwmWidth	"wmWidth"
#define	MgvCWmWidth	"WmWidth"

#define	MgvNwmHeight	"wmHeight"
#define	MgvCWmHeight	"WmHeight"

#define	MgvNscrollPercentage	"scrollPercentage"
#define	MgvCScrollPercentage	"ScrollPercentage"

#define	MgvNcopyStdin		"copyStdin"
#define	MgvCCopyStdin		"CopyStdin"

/*
 * strings
 */
#define	MgvNstringBadArgument	"stringBadArgument"
#define	MgvCStringBadArgument	"StringBadArgument"

#define	MgvNstringBadVersion	"stringBadVersion"
#define	MgvCStringBadVersion	"StringBadVersion"

#define	MgvNstringCantAllocPixmap	"stringCantAllocPixmap"
#define	MgvCStringCantAllocPixmap	"StringCantAllocPixmap"

#define	MgvNstringCantDecompress	"stringCantDecompress"
#define	MgvCStringCantDecompress	"StringCantDecompress"

#define	MgvNstringCantHideMenu	"stringCantHideMenu"
#define	MgvCStringCantHideMenu	"StringCantHideMenu"

#define	MgvNstringCantOpen	"stringCantOpen"
#define	MgvCStringCantOpen	"StringCantOpen"

#define	MgvNstringCantPrint	"stringCantPrint"
#define	MgvCStringCantPrint	"StringCantPrint"

#define	MgvNstringError		"stringError"
#define	MgvCStringError		"StringError"

#define	MgvNstringFileExists	"stringFileExists"
#define	MgvCStringFileExists	"StringFileExists"

#define	MgvNstringInterpDone	"stringInterpDone"
#define	MgvCStringInterpDone	"StringInterpDone"

#define	MgvNstringInterpFailed	"stringInterpFailed"
#define	MgvCStringInterpFailed	"StringInterpFailed"

#define	MgvNstringMessage	"stringMessage"
#define	MgvCStringMessage	"StringMessage"

#define	MgvNstringNoCoordinates	"stringNoCoordinates"
#define	MgvCStringNoCoordinates	"StringNoCoordinates"

#define	MgvNstringNoDate	"stringNoDate"
#define	MgvCStringNoDate	"StringNoDate"

#define	MgvNstringNoFile	"stringNoFile"
#define	MgvCStringNoFile	"StringNoFile"

#define	MgvNstringNoPrintCommand	"stringNoPrintCommand"
#define	MgvCStringNoPrintCommand	"StringNoPrintCommand"

#define	MgvNstringPageFinished	"stringPageFinished"
#define	MgvCStringPageFinished	"StringPageFinished"

#define	MgvNstringUnknownSwitch	"stringUnknownSwitch"
#define	MgvCStringUnknownSwitch	"StringUnknownSwitch"

#define	MgvNstringPS	"stringPS"
#define	MgvCStringPS	"StringPS"

#define	MgvNstringEPS	"stringEPS"
#define	MgvCStringEPS	"StringEPS"

#define	MgvNstringPDF	"stringPDF"
#define	MgvCStringPDF	"StringPDF"

#define	MgvNstringUnknownType	"stringUnknownType"
#define	MgvCStringUnknownType	"StringUnknownType"

#define	MgvNstringUnknownMedia	"stringUnknownMedia"
#define	MgvCStringUnknownMedia	"StringUnknownMedia"

#define	MgvNstringUnknown	"stringUnknown"
#define	MgvCStringUnknown	"StringUnknown"

#endif
