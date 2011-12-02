/********************************************************************
 *         This example code is from the book:
 *
 *           The X Window System: Programming and Applications with Xt
 *           Second OSF/Motif Edition
 *         by
 *           Douglas Young
 *           Prentice Hall, 1994
 *
 *         Copyright 1994 by Prentice Hall
 *         All Rights Reserved
 *
 *  Permission to use, copy, modify, and distribute this software for 
 *  any purpose except publication and without fee is hereby granted, provided 
 *  that the above copyright notice appear in all copies of the software.
 * ***************************************************************************/

/* Cascade support added and some lint-like modifications made - rocky. */

/******************************************************
 * MenuSupport.c: Convenience functions that make it 
 *                easier to create Motif menus
 ******************************************************/
#include <Xm/Xm.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include "MenuSupport.h"

/* 
 * Declarations of internal functions used in this package.
 */

static void PostMenuHandler ( Widget    w, 
                              XtPointer clientData,
                              XEvent    *event,
                              Boolean   *flag );

static void AttachPopup ( Widget parent, Widget menu );
static void CreateMenuChildren ( Widget           parent, 
                                 MenuDescription *desc,
                                 XtPointer        defaultClientData );
/*
 * External interface function, called by applications
 * to create option menus, popup menus, or menubars.
 */

Widget CreateMenu ( MenuType         menuType,
                    char            *name,
                    Widget           parent,
                    MenuDescription *desc,
                    XtPointer        defaultClientData ) 
{
    Widget w;

   /*
    * Create the basic menu widget, based on the type of
    * menu requested. Do any special setup required
    * and then call CreateMenuChildren() to populate the menu.
    */

    switch ( menuType ) 
    {
      case POPUP:

       /*
        * Popups must have an event handler registered with the
        * parent to display the menu when button 3 is pressed.
        */

        w = XmCreatePopupMenu ( parent, name, NULL, 0 );
    
        AttachPopup ( parent, w );

        CreateMenuChildren ( w, desc, defaultClientData );

        return ( w );

      case MENUBAR:

        /*
         * For menu bars, just create the menu bar, and then
         * add children. All direct children are expected
         * to be pulldown menus.
         */

         w = XmCreateMenuBar ( parent, name, NULL, 0 );

         CreateMenuChildren ( w, desc, defaultClientData );

         XtManageChild ( w );

         return ( w );
    
      case OPTION:
      {
        /*
         * All option menus have a pulldown, but it is nicer to hide
         * this, allowing the programmer to just specify the contents
         * of the pane. So, create the option menu, create and attach
         * a pulldown pane, and then call CreateMenuChildren() to
         * populate the pulldown menu pane.
         */

         Widget option = XmCreateOptionMenu ( parent, name, NULL, 0 );

         w = XmCreatePulldownMenu ( parent, name, NULL, 0 );

         XtVaSetValues ( option, XmNsubMenuId, w, NULL );

         CreateMenuChildren ( w, desc, defaultClientData );

         XtManageChild ( option );

         return ( option );
     }

      default:
        XtError ( "Invalid Menu Type" );
        return ( NULL );
    }
}

static void CreateMenuChildren ( Widget           parent, 
                                 MenuDescription *desc, 
                                 XtPointer        defaultClientData ) 
{
    Widget  w;
    int     i;

   /*
    * Create an entry for each item in the menu.
    */
    
    for ( i=0; desc[i].type != END; i++ )  
    {
        switch ( desc[i].type ) 
        {
          case LABEL:
        
            w = XtCreateManagedWidget ( desc[i].name,
                                        xmLabelWidgetClass,
                                        parent, NULL, 0 );
            
            break;

          case BUTTON:
        
           /*
            * Create a button, and add the specified callback.
            * If the item-specific client data is NULL, add
            * the default client data passed to this function.
            */

            w = XtCreateManagedWidget ( desc[i].name,
                                        xmPushButtonWidgetClass,
                                        parent, NULL, 0 );
            XtAddCallback ( w, 
                            XmNactivateCallback,
                            desc[i].func, 
                            desc[i].data ? 
                                    desc[i].data : 
                                    defaultClientData );
            break;

          case TOGGLE:

           /*
            * Toggles are handled just like buttons, except for
            * the name of the callback list.
            */
        
            w = XtCreateManagedWidget ( desc[i].name,
                                        xmToggleButtonWidgetClass,
                                        parent, NULL, 0 );
            XtAddCallback ( w, 
                            XmNvalueChangedCallback,
                            desc[i].func, 
                            desc[i].data ?
                                   desc[i].data :
                                   defaultClientData );
            break;


          case SEPARATOR:
            
            XtCreateManagedWidget ( "separator",
                                    xmSeparatorWidgetClass,
                                    parent, NULL, 0 );
            break;

          case PULLDOWN:
          {
            Widget pulldown;

           /*
            * A pulldown requires the creation of a pulldown
            * menu pane and a cascade button. Children are added to
            * the pane by calling this function recursively.
            */
            
            pulldown = XmCreatePulldownMenu ( parent,
                                              desc[i].name,
                                              NULL, 0 );
            
            w = XtVaCreateManagedWidget ( desc[i].name,
                                          xmCascadeButtonWidgetClass,
                                          parent, 
                                          XmNsubMenuId, pulldown,
                                          NULL );
            
            CreateMenuChildren ( pulldown, desc[i].subMenu, 
                                 defaultClientData );
            break;
          } 

          	case HELPPANE:
          	{
	            Widget pulldown;

	           /*
            * A help pane can be handled just like the pulldown case,
            * but the cascade button is used as the value of 
            * the menubar's XmNmenuHelpWidget resource.
            */

	            pulldown = XmCreatePulldownMenu ( parent, desc[i].name,
						      NULL, 0 );
	    
	            w = XtVaCreateManagedWidget ( desc[i].name,
						  xmCascadeButtonWidgetClass,
						  parent, 
						  XmNsubMenuId, pulldown,
						  NULL );

	            XtVaSetValues ( parent, XmNmenuHelpWidget, w, NULL );
	    
	            CreateMenuChildren ( pulldown, desc[i].subMenu, 
                                 defaultClientData );

	            break;
          	} 

         case RADIOPULLDOWN:
         {
            Widget pulldown;

           /*
            * A radio pulldown is handled just like the pulldown case,
            * but the menu pane is set to exhibit radio behavior.
            * Items added to this pane are expected to be toggles.
            */
            
            pulldown = XmCreatePulldownMenu ( parent, desc[i].name,
                                              NULL, 0 );

            XtVaSetValues ( pulldown, XmNradioBehavior, TRUE, NULL );
            
            w = XtVaCreateManagedWidget ( desc[i].name,
                                          xmCascadeButtonWidgetClass,
                                          parent, 
                                          XmNsubMenuId, pulldown,
                                          NULL );
            
            CreateMenuChildren ( pulldown, desc[i].subMenu,
                                 defaultClientData );
            break;
         } 
	case POPUP:
	case MENUBAR:
	case OPTION:
	case END:
	  {
	    XtError ( "Invalid Menu Type" );
	    break;
	  }
       }
    }
}

static void AttachPopup ( Widget parent, Widget menu ) 
{
    XtAddEventHandler ( parent, ButtonPressMask, FALSE,
                        PostMenuHandler, menu );
}

static void PostMenuHandler ( Widget w, XtPointer clientData,
                              XEvent *event, Boolean *flag ) 
{
    Widget menu = ( Widget )  clientData;

    if ( event->type == ButtonPress &&
         event->xbutton.button == Button3 ) 
    {
       /*
        * Position the menu over the pointer and post the menu.
        */

        XmMenuPosition ( menu, ( XButtonPressedEvent * )  event );
        XtManageChild ( menu );
    }
}
