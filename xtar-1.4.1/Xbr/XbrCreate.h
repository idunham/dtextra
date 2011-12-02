/* 
 * Copyright 1993, 1994 Liverpool University Computer Science Department
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appear in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation, and that the name of L.U.C.S. not be used in advertising
 * or publicity pertaining to distribution of the software without specific, 
 * written prior permission. L.U.C.S. makes no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FILE NAME:	XbrCreate.h
 * CREATED:	Fri Nov  5 1993
 * AUTHOR:	Rik Turnbull (rik@csc.liv.ac.uk)
 * DESCRIPTION:	Header file for XbrCreate.c
 *
 */

#ifndef	XBRLIB_XBRCREATE_H
#define	XBRLIB_XBRCREATE_H		1

extern Widget XbrCreateList(Widget, unsigned char, unsigned char, unsigned char,
  int, unsigned char, int, unsigned char, int, unsigned char, int);

extern Widget XbrCreateText(Widget, Boolean, Boolean, Boolean, unsigned char,
  unsigned char, int, unsigned char, int, unsigned char, int, unsigned char,
  int);

#define XbrCreateForm(w) \
    XtVaCreateManagedWidget("Form", xmFormWidgetClass, w,              \
      XmNmarginWidth, 5, XmNmarginHeight, 5, NULL)

#define XbrCreateFormWH(w, width, height) \
    XtVaCreateManagedWidget("Form", xmFormWidgetClass, w,                \
      XmNmarginWidth, 5, XmNmarginHeight, 5, XmNwidth, width, XmNheight, \
      height, NULL)

#define XbrCreateMainW(w, seps, width, height) \
    XtVaCreateManagedWidget("Main Window", xmMainWindowWidgetClass, w, \
      XmNshowSeparator, seps, XmNwidth, width, XmNheight, height,      \
      NULL)

#define XbrCreateLabelN(w, name, string, alignment, leftAt, leftPos, rightAt, \
    rightPos, topAt, topPos, botAt, botPos)                            \
    XtVaCreateManagedWidget(name, xmLabelWidgetClass, w,               \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,         \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,        \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,          \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,          \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),  \
     XmNalignment, alignment, NULL)

#define XbrCreateLabel(w, string, alignment, leftAt, leftPos, rightAt, \
    rightPos, topAt, topPos, botAt, botPos)                            \
    XtVaCreateManagedWidget("Label", xmLabelWidgetClass, w,            \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,         \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,        \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,          \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,          \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),  \
     XmNalignment, alignment, NULL)

#define XbrCreatePBN(w, name, string, alignment, leftAt, leftPos, rightAt,    \
    rightPos, topAt, topPos, botAt, botPos)                                   \
   XtVaCreateManagedWidget(name, xmPushButtonWidgetClass, w,                  \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,                \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,               \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,                 \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,                 \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),         \
     XmNalignment, alignment, NULL)

#define XbrCreatePB(w, string, alignment, leftAt, leftPos, rightAt, rightPos, \
   topAt, topPos, botAt, botPos)                                              \
   XtVaCreateManagedWidget("Push Button", xmPushButtonWidgetClass, w,         \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,                \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,               \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,                 \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,                 \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),         \
     XmNalignment, alignment, NULL)

#define XbrCreateToggleN(w, name, string, set, alignment, leftAt, leftPos,    \
   rightAt,  rightPos, topAt, topPos, botAt, botPos)                          \
   XtVaCreateManagedWidget(name, xmToggleButtonWidgetClass, w,                \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,                \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,               \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,                 \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,                 \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),         \
     XmNalignment, alignment,      XmNset,            set, NULL)

#define XbrCreateToggle(w, string, set, alignment, leftAt, leftPos, rightAt,  \
   rightPos, topAt, topPos, botAt, botPos)                                    \
   XtVaCreateManagedWidget("Toggle Button", xmToggleButtonWidgetClass, w,     \
     XmNleftAttachment,   leftAt,  XmNleftPosition,   leftPos,                \
     XmNrightAttachment,  rightAt, XmNrightPosition,  rightPos,               \
     XmNtopAttachment,    topAt,   XmNtopPosition,    topPos,                 \
     XmNbottomAttachment, botAt,   XmNbottomPosition, botPos,                 \
     XtVaTypedArg, XmNlabelString, XmRString, string, sizeof(char *),         \
     XmNalignment, alignment,      XmNset,            set, NULL)

#define XbrCreateSep(w, style, leftAt, leftPos, rightAt,               \
   rightPos, topAt, topPos, botAt, botPos)                             \
   XtVaCreateManagedWidget("Separator", xmSeparatorWidgetClass, w,     \
     XmNseparatorType,    style,                                       \
     XmNleftAttachment,   leftAt,  XmNleftPosition,          leftPos,  \
     XmNrightAttachment,  rightAt, XmNrightPosition,         rightPos, \
     XmNtopAttachment,    topAt,   XmNtopPosition,           topPos,   \
     XmNbottomAttachment, botAt,   XmNbottomPosition,        botPos,   \
     NULL)

#define XbrCreateTextF(w, string, editable, leftAt, leftPos, rightAt,  \
   rightPos, topAt, topPos, botAt, botPos)                             \
   XtVaCreateManagedWidget("Text Field", xmTextFieldWidgetClass, w,    \
     XmNeditable,         editable,XmNcursorPositionVisible, editable, \
     XmNleftAttachment,   leftAt,  XmNleftPosition,          leftPos,  \
     XmNrightAttachment,  rightAt, XmNrightPosition,         rightPos, \
     XmNtopAttachment,    topAt,   XmNtopPosition,           topPos,   \
     XmNbottomAttachment, botAt,   XmNbottomPosition,        botPos,   \
     XmNvalue,            string,  NULL)

#define XbrCreateTextFN(w, name, string, editable, leftAt, leftPos, rightAt, \
   rightPos, topAt, topPos, botAt, botPos)                                   \
   XtVaCreateManagedWidget(name, xmTextFieldWidgetClass, w,                  \
     XmNeditable,         editable,XmNcursorPositionVisible, editable,       \
     XmNleftAttachment,   leftAt,  XmNleftPosition,          leftPos,        \
     XmNrightAttachment,  rightAt, XmNrightPosition,         rightPos,       \
     XmNtopAttachment,    topAt,   XmNtopPosition,           topPos,         \
     XmNbottomAttachment, botAt,   XmNbottomPosition,        botPos,         \
     XmNvalue,            string,  NULL)


#endif
