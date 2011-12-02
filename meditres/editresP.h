/*
 * $Header: /cvsroot-fuse/meditres/meditres/editresP.h,v 1.4 2001/12/23 17:01:52 dannybackx Exp $
 *
 * $XConsortium: editresP.h,v 1.14 94/04/17 20:38:54 dave Exp $
 *
 * Copyright (c) 1989  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from the X Consortium.
 *
 * Author:  Chris D. Peterson, MIT X Consortium
 */

/*
 * Modifications by Volker Gering, 1997
 * and more © by Danny Backx, 2001.
 */

#ifndef	_EDITRES_P_H_
#define	_EDITRES_P_H_

#include <X11/Xmu/EditresP.h>
#include <X11/Xresource.h>

/* if DEBUG is TRUE, */
/* all nodes are shown, but they can't be selected */
/*   (the toggles can be pushed in, but it is not a valid selection) */
/* The View menu has a new entry: Folding Status, showing the viewing */
/*   status of the node. */
/* The timeout is 60sec versus 5sec */
#undef DEBUG

#define LOG   no_printf(
#define END   );

#ifdef DEBUG
#define CLIENT_TIME_OUT 60000	/* wait sixty seconds for the client. */
#else
#define CLIENT_TIME_OUT 5000	/* wait five seconds for the client. */
#endif /* DEBUG */

#define PROTOCOL_VERSION_ONE_POINT_ONE  5 /* version 1.1 */
#define ONE_POINT_ONE_STRING "1.1"
#define PROTOCOL_VERSION_ONE_POINT_ZERO 4 /* version 1.0 */
#define ONE_POINT_ZERO_STRING "1.0" ONE_POINT_ONE_STRING 

#define CURRENT_PROTOCOL_VERSION PROTOCOL_VERSION_ONE_POINT_ONE 
#define CURRENT_PROTOCOL_VERSION_STRING ONE_POINT_ONE_STRING

#define FLASH_TIME  1000	/* Default flash time in microseconds */
#define NUM_FLASHES 3		/* Default number of flashes. */

#define NO_IDENT    0		/* an ident that will match nothing. */

#define NUM_INC     10		/* amount to increment allocators. */

#define ANY_RADIO_DATA ("the any widget")
#define RESOURCE_BOX   ("resourceBox")

extern void exit();

/*
 * Retrieving ResType and Boolean is the same as retrieving a Card8.
 */

#define _XEditResGetBoolean _XEditResGet8
#define _XEditResGetResType _XEditResGet8

/*
 * Contexts to use with the X Context Manager.
 */

#define NODE_INFO ((XContext) 42)

/*
 * Error codes for X Server errors. 
 */

#define NO_ERROR  0
#define NO_WINDOW 1

typedef enum
{
  LocalSendWidgetTree,
  LocalSetValues,
  LocalFindChild, 
  LocalFlashWidget,
  LocalGetGeometry,
  LocalGetResources,
  LocalGetValues
} ResCommand;

typedef enum
{
  ClassLabel,
  NameLabel,
  IDLabel,
  WindowLabel,
  ToggleLabel,
  ViewLabel
} LabelTypes;

typedef enum
{
  SelectWidget,
  SelectAll,
  SelectNone,
  SelectInvert,
  SelectParent, 
  SelectChildren,
  SelectDescendants,
  SelectAncestors
} SelectTypes;

typedef enum
{
  NotShownBefore,
  FirstShown,
  Shown,
  LastShown,
  NotShownAfter,
  NotShownBeside
} ViewTypes;

typedef enum
{
  NewRoot,
  NewParent,
  NewSelected
} NewtopTypes;

typedef enum
{
  FoldSelected,
  FoldChildSelected,
  Unfold1Selected,
  UnfoldSelected,
  UnfoldAll
} FoldTypes;

typedef enum
{
  FindTypeAll,
  FindTypeNameOrClass,
  FindTypeName,
  FindTypeClass,
  FindTypeAnyID,
  FindTypeWinID,
  FindTypeWidID
} FindTypes;

typedef struct _NameInfo
{
  struct _NameInfo *next;	/* Next element in the linked list. */
  Widget           sep_leader;	/* The separator toggle group leader. */
  Widget           name_leader;	/* The name toggle group leader. */
} NameInfo;

typedef struct _ResourceBoxInfo
{
  Widget   value_wid;	/* The string containing the value. */
  Widget   res_label;	/* The label containing current resoruce. */
  Widget   shell;	/* Shell widget containing resource box. */
  Widget   norm_list;	/* The List widget for the normal list. */
  Widget   cons_list;	/* The List widget for the 
			   Constriaint Resources */
  NameInfo *name_info;	/* The info about the widgets for each
			   name and class in the instance heirarchy. */
} ResourceBoxInfo;
    
typedef struct _WidgetResourceInfo
{
  char *name;
  char *class;
  char *type;           /* Name, Class and Type of each resource. */
} WidgetResourceInfo;
    
typedef struct _WidgetResources
{
  int                num_normal;
  int                num_constraint;
  WidgetResourceInfo *normal;
  WidgetResourceInfo *constraint;
  ResourceBoxInfo    *res_box;
} WidgetResources;

typedef struct _NodeGeometry
{
  int                x;
  int                y;
  int                width;
  int                height;
  int                maxWidth;      /* max. width of all nodes at this level */
} NodeGeometry;

typedef struct _WNode
{
  char             *name;
  char             *class;
  unsigned long    id;
  unsigned long    window;
  char             *label;          /* the actual shown text of this node */
  struct _WNode    *parent;
  Cardinal         num_parents;     /* the depth of this node */
  struct _WNode    **children;
  Cardinal         num_children;
  Cardinal         alloc_children;
  struct _TreeInfo *tree_info;
  Widget           widget;
  NodeGeometry     item;            /* geometry for printing tree layout */
  Position         posx;
  Position         posy;
  Dimension        width;
  Dimension        height;
  ViewTypes        view;
  Boolean          childrenFolded;
  WidgetResources  *resources;
} WNode;

/*
 * Information for the Select any widget, toggle buttons in the
 * resource boxes.
 */

typedef struct _AnyInfo
{
  WNode  *node;		/* A Pointer off to the node corrsponding to
			   this resource box. */
  Widget left_dot;
  Widget left_star;	/* The dot and star widgets to our left. */
  Widget right_dot;
  Widget right_star;    /* The dot and star widgets to our right. */
  int    left_count;
  int    *right_count;  /* If count > 0 then desensitize the left or
			   right dot and star widgets. */
} AnyInfo;

/*
 * Information about the client we are currently working with.
 */

typedef struct _CurrentClient
{
  ResCommand     command;  /* the command sent. */
  ResIdent       ident;
  ProtocolStream stream;   /* protocol stream for this client. */
  XtIntervalId   timeout;  /* timeout set in case he doesn't answer. */
  Window         window;   /* window to communicate with. */
  Atom           atom;	   /* Atom used to communicate with this client.*/
} CurrentClient;

/*
 * Information about a tree we can display.
 */

typedef struct _TreeInfo
{
  Widget    tree_widget;       	 /* The Tree widget that contains all nodes */
  WNode     *top_node;		 /* The top node in the tree. */
  WNode     *newtop_node;        /* The first shown node */
  Cardinal  newtop_depth;        /* The depth of the first shown node */
  Cardinal  num_nodes;           /* number of all nodes */
  WNode     **active_nodes;	 /* The currently active nodes. */
  Cardinal  num_active;          /* number of active nodes, */
  Cardinal  alloc_active;        /* and space */
  Widget    *flash_widgets;	 /* list of widgets to flash on and off. */
  Cardinal  num_flash_widgets;   /* number of flash widgets */
  Cardinal  alloc_flash_widgets; /* and space */
  Cardinal  num_shown;           /* number of shown toggles/nodes */
  ViewTypes viewing;             /* actual labeling */
} TreeInfo;

/*
 * Information specific to a give APPLICATION screen.
 */

typedef struct _ScreenData
{
  Widget set_values_popup;	/* The SetValues popup. */
  Widget res_text;		/* SetValues resource text widget. */
  Widget val_text;		/* SetValues value text widget. */
  Widget find_popup;            /* The Find dialog */
  Widget find_text;             /* FindDlg searchFor text widget */
  Widget tree_dialog;           /* Graphical view of the tree to print */
  Widget info_label;	        /* The information label for messages */
} ScreenData;

typedef struct _AppResources
{
  Boolean debug;	 	/* Is debugging on? */
  int     num_flashes;          /* Number of flashes */
  int     flash_time;           /* Duration of flashes. */
  Pixel   flash_color;          /* Color of flash window. */
  char    *save_resources_file; /* File to save the resources into. */
  Boolean msgWidget;            /* write messages only to widget */
  Boolean msgStderr;            /* write messages only to stderr */
  Boolean msgBoth;              /* write messages to widget and stderr */
  /* >>>>> HACK for batchres */
  char    *winID;               /* Window ID of client */
  char    *resFile;             /* new resource file to load */
  Boolean terminate;            /* terminate after loading resFile */
  /* <<<<< HACK for batchres */

  /* Private state */
  Boolean allocated_save_resources_file;
} AppResources;

/*
 * Information needed to apply the resource string to all widgets.
 */

typedef struct _ApplyResourcesInfo
{
  char           *name;
  char           *class;  /* name and class  of this resource. */
  unsigned short count;
  ProtocolStream *stream;
  XrmDatabase    database;
} ApplyResourcesInfo;
    
/*
 * Information needed to get a resource string from a widget.
 */

typedef struct _ObtainResourcesInfo
{
  char           *name;
  char           *class; /* name and class  of this resource. */
  unsigned short count;
  ProtocolStream *stream;
  XrmDatabase    database;
} ObtainResourcesInfo;

/************************************************************
 *
 * The Event Structures.
 *
 ************************************************************/

typedef struct _AnyEvent
{
  EditresCommand type;
} AnyEvent;

typedef struct _WidgetTreeInfo
{
  WidgetInfo    widgets;
  char          *name;
  char          *class;
  unsigned long window;
} WidgetTreeInfo;

typedef struct _SendWidgetTreeEvent
{
  EditresCommand type;
  char           *toolkit;
  unsigned short num_entries;
  WidgetTreeInfo *info;
} SendWidgetTreeEvent;

typedef struct _SetValuesInfo
{
  WidgetInfo widgets;
  char       *message;
} SetValuesInfo;
    
typedef struct _SetValuesEvent
{
  EditresCommand type;
  unsigned short num_entries;
  SetValuesInfo  *info;
} SetValuesEvent;

typedef struct _GetValuesInfo
{
  WidgetInfo widgets;
  char       *value;
} GetValuesInfo;
    
typedef struct _GetValuesEvent
{
  EditresCommand type;
  unsigned short num_entries;
  GetValuesInfo  *info;
} GetValuesEvent;

typedef struct _ResourceInfo
{
  ResourceType res_type;
  char         *name;
  char         *class;
  char         *type;
} ResourceInfo;

typedef struct _GetResourcesInfo
{
  WidgetInfo     widgets;
  Boolean        error;
  char           *message;
  unsigned short num_resources;
  ResourceInfo   *res_info;
} GetResourcesInfo;

typedef struct _GetResourcesEvent
{
  EditresCommand   type;
  unsigned short   num_entries;
  GetResourcesInfo *info;
} GetResourcesEvent;

typedef struct _GetGeomInfo
{
  EditresCommand type;
  WidgetInfo     widgets;
  Boolean        error;
  char           *message;
  Boolean        visable;
  short          x;
  short          y;
  unsigned short width;
  unsigned short height;
  unsigned short border_width;
} GetGeomInfo;

typedef struct _GetGeomEvent
{
  EditresCommand type;
  unsigned short num_entries;
  GetGeomInfo    *info;
} GetGeomEvent;

typedef struct _FindChildEvent
{
  EditresCommand type;
  WidgetInfo     widgets;
} FindChildEvent;

typedef union _Event
{
  AnyEvent            any_event;
  SendWidgetTreeEvent send_widget_tree_event;
  SetValuesEvent      set_values_event;
  GetResourcesEvent   get_resources_event;
  GetGeomEvent        get_geom_event;
  FindChildEvent      find_child_event;
  GetValuesEvent      get_values_event;
} Event;
    
/*
 * number of application resource labels (from 0 to NUM_RES_LABELS - 1).
 */

#define NUM_RES_LABELS 48

/*
 * Global variables. 
 */

#ifndef THIS_IS_MAIN
extern int           global_effective_protocol_version;
extern char          *global_effective_toolkit;
extern int           global_error_code;
extern unsigned long global_serial_num;
extern int           (*global_old_error_handler)();
extern Boolean       global_resource_box_up;

extern TreeInfo      *global_tree_info;
extern CurrentClient global_client;
extern ScreenData    global_screen_data;
extern Widget        global_tree_parent;
extern Widget        global_paned;	/* named after toolkit */
extern Widget        global_toplevel;
extern AppResources  global_resources;
extern FindTypes     global_findType;
extern GC            global_draw_gc;
extern GC            global_fill_gc;
extern XFontStruct   *global_draw_font;
extern int           global_print_width;
/* >>>>> HACK for batchres */
extern Window        global_winID;
extern int           global_mode;
extern char          *global_resfile_buffer;
extern Cardinal      global_resfile_wraplines;
extern Cardinal      global_resfile_lines;
/* <<<<< HACK for batchres */

/* Widgets in the status area */
extern Widget        global_numWid;
extern Widget        global_shownWid;   /* number of shown toggles/nodes */
extern Widget        global_selWid;     /* number of selected/active widgets */
extern Widget        global_wait;       /* countdown */
extern Widget        global_zoom;
extern Widget        global_work;       /* work in progress */
extern Widget        global_rcv;        /* receiving data */

extern int           global_countdown;  /* actual countdown value */
extern XtIntervalId  global_timeout;    /* id for countdown timer */

extern Cardinal      global_maxdepth;   /* max. levels to show */
extern Cardinal      global_maxchilds;  /* max. childs of node to show */
extern Cardinal      global_movespeed;  /* multiplication for Btn2Motion in tree */

extern String        res_labels[NUM_RES_LABELS];
#endif

/*
 * Macros.
 */

/* default number of levels and children to show */
#define MAXDEPTH           10
#define MAXCHILDS          30

/* default speed (multiplication) for moving tree "by hand" */
#define MOVESPEED          2

/* number of lines to keep in message list */
#define MAX_LIST_LEN       70

/* editres can load a full resource file into the client: MODE_BATCHRES */
#define MODE_INTERACTIVE   0      /* default */
#define MODE_BATCHRES      1

/* number of entries in the command menu */
/* #define NUM_CM_ENTRIES     8 */

/* offset into CM entries for setting insensitive */
/* #define CM_OFFSET          1 */

/* number of CM entries to make insensitive */
/* #define CM_NUM             5 */

/* number of entries in the tree menu */
/* #define NUM_TM_ENTRIES     16 */
/* #define TM_OFFSET          0 */
/* #define TM_NUM             16 */


/*
 * Function declarations
 */
void GetResource(Widget, XtPointer, XtPointer);

#endif
