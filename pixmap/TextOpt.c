/*
 * $Id: TextOpt.c,v 1.6 1994/05/13 14:14:41 mallet Exp $
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


extern void unsetKillfromWM();


static void PopupTextOptions(button, text_options)
    Widget      button;
    TextOptions *text_options;
{
    Position	   x, y;
    Dimension	   width, height;

    if (!text_options->up) {

        XtVaGetValues( button, 
                       XtNwidth,  &width,
                       XtNheight, &height,
                       NULL );
        
        XtTranslateCoords(button, 
			  (Position) (width / 2), (Position) (height / 2),
			  &x, &y );
    
        XtVaSetValues( text_options->shell, 
                       XtNx, x,
                       XtNy, y,
                       NULL );

#ifndef USE_ATHENA
	XtManageChild(text_options->popup);
#else
	XtPopup(text_options->popup, XtGrabNone);
#endif
    unsetKillfromWM(text_options->popup);
    text_options->up = True;
    } else text_options->up = False;

}


static void PopdownTextOptions( w, client_data, call_data )
    Widget	w;		/* button of form of popup */
    XtPointer	client_data;	/* TextOptions* */
    XtPointer	call_data;	/* unused */
{
    TextOptions *text_options = (TextOptions *) client_data;
#ifndef USE_ATHENA
    XtUnmanageChild(text_options->popup);
#else
    XtPopdown(text_options->popup);
#endif
}


static void Apply( w, client_data, call_data )
    Widget	w;		/* apply button */
    XtPointer	client_data;	/* TextOptions* */
    XtPointer	call_data;	/* unused */
{
    TextOptions *text_options = (TextOptions *) client_data;
    String	new_font_name = NULL;
    String	new_text_string = NULL;
    
    /* get new font name and new text string */
#ifndef USE_ATHENA
    /* Big hack for Motif: avoid arbitrary resizing problems */
    Dimension width, height;
    XtVaGetValues(text_options->popup, XmNwidth, &width, XmNheight, &height,
		  NULL);
    
    new_font_name = (String) XmTextFieldGetString(text_options->font);
    new_text_string = (String) XmTextFieldGetString(text_options->text);
    
#else
    XtVaGetValues( text_options->font, 
                   XtNstring, &new_font_name, 
                   NULL );
    new_font_name = XtNewString( new_font_name );

    XtVaGetValues( text_options->text, 
                   XtNstring, &new_text_string, 
                   NULL );
    new_text_string = XtNewString( new_text_string );
#endif

    /* if new font name != current font name, ... */

    if (strcmp( new_font_name, text_options->font_name )) {

        Display     *dpy      = XtDisplay(text_options->popup);
        XFontStruct *new_font = XLoadQueryFont( dpy, new_font_name );

        if (new_font) {
#ifndef USE_ATHENA
	    XmString xmstr;

	    xmstr = XmStringCreateLtoR("Valid font, text ready to paste",
				       XmSTRING_DEFAULT_CHARSET);
	    XtVaSetValues(text_options->status, XmNlabelString, xmstr, NULL);
	    XmStringFree(xmstr);
	    
	    /* Big hack for Motif: avoid arbitrary resizing problems */
	    XtVaSetValues(text_options->shell, XmNwidth, width,
			  XmNheight, height, NULL);
#else    
            XtVaSetValues(text_options->status, 
			  XtNlabel, "Valid font, text ready to paste",
			  NULL );
#endif
        }
        else {
#ifndef USE_ATHENA
	    XmString xmstr;

	    xmstr = XmStringCreateLtoR("No font with this name!",
				       XmSTRING_DEFAULT_CHARSET);
	    XtVaSetValues(text_options->status, XmNlabelString, xmstr, NULL);
	    XmStringFree(xmstr);
	    
	    /* Big hack for Motif: avoid arbitrary resizing problems */
	    XtVaSetValues(text_options->shell, XmNwidth, width,
			  XmNheight, height, NULL);
#else    
            XtVaSetValues( text_options->status, 
                           XtNlabel, "No font with this name!", 
                           NULL );
#endif
        }

        if (text_options->font_struct)
            XFreeFont( dpy, text_options->font_struct );
        XtFree( text_options->font_name );

        text_options->font_struct = new_font;
        text_options->font_name   = XtNewString( new_font_name );

        PWSetFont( pixmap_widget, text_options->font_struct );
    }

    /* if new text string != current text string, ... */
    if (strcmp( new_text_string, text_options->text_string ) != 0) {

        XtFree( text_options->text_string );
        text_options->text_string = XtNewString( new_text_string );
        PWSetText( pixmap_widget, text_options->text_string );
    }

#ifndef USE_ATHENA
#endif

    XtFree( new_font_name );
    XtFree( new_text_string );
}

#ifdef USE_ATHENA
static void ApplyAction( w, event, params, num_params )
    Widget	w;		/* either text or font widget */
    XEvent	*event;
    String	*params;
    Cardinal	*num_params;
{
    /* how to get TextOptions data structure ? */
    /* oh, just use that global pointer */
    Apply( w, (XtPointer) textOptions, (XtPointer) NULL );
}


static XtActionsRec actions[] = {
  { "apply", ApplyAction }
};
#endif

TextOptions* CreateTextOptions(parent, name ) 
    Widget	parent;
    String	name;
{
    Widget	popup, shell, textOptions, 
		title, 
		status, 
		font_label, font, 
		text_label, text, 
		close, apply;
    String	font_name, text_string;
    
    TextOptions *text_options = (TextOptions *)XtCalloc(1,
							sizeof(TextOptions));

#ifdef USE_ATHENA
    XtAppAddActions(pixmap_context, actions, XtNumber(actions));
#endif

#ifndef USE_ATHENA
    textOptions = XmCreateFormDialog(parent, name, NULL, 0);
    popup = textOptions;
    shell = XtParent(popup);
    
    title = XmCreateLabelGadget(textOptions, "title", NULL, 0);
    XtManageChild(title);

    status = XmCreateLabelGadget(textOptions, "fontStatus", NULL, 0);
    XtManageChild(status);

    font_label = XmCreateLabelGadget(textOptions, "fontLabel", NULL, 0);
    XtManageChild(font_label);

    font = (Widget) XmCreateTextField(textOptions, "fontName", NULL, 0);
    XtManageChild(font);
    XtAddCallback(font, XmNactivateCallback, Apply, (XtPointer)text_options);

    text_label = XmCreateLabelGadget(textOptions, "textLabel", NULL, 0);
    XtManageChild(text_label);
    
    text = (Widget) XmCreateTextField(textOptions, "textString", NULL, 0);
    XtManageChild(text);
    XtAddCallback(text, XmNactivateCallback, Apply, (XtPointer)text_options);
    
    close = XmCreatePushButtonGadget(textOptions, "close", NULL, 0);
    XtManageChild(close);
    XtAddCallback(close, XmNactivateCallback, PopdownTextOptions,
		  (XtPointer)text_options);

    apply = XmCreatePushButtonGadget(textOptions, "apply", NULL, 0);
    XtManageChild(apply);
    XtAddCallback(apply, XmNactivateCallback, Apply, (XtPointer)text_options);
    
#else
    popup = XtCreatePopupShell(
			"textOptionsShell", transientShellWidgetClass, 
			parent,
			NULL, 0 );
    shell = popup;
    
    textOptions = XtCreateManagedWidget( 
			"textOptions", formWidgetClass, 
			popup,
			NULL, 0 );

    title = XtCreateManagedWidget(
			"title", labelWidgetClass, 
			textOptions,
			NULL, 0 );

    status = XtCreateManagedWidget(
			"fontStatus", labelWidgetClass, 
			textOptions,
			NULL, 0 );

    font_label = XtCreateManagedWidget(
			"fontLabel", labelWidgetClass, 
			textOptions,
			NULL, 0 );

    font = XtCreateManagedWidget(
			"fontName", asciiTextWidgetClass, 
			textOptions,
			NULL, 0 );

    text_label = XtCreateManagedWidget(
			"textLabel", labelWidgetClass, 
			textOptions,
			NULL, 0 );

    text = XtCreateManagedWidget(
			"textString", asciiTextWidgetClass, 
			textOptions,
			NULL, 0 );

    close = XtCreateManagedWidget(
			"close", commandWidgetClass, 
			textOptions,
			NULL, 0 );
    XtAddCallback(close, XtNcallback, PopdownTextOptions,
		  (XtPointer)text_options);

    apply = XtCreateManagedWidget(
			"apply", commandWidgetClass, 
			textOptions,
			NULL, 0 );
    XtAddCallback(apply, XtNcallback, Apply, (XtPointer) text_options);
#endif

    text_options->up     = False;
    text_options->popup  = popup;
    text_options->shell  = shell;
    text_options->text   = text;
    text_options->font   = font;
    text_options->status = status;

#ifndef USE_ATHENA
    text_options->font_name = (String)XmTextFieldGetString(text_options->font);
#else
    XtVaGetValues( text_options->font, 
                   XtNstring, &font_name, 
                   NULL );
    text_options->font_name = XtNewString( font_name );
#endif
    text_options->font_struct = XLoadQueryFont(XtDisplay(parent),
					       text_options->font_name);

#ifndef USE_ATHENA
    text_options->text_string=(String)XmTextFieldGetString(text_options->text);
#else
    XtVaGetValues( text_options->text, 
                   XtNstring, &text_string, 
                   NULL );
    text_options->text_string = XtNewString(text_string);
#endif

    
    return text_options;
}

