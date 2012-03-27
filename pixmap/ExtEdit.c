/*
 * $Id: ExtEdit.c,v 1.6 1994/05/13 14:11:44 mallet Exp $
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


/*--------------------------------------------------------------------------*/
/*
				ExtEditor.c		

	This file contains routines used for the xpm extension editor
	of pixmap.

*/
/*--------------------------------------------------------------------------*/

#define NEWLINE '\n'

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

extern void unsetKillfromWM();

/*--------------------------------------------------------------------------*/
/*
		X p m E x t e n s i o n T o S t r i n g

	Convert an xpm extension to a string. Lines of the extension
	are separated by newlines in the string.
*/
/*--------------------------------------------------------------------------*/
String XpmExtensionToString( ext )
    XpmExtension *ext;
{
    String s = NULL;
    int    i, len;

    if (!ext)
        return NULL;

    if (ext->nlines == 0) 
        return "";

    for ( i=0, len = 0; i < ext->nlines; i++)
        len += strlen( ext->lines[i] ) + 1; /* plus one for newline or NULL */

    s = XtCalloc( len, sizeof(char) );

    for ( i=0; i < ext->nlines - 1; i++ ) {
        strcat( s,  ext->lines[i] );
        strcat( s, "\n" );
    }
    strcat( s,  ext->lines[i] );

    return s;
}


/*--------------------------------------------------------------------------*/
/*
		S t r i n g T o X p m E x t e n s i o n

	Convert to a string to an xpm extension.  newlines in the
	string cause a new line of the extension to be created.
*/
/*--------------------------------------------------------------------------*/
XpmExtension *StringToXpmExtension( s )
    String       s;
{
    XpmExtension *ext;
    String       beg, end;
    int          i, len, n;

    if (!s)
        return NULL;

    ext  = (XpmExtension *) XtCalloc(1, sizeof(XpmExtension));

    /* count nlines (number of newlines + 1) */
    n = 0;
    beg = s;
    for( end=index(beg, NEWLINE); end; end=index(beg, NEWLINE) ) {
        n++; 
        beg = end + 1;
    }
    n++;	

    /* assert: n > 0 */

    ext->nlines = n; 
    ext->lines  = (char **) XtCalloc(n, sizeof(char *));

    /* do newline terminated lines */
    beg = s;
    for ( i=0; i < n - 1; i++ ) {

        end = index(beg, NEWLINE);
        len = end - beg;
        ext->lines[i] = (char *) XtCalloc(len + 1, sizeof(char));

        strncpy( ext->lines[i], beg, len );

        beg = end + 1;
    }

    /* last line is null terminated */
    ext->lines[i] = (char *) XtCalloc(strlen(beg) + 1, sizeof(char));
    strcpy( ext->lines[i], beg );

    return ext;
}


/*--------------------------------------------------------------------------*/
/*
			P o s i t i o n P o p u p

	Position a popup in the center of its parent. This is the same
	code as is used in Dialog.c/PopupDialog(). 
*/
/*--------------------------------------------------------------------------*/
void PositionPopup( popup )
    Widget popup;		/* shell widget to postion */
{
    Widget	parent = XtParent( popup );

    Position	parentX, parentY; 
    Dimension	parentWidth, parentHeight;

    Position	popupX, popupY;
    Dimension	popupWidth, popupHeight, popupBorder;

    Dimension	dpyWidth, dpyHeight;
    Display     *dpy;

    int		n;
    Arg		args[4];

    n = 0;
    XtSetArg( args[n], XtNx,      &parentX      ); n++;
    XtSetArg( args[n], XtNy,      &parentY      ); n++;
    XtSetArg( args[n], XtNwidth,  &parentWidth  ); n++;
    XtSetArg( args[n], XtNheight, &parentHeight ); n++;

    XtGetValues( parent , args, n );

    n = 0;
    XtSetArg( args[n], XtNwidth,       &popupWidth  ); n++;
    XtSetArg( args[n], XtNheight,      &popupHeight ); n++;
    XtSetArg( args[n], XtNborderWidth, &popupBorder ); n++;

    XtGetValues( popup, args, n );

    dpy       = XtDisplay(popup);
    dpyWidth  = (Position) DisplayWidth ( dpy, DefaultScreen(dpy) );
    dpyHeight = (Position) DisplayHeight( dpy, DefaultScreen(dpy) );

    popupX = 
      max( 0,
           min( parentX + ((Position)parentWidth - (Position)popupWidth) / 2,
                dpyWidth - (Position)popupWidth - 2 * (Position)popupBorder 
         ));

    popupY = 
      max( 0,
           min( parentY + ((Position)parentHeight - (Position)popupHeight) / 2,
                dpyHeight - (Position)popupHeight - 2 * (Position)popupBorder 
          ));

    n = 0;
    XtSetArg( args[n], XtNx, popupX); n++;
    XtSetArg( args[n], XtNy, popupY); n++;

    XtSetValues( popup, args, n );
}

/*--------------------------------------------------------------------------*/
/*
				P o p u p

	Position a shell relative to parent then pop it up.
*/
/*--------------------------------------------------------------------------*/
static void Popup( popup, grab )
    Widget	popup;
    XtGrabKind	grab;
{
    /* position shell relative to parent */
    PositionPopup( popup );

#ifndef USE_ATHENA
    XtManageChild(popup);
#else
    XtPopup( popup, grab );
#endif
}



/*--------------------------------------------------------------------------*/
/*
				P o p d o w n 

	A callback to popdown a given shell.
*/
/*--------------------------------------------------------------------------*/
/* ARGSUSED */
static void Popdown( w, client_data, call_data )
    Widget	w;		/* unused */
    XtPointer	client_data;	/* child of shell to popdown */
    XtPointer	call_data;	/* unused */
{
#ifndef USE_ATHENA
    XtUnmanageChild((Widget) client_data);
#else
    XtPopdown( XtParent((Widget) client_data) );
#endif
}


/*--------------------------------------------------------------------------*/
/*
		P o p u p E x t e n s i o n E d i t o r

	Popup the xpm extension editor form using the given name
	and text.
*/
/*--------------------------------------------------------------------------*/
static void PopupExtensionEditor( formW, name, text )
    Widget	formW;
    String	name;
    String	text;
{
    Widget	textW = XtNameToWidget( formW, "*text" );
    Widget	nameW = XtNameToWidget( formW, "name" );
    Arg		args[1];
#ifndef USE_ATHENA
    XmString xmstr;
    
    XmTextSetString(textW, text);

    xmstr = XmStringCreateLtoR(name, XmSTRING_DEFAULT_CHARSET);
    XtSetArg(args[0], XmNlabelString, xmstr);
    XtSetValues( nameW, args, 1 );
    XmStringFree(xmstr);
    
    Popup(formW, XtGrabExclusive);
#else
    XtSetArg( args[0], XtNstring, text );
    XtSetValues( textW, args, 1 );

    XtSetArg( args[0], XtNlabel, name );
    XtSetValues( nameW, args, 1 );
    Popup( XtParent(formW), XtGrabExclusive );
#endif
}


/*--------------------------------------------------------------------------*/
/*
			D o E d i t E x t e n s i o n

	Given an xpm extension name, get the extension text from the 
	pixmap widget and popup it up in a form for the user to change.
*/
/*--------------------------------------------------------------------------*/
void DoEditExtension( name )
    String name; 
{

    if ( name != NULL && strcmp(name, "") != 0 ) {

        XpmExtension *ext = PWFindExtension( pixmap_widget, name );

        String text;
        if ( ext == NULL || ext->nlines == 0 ) 
            text = "";
        else
            text = XpmExtensionToString( ext );

        PopupExtensionEditor( extensionEditor, name, text );
    }
}

/*--------------------------------------------------------------------------*/
/*
                      A d d T o E x t e n s i o n M e n u
      Add given name to extension menu.
*/

void AddToExtensionMenu (name)
    char *name;
{
    void ExtensionMenuCallback();
#ifndef USE_ATHENA
    Widget item = XmCreatePushButtonGadget(extensionMenu_widget,
					   name, NULL, 0);
    XtManageChild(item);
    XtAddCallback(item, XmNactivateCallback,
		  ExtensionMenuCallback, (XtPointer) NULL );
#else
    Widget item = XtCreateManagedWidget(name,
					smeBSBObjectClass,
					extensionMenu_widget,
					NULL, 0 );
    XtAddCallback(item, XtNcallback, ExtensionMenuCallback,
		  (XtPointer) NULL );
#endif
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
			D o A d d E x t e n s i o n

	Prompt user for name of new xpm extension then popup the editor
	for the user to supply the text for the extension.
*/
/*--------------------------------------------------------------------------*/
void DoAddExtension()
{
    char *name = NULL;

    if ( PopupDialog( input_dialog, 
                      "Add extension name:", 
                      "", &name,
                      XtGrabExclusive ) == Okay ) {

        if ( name != NULL && strcmp( name, "" ) != 0 ) {

            /* add to menu, if necessary */
            if ( ! XtNameToWidget(extensionMenu_widget, name) )
		AddToExtensionMenu (name);

            PWAddExtension( pixmap_widget, name );  /* add, if necessary */

            DoEditExtension( name );
        }
    }
}
/*--------------------------------------------------------------------------*/
/*
                      F i x E x t e n s i o n M e n u
        The list of extensions in a pixmap widget has been changed.
        Update the extension menu by destroying the old menu and
        recreating it from scratch.
*/
/*--------------------------------------------------------------------------*/
void FixExtensionMenu (w)
    Widget w;
{
    void ExtensionMenuCallback();

    char **PWGetExtensionNames();
    char **names;
    char **name;
    Widget *children;
    Cardinal num_children, i;
    Arg arglist[2];
    
#ifndef USE_ATHENA
    XtSetArg(arglist[0], XmNchildren, &children);
    XtSetArg(arglist[1], XmNnumChildren, &num_children);
#else
    XtSetArg(arglist[0], XtNchildren, &children);
    XtSetArg(arglist[1], XtNnumChildren, &num_children);
#endif
    XtGetValues(extensionMenu_widget, arglist, 2);
    
    /* destroy all children */
    for (i = 0; i < num_children; i++)
      XtDestroyWidget(*(children + i));
	
    /* build again base extension menu */
    MakeMenuPanel(extensionMenu_widget, extension_menu,
		  XtNumber(extension_menu), ExtensionMenuCallback);
    
    names = PWGetExtensionNames (w);
    if (names) {
        for (name = names; name && *name; name++) {
           AddToExtensionMenu (*name);
           XtFree (*name);
        }
        if (names)
            XtFree ((char *)names);
    }
}


/*--------------------------------------------------------------------------*/
/*
			D o R e m o v e E x t e n s i o n

	Callback for extension editor form Remove button. Get the name
	of the extension from the form then remove the extension from
	the pixmap widget and from the menu of extension names.
*/
/*--------------------------------------------------------------------------*/
void DoRemoveExtension( w, client_data, call_data )
    Widget	w;		/* unused */
    XtPointer	client_data;	/* extension editor form */
    XtPointer	call_data;	/* unused */
{
    Widget       formW = (Widget) client_data;
    Widget       nameW = XtNameToWidget( formW, "name" );
    String       name  = NULL; 

    Popdown( NULL, (XtPointer) formW, NULL );

    /* get name of extension */
#ifndef USE_ATHENA
    {
	XmString name_xms;
	XtVaGetValues(nameW, XmNlabelString, &name_xms, NULL);

	XmStringGetLtoR(name_xms, XmSTRING_DEFAULT_CHARSET, &name);
    }
#else
    XtVaGetValues( nameW, 
                   XtNlabel, &name, 
                   NULL );
#endif

    PWRemoveExtension( pixmap_widget, name ); 

    /* remove name from menu */
    {
      Widget item = XtNameToWidget(extensionMenu_widget, name);
      XtDestroyWidget( item );
    }
}


/*--------------------------------------------------------------------------*/
/*
			D o S a v e E x t e n s i o n

	Callback for the Okay button of the extension editor form.
	Save the text of the extension with the pixmap widget.
*/
/*--------------------------------------------------------------------------*/
void DoSaveExtension( w, client_data, call_data )
    Widget	w;		/* unused */
    XtPointer	client_data;	/* extension editor form */
    XtPointer	call_data;	/* unused */
{
    Widget       formW = (Widget) client_data;
    Widget       nameW = XtNameToWidget( formW, "name" );
    Widget       textW = XtNameToWidget( formW, "*text" );

    String       name = NULL; 
    String       text = NULL;

    Popdown( NULL, (XtPointer) formW, NULL );

#ifndef USE_ATHENA
    {
	XmString name_xms;
	XtVaGetValues(nameW, XmNlabelString, &name_xms, NULL);

	XmStringGetLtoR(name_xms, XmSTRING_DEFAULT_CHARSET, &name);

	text = (String) XmTextGetString(textW);
    }

#else
    XtVaGetValues( textW, 
                 XtNstring, &text,
                 NULL );

    XtVaGetValues( nameW, 
                   XtNlabel, &name,
                   NULL );
#endif

/*
    if ( strcmp(text, "") ==  0 ) {
        PWRemoveExtension( pixmap_widget, name );
    }
    else 
*/
    {
        XpmExtension *ext = StringToXpmExtension( text );
	ext->name = XtNewString( name );

        PWAddExtension( pixmap_widget, name );	/* add if necessary */
        PWUpdateExtension( pixmap_widget, ext );
    }
}



/*--------------------------------------------------------------------------*/
/*
		D o S h o w C u r r e n t E x t e n s i o n s

	Print names of xpm extensions in pixmap widget.
*/
/*--------------------------------------------------------------------------*/
void DoShowCurrentExtensions()
{
    char **extension_names;
    char **name;

    extension_names = PWGetExtensionNames( pixmap_widget );

    if ( extension_names != NULL ) {
        fprintf( stderr, "Extension names:\n" );
        for ( name = extension_names; *name != NULL; name++ ) 
            fprintf( stderr, "\t%s\n", *name );
    }
}



/*--------------------------------------------------------------------------*/
/*
		C r e a t e E x t e n s i o n E d i t o r

	The xpm extension editor is a form with a label for the extension
	name, and a text box for the extension text.  

	Buttons:
	- ok : save text.
	- cancel : ignore any changes made to text.
        - remove : delete extension from pixmap widget and menu.
*/
/*--------------------------------------------------------------------------*/
static Widget CreateExtensionEditor( topWidget, editorName )
    Widget	topWidget;	
    String	editorName;
{
    Widget	popup, form, name, text, ok, cancel, remove;

#ifndef USE_ATHENA
    popup = XmCreateFormDialog(top_widget, editorName, NULL, 0);
    form = popup;

    name = XmCreateLabelGadget(form, "name", NULL, 0);
    XtManageChild(name);

    text = (Widget) XmCreateScrolledText(form, "text", NULL, 0);
    XtManageChild(text);

    ok = XmCreatePushButtonGadget(form, "ok", NULL, 0);
    XtManageChild(ok);
    XtAddCallback(ok, XmNactivateCallback, DoSaveExtension, (XtPointer)form);

    cancel = XmCreatePushButtonGadget(form, "cancel", NULL, 0);
    XtManageChild(cancel);
    XtAddCallback(cancel, XmNactivateCallback, Popdown, (XtPointer)form);

    remove = XmCreatePushButtonGadget(form, "remove", NULL, 0);
    XtManageChild(remove);
    XtAddCallback(remove, XmNactivateCallback, DoRemoveExtension,
		  (XtPointer)form);

#else
    popup = XtCreatePopupShell( 
		"extEditorShell", transientShellWidgetClass, 
		top_widget, 
		NULL, 0 );

    form = XtCreateManagedWidget( 
		editorName, formWidgetClass, 
		popup,
		NULL, 0 );

    name = XtCreateManagedWidget(
		"name", labelWidgetClass, 
		form,
		NULL, 0 );

    text = XtCreateManagedWidget(
		"text", asciiTextWidgetClass, 
		form,
		NULL, 0 );

    ok = XtCreateManagedWidget(
		"ok", commandWidgetClass, 
		form,
		NULL, 0);
    XtAddCallback( ok, XtNcallback, 
                   DoSaveExtension, (XtPointer) form );

    cancel = XtCreateManagedWidget(
		"cancel", commandWidgetClass, 
		form,
		NULL, 0);
    XtAddCallback( cancel, XtNcallback, Popdown, (XtPointer) form );

    remove = XtCreateManagedWidget(
		"remove", commandWidgetClass, 
		form,
		NULL, 0);
    XtAddCallback( remove, XtNcallback, DoRemoveExtension, (XtPointer) form );
#endif

    return form;
}


/*--------------------------------------------------------------------------*/
/*
		E x t e n s i o n M e n u C a l l b a c k

	Callback for items on the xpm extension menu. Check for adding
	new item to menu, otherwise invoke the extension editor for the
	chosen extension name.
*/
/*--------------------------------------------------------------------------*/
void ExtensionMenuCallback(w, client_data, call_data)
     Widget w;
     XtPointer client_data, call_data;
{
    int   *id = (int *)client_data;

    if (id && *id == AddExtension)
        DoAddExtension(); 		/* add then edit */
    else
	DoEditExtension( XtName(w) );
}

