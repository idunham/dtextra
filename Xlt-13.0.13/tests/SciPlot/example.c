/*
 * main.c 
 * Where the whole enchillada kicks off
 * $Id: example.c,v 1.4 2001/04/28 16:24:29 rwscott Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>              /* Motif Toolkit */
#include <Mrm/MrmPublic.h>      /* Mrm Toolkit */
#include <X11/Intrinsic.h>

#include "SciPlotP.h"


/* time in ms */
#define DELTAT 200

static void counter(XtPointer client_data, XtIntervalId *id);

#define NEW_BUTTON_X 11
/* not mentioning hardcoding labels... look at hellomotif_i18n for a
   egisterArgmple  */
#define NEW_BUTTON_LABEL  "Goodbye\nWorld!"

static String uid_vec[]={"app.uid"}; /* MRM database file list   */
static MrmRegisterArg   reg_vec[] = {       /* Callback bindings */
    {"exit", (XtPointer)exit}
};


static XtAppContext  app_context = NULL;

int main(int argc, char **argv)
{
   Widget top_level, app_main;
   Widget plotwidget;
   MrmCode    mrm_class ;
   MrmHierarchy  mrm_hierarchy;

   double y[10];
   double x[10];
   int i;

   for(i=0;i<10;i++) {
      x[i]=i;
      y[i]=sin(i);
   }
 /*
  *  Initialize the MRM
  */

   MrmInitialize ();

 /*
  * Initialize Xt and create a resizable shell
  */

   top_level = XtVaAppInitialize(&app_context, "SciPlot",
              NULL, 0, &argc, argv, NULL,
              XmNallowShellResize, False, NULL);

  /*
   *  Create the Mrm hierarchy
   */

   if (MrmOpenHierarchy (XtNumber(uid_vec),     /* number of files      */
           uid_vec,         /* files          */
           NULL,            /* os_ext_list (null)   */
           &mrm_hierarchy)     /* ptr to returned id   */
   != MrmSUCCESS) {
   XtError ("can't open hierarchy\n");
   }
    /*
     *   Register our callback routines so that the resource manager can
     *   resolve them at widget-creation time.
     */

    if (MrmRegisterNames (reg_vec, XtNumber(reg_vec)) != MrmSUCCESS)
   XtError("can't register names\n");

    if (MrmRegisterClass(MrmwcUnknown, "SciPlot" , "SciPlotCreate",
                            (Widget(*)())SciPlotCreate,
                            (WidgetClass)&sciplotClassRec) != MrmSUCCESS)

       XtError("Can't register Sciplot class\n");
    /*
     *  Call MRM to fetch and create the widgets
     */
    if (MrmFetchWidget (mrm_hierarchy,
         "app_main",
         top_level,
         &app_main,
         &mrm_class)
   != MrmSUCCESS)
   XtError ("can't fetch interface\n");

    XtManageChild(app_main);
    XtRealizeWidget(top_level);


    plotwidget=XtNameToWidget(app_main, "myform.plot");
    if(plotwidget) {
       SciPlotSetYUserScale(plotwidget, -1.0, 1.0);
       SciPlotUpdate(plotwidget);
    }
    
   /*
     *  Loop and process events
     */

    XtAppAddTimeOut(app_context, DELTAT, counter, app_main);

    XtAppMainLoop(app_context);

    /* UNREACHABLE */

   printf("Done.\n");
   return 0;
}


static void counter(XtPointer client_data, XtIntervalId *id)
{
  Widget plotwidget;
  static double x=0;
  double y=0;
  static int nlist=-1;

  plotwidget=XtNameToWidget(client_data, "myform.plot");

  if(!plotwidget)
     return;

  if(nlist==-1) {
     nlist=SciPlotListCreateFromDouble(plotwidget, 1, &x, &y, "Blah");
     SciPlotListSetStyle(plotwidget,nlist,1,XtMARKER_NONE,1,XtLINE_SOLID);
  } else {
     y=sin(x);
     SciPlotListAddDouble(plotwidget,nlist,1,&x,&y);
     x+=0.2;
  }
  if(SciPlotQuickUpdate(plotwidget)) {
     SciPlotUpdate(plotwidget);
  }
  XtAppAddTimeOut(app_context, DELTAT, counter, client_data);
  return;
}
