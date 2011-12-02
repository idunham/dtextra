/*
 * $Header: /cvsroot-fuse/meditres/meditres/print.c,v 1.6 2002/01/12 00:43:19 amai Exp $
 *
 * This is part of the program "editres2",
 * based on "editres" from Chris D. Peterson, MIT X Consortium
 *
 * Volker Gering, 1997
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>

#include "meditres.h"


static int  _nodeHeight(WNode *node, int x_start, int y_start);
static int  _nodeWidth(WNode *node);
static void showTree(Widget parent, WNode *top);
static void printCancel(Widget w, XtPointer junk, XtPointer garbage);
static void makePS(Widget w, XtPointer junk, XtPointer garbage);
static void _psfuncTextBox(void);
static void _psfuncDupXY(void);
static void _printNode(WNode *node);


/* global variables */
static FILE *outstream = NULL;


/* Function Name:
 *   printTree
 *
 * Description:
 *   Prints the layout of the widget tree graphically.
 *
 * Arguments:
 *   top - the top node.
 *
 * Returns:
 *   (None)
 *
 * Calls:
 *   _nodeHeight - (self)
 */
void
printTree(Widget w, XtPointer junk, XtPointer garbage)
{
  WNode *top;
  char msg[BUFSIZ];
  char fontName[BUFSIZ];


  if (global_tree_info == NULL)
    {
      /* "No widget/style tree is available" */
      SetMessage(global_screen_data.info_label,
		 res_labels[17],
		 "No widget/style tree is available");
      return;
    }
  else
    {
      strcpy(fontName,
	     "-*-helvetica-*-r-*-*-0-0-*-*-*-*-*-*");
      if ((global_draw_font = XLoadQueryFont(XtDisplay(global_toplevel),
					     fontName)) == NULL)
	{
	  /* Could not load font '%s' */
	  if (res_labels[47] != NULL)
	    {
	      sprintf(msg,
		      res_labels[47],
		      fontName);
	    }
	  else
	    {
	      sprintf(msg,
		      "Could not load font '%s'",
		      fontName);
	    }
	  SetMessage(global_screen_data.info_label,
		     msg,
		     "?res_labels[47]");
	  return;
	}

      else
	{
	  top = global_tree_info->newtop_node;
	  top->item.width = 0;
	  top->item.maxWidth = _nodeWidth(top);
	  global_print_width = 0;
	  (void)_nodeHeight(top, 0, 0);
	}   /* else (font loaded) */
    }   /* else (widget/style tree available) */

  /* show the tree to print on screen (preview) */
  showTree (global_toplevel, top);

}   /* printTree() */


/* Function Name:
 *   _nodeHeight
 *
 * Description:
 *   Calculates the height and the position of the given node.
 *   The height is the summary of the height of all children.
 *
 * Arguments:
 *   node - the node we are looking at
 *
 * Returns:
 *   the height
 *
 * Calls:
 *   _nodeHeight - (recursive)
 *   _nodeWidth - (self)
 *   notShown - wtree.c
 */
#define HOR_DISTANCE 25
#define VERT_DISTANCE 4
/* #define OTHER_TREELAYOUT */

static int
_nodeHeight(WNode *node, int x_start, int y_start)
{
  int  height;      /* height of all children or space for this node */
  int  ownHeight;   /* height of this node */
  int  width;       /* width of this node */
  int  maxWidth;    /* max. width of children */
  int  i;


  width = node->item.width ? node->item.width : _nodeWidth(node);
  ownHeight = global_draw_font->max_bounds.ascent
              + global_draw_font->max_bounds.descent;

  if (node->num_children == 0)
    {
      height = ownHeight;

      /* keeping the max. width of the whole tree */
      if (global_print_width < x_start + node->item.maxWidth)
	{
	  global_print_width = x_start + node->item.maxWidth;
	}
    }   /* if (a leaf of the tree) */

  else if (notShown(node))
    {
      height = 0;
    }

  else
    {
      maxWidth = 0;
      for (i=0; i < node->num_children; i++)
	{
	  if (!notShown(node->children[i])
	      && _nodeWidth(node->children[i]) > maxWidth)
	    {
	      maxWidth = _nodeWidth(node->children[i]);
	    }
	}

      height = 0;
      for (i=0; i < node->num_children; i++)
	{
	  if (!notShown(node->children[i]))
	    {
	      node->children[i]->item.maxWidth = maxWidth;
	      height = height
		+ _nodeHeight(node->children[i],
			      x_start + node->item.maxWidth
			      + HOR_DISTANCE
#ifdef OTHER_TREELAYOUT
 * node->num_children
#endif   /* OTHER_TREELAYOUT */
			      ,
			      y_start + height + VERT_DISTANCE / 2)
		       + VERT_DISTANCE;
	    }
	}   /* for (all children) */

      /* perhaps this node is higher than its children */
      if (height < ownHeight)
	{
	  height = ownHeight;
	}
    }

  /* keep the calculated values for this node */
  node->item.x = x_start;
  node->item.y = y_start + (height / 2 - ownHeight / 2);
  node->item.height = ownHeight;
  node->item.width = width;
  /* node->bbox.x = x_start; */
  /* node->bbox.y = y_start; */
  /* node->bbox.height = height; */
  /* node->width = width; */

  /* fprintf(stderr,
	  "name: %15s,  x %3d, y %3d, w %3d, h %3d; BBox: w %d, h %d\n",
	  node->label, x_start, node->item.y,
	  node->item.width, node->item.height,
	  node->item.maxWidth, height); */

  return (height);

}   /* _nodeHeight() */


/* Function Name:
 *   _nodeWidth
 *
 * Description:
 *   Calculates the width of the given node. It is given by the font,
 *   the text length and the possible add on´s like "...", ">" and "+".
 *
 * Arguments:
 *   node - the node we are looking at
 *
 * Returns:
 *   the width
 *
 * Calls:
 */
static int
_nodeWidth(WNode *node)
{
  int width;


  if (node->label != NULL)
    {
      width = XTextWidth(global_draw_font,
			 node->label,
			 strlen(node->label));
    }
  else
    {
      width = XTextWidth(global_draw_font,
			 "(NULL)?",
			 strlen("(NULL)?"));
    }

  return (width);

}   /* _nodeWidth() */


/* Function Name:
 *   showTree
 *
 * Description:
 *   Shows the actual widget tree graphically
 *
 * Arguments:
 *   parent - parent widget for the dialog
 *   top - the first node of the tree
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   _drawNode - (self)
 *
 * Callbacks installed:
 *   (XtEventhandler) printExpose - handler.c
 */
static void
showTree(Widget parent, WNode *top)
{
#if 0
  Widget            pdialog;
  Widget            dchild;
  Widget            vport;
  Widget            vclip;
  static Widget     plabel;
  Arg               args[6];
  int               num_args;
  int               i;
  int               twidth = global_print_width + 2;
  int               theight = top->item.y * 2 + top->item.height;


  if (global_screen_data.tree_dialog == NULL)
    {
      global_screen_data.tree_dialog
	= XtCreatePopupShell("printTreeSh", 
			     transientShellWidgetClass, 
			     parent,
			     NULL, ZERO);
      num_args = 0;
      XtSetArg(args[num_args],
	       XtNlabel, "");
      num_args++;
      XtSetArg(args[num_args],
	       XtNvalue, "");
      num_args++;
      pdialog = XtCreateManagedWidget("pdialog",
				      dialogWidgetClass,
				      global_screen_data.tree_dialog,
				      args, num_args);

      num_args = 0;
      XtSetArg(args[num_args],
	       XtNwidth, twidth < 700 ? twidth : 700);
      num_args++;
      XtSetArg(args[num_args],
	       XtNheight, theight < 500 ? theight : 500);
      num_args++;
      XtSetArg(args[num_args],
	       XtNallowHoriz, TRUE);
      num_args++;
      XtSetArg(args[num_args],
	       XtNallowVert, TRUE);
      num_args++;
      XtSetArg(args[num_args],
	       XtNresizable, FALSE);
      num_args++;
      vport = XtCreateManagedWidget("vport",
				    viewportWidgetClass,
				    pdialog,
				    args, num_args);

      num_args = 0;
      XtSetArg(args[num_args],
	       XtNmappedWhenManaged, TRUE);
      num_args++;
      XtSetArg(args[num_args],
	       XtNlabel, "");
      num_args++;
      plabel = XtCreateManagedWidget("plabel",
				     labelWidgetClass,
				     vport,
				     args, num_args);

      XtAddEventHandler(plabel,
			ExposureMask,
			FALSE,
			printExpose,
			NULL);

      global_draw_gc = NULL;

      num_args = 0;
      XtSetArg(args[num_args],
	       XtNfromVert, vport);
      num_args++;
      XtSetArg(args[num_args],
	       XtNresizable, TRUE);
      num_args++;
      XtSetArg(args[num_args],
	       XtNlabel, "Filename: (\"-\" for stdout)");
      num_args++;
      dchild = XtNameToWidget(pdialog, "label");
      XtSetValues (dchild,
		   args, num_args);

      XawDialogAddButton(pdialog,
			 "printOK",
			 makePS,
			 NULL);
      XawDialogAddButton(pdialog,
			 "printCancel",
			 printCancel,
			 NULL);

      /* do some "corrections" to the form constraints */
      num_args = 0;
      XtSetArg(args[num_args],
	       XtNbottom, XtChainBottom);
      num_args++;
      XtSetValues(vport,
		  args, num_args);

      XtSetArg(args[num_args],
	       XtNtop, XtChainBottom);
      num_args++;
      XtSetValues (dchild,
		   args, num_args);

      dchild = XtNameToWidget(pdialog, "value");
      XtSetValues (dchild,
		   args, num_args);

      dchild = XtNameToWidget(pdialog, "printOK");
      XtSetValues (dchild,
		   args, num_args);

      dchild = XtNameToWidget(pdialog, "printCancel");
      XtSetValues (dchild,
		   args, num_args);

    }   /* if (print dialog not already created) */

  XtPopup(global_screen_data.tree_dialog, XtGrabNone);

  num_args = 0;
  XtSetArg(args[num_args], XtNwidth, global_print_width + 2);
  num_args++;
  XtSetArg(args[num_args], XtNheight, top->item.y * 2 + top->item.height);
  num_args++;
  XtSetValues (plabel, args, num_args);
#endif
}   /* showTree() */


/* Function Name:
 *   printCancel
 *
 * Description:
 *   Closes the print preview.
 *
 * Arguments:
 *
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   Xt stuff
 */
static void
printCancel(Widget w, XtPointer junk, XtPointer garbage)
{
  XtPopdown(global_screen_data.tree_dialog);
}   /* printCancel() */


/* Function Name:
 *   makePS
 *
 * Description:
 *
 * Arguments:
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   _printNode - (self)
 *
 * Globals:
 *   FILE outstream
 */
static void
makePS(Widget w, XtPointer junk, XtPointer garbage)
{
  Widget    pdialog;
  char      tbuf[BUFSIZ];
  time_t    t;
  struct tm *ltime;
  int       bboxWidth;
  int       bboxHeight;
  double    scaleX = 1.0;
  double    scaleY = 1.0;
  char      *outfileName = "";
  Boolean   eps = FALSE;	/* TRUE: generate an encapsulated postscript file */


  /* temporarily for A4 papersize */
#define paperWidth 576
#define paperHeight 792
#define pageMarginWidth 10
#define pageMarginHeight 10
#define boxMarginWidth 2
#define boxMarginHeight 2
#define maxBboxWidth (paperWidth - pageMarginWidth * 2)
#define maxBboxHeight (paperHeight - pageMarginHeight * 2)

  /* fprintf(stderr, "connecting to Postscript-Server not yet implemented\n"); */

  /* open the output file */
  pdialog = XtNameToWidget(global_screen_data.tree_dialog, "pdialog");
  outfileName = 0;
/*  outfileName = XawDialogGetValueString(pdialog); */
  if (outfileName == NULL
      || strlen(outfileName) == 0
      || strcmp(outfileName, "-") == 0)
    {
      outstream = stdout;
    }
  else
    {
      if ((outstream = fopen(outfileName, "w")) == NULL)
	{
	  perror("Error opening output file for printing");
	  return;
	}
    }   /* else (not stdout) */

  /* Postscript prolog */
  if (eps)
    {
      fprintf(outstream, "%%!PS-Adobe-2.0 EPSF-1.2\n");
    }
  else
    {
      fprintf(outstream, "%%!PS-Adobe-2.0\n");
    }
  fprintf(outstream, "%%%%Title: (Widget tree from root widget '%s')\n",
	 global_tree_info->newtop_node->name);
  fprintf(outstream, "%%%%Creator: editres2\n");
  /* fprintf(outstream, "%%%%For: %s\n", this_operating_system); */

  tbuf[0] = '\0';
  t = time(NULL);
  ltime = localtime(&t);
  if (strftime(tbuf, BUFSIZ, "%d %b %Y, %H:%M", ltime) >= BUFSIZ)
    {
      fprintf(stderr, "internal ERROR: Variable too small!\n");
    }
  fprintf(outstream, "%%%%CreationDate: %s\n", tbuf);

  bboxWidth = global_print_width + 1;
  bboxHeight = global_tree_info->newtop_node->item.y * 2
    + global_tree_info->newtop_node->item.height + 1
    + 12 + 4;					/* Pageheader */
  if (bboxWidth > maxBboxWidth)
    {
      scaleX = (double)(maxBboxWidth) / (double)(bboxWidth);
    }
  if (bboxHeight > maxBboxHeight)
    {
      scaleY = (double)(maxBboxHeight) / (double)(bboxHeight);
    }
  /* symetrical scaling in x- and y-direction */
  if (scaleX < scaleY)
    {
      scaleY = scaleX;
    }
  else
    {
      scaleX = scaleY;
    }
  fprintf(outstream, "%%%%BoundingBox: %d %d %d %d\n",
	 0, 0,
	 (int)(bboxWidth * scaleX), (int)(bboxHeight * scaleY));
  fprintf(outstream, "%%%%EndComments\n\n");
  fprintf(outstream, "%%%%EndProlog\n\n");
  fprintf(outstream, "%%%%Page: 1 1\n\n");

  fprintf(outstream, "/Helvetica findfont\n");
  fprintf(outstream, "12 scalefont\n");
  fprintf(outstream, "setfont\n\n");

  /* Margins */
  fprintf(outstream, "/pageMarginHeight %d def\n", pageMarginHeight);
  fprintf(outstream, "/pageMarginWidth %d def\n", pageMarginWidth);
  fprintf(outstream, "/boxMarginHeight %d def\n", boxMarginHeight);
  fprintf(outstream, "/boxMarginWidth %d def\n\n", boxMarginWidth);

  /* print header if not encapsulated postscript */
  if (!eps)
    {
      fprintf(outstream, "pageMarginWidth %d moveto\n",
	     (int)(bboxHeight * scaleY));
      fprintf(outstream, "(editres2: Widget tree from root widget %s, %s) show\n\n",
	     global_tree_info->newtop_node->name, tbuf);
    }

  /* X11 and Postscript have different origin (0,0) */
  fprintf(outstream, "/mirrorY\n");
  fprintf(outstream, "{  %d exch sub\n", bboxHeight + 1);
  if (!eps)
    {
      fprintf(outstream, "  pageMarginHeight sub\n");
    }
  fprintf(outstream, "} def\n\n");

  /* A4: 11"(792p) x 8"(576p), Letter: 10"(720p) x 8"(576p) */
  fprintf(outstream, "pageMarginWidth -1 translate\n");
  fprintf(outstream, "%.4f %.4f scale\n\n", scaleX, scaleY);

  _psfuncTextBox();
  _psfuncDupXY();

  _printNode(global_tree_info->newtop_node);

  fprintf(outstream, "showpage\n");

  if (outstream != stdout)
    {
      fclose(outstream);
    }
  XtPopdown(global_screen_data.tree_dialog);
}   /* makePS() */


/* Function Name:
 *   _psfuncTextBox
 *
 * Description:
 *   Prints the postscript function to draw a box around a text.
 *
 * Arguments:
 *   <void>
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   <none>
 *
 * Globals:
 *   FILE outstream
 */
static void
_psfuncTextBox(void)
{
  fprintf(outstream, "\n%% Parameter on stack: (Text) x y\n");
  fprintf(outstream, "%% Globals:\n");
  fprintf(outstream, "%%   in: boxMarginWidth, boxMarginHeight\n");
  fprintf(outstream, "%%   out/modified: x, y, xend, yend\n");
  fprintf(outstream, "/textBox\n");
  fprintf(outstream, "{ newpath\n");
  fprintf(outstream, "  moveto\n");
  fprintf(outstream, "  true charpath\n");
  fprintf(outstream, "  flattenpath pathbbox\n");
  fprintf(outstream, "  newpath\n\n");

  fprintf(outstream, "  boxMarginHeight 2 mul add\n");
  fprintf(outstream, "  /yend exch def\n");
  fprintf(outstream, "  boxMarginWidth 2 mul add\n");
  fprintf(outstream, "  /xend exch def\n");
  fprintf(outstream, "  boxMarginHeight sub\n");
  fprintf(outstream, "  /y exch def\n");
  fprintf(outstream, "  boxMarginWidth sub\n");
  fprintf(outstream, "  /x exch def\n\n");

  fprintf(outstream, "  x y moveto\n");
  fprintf(outstream, "  xend y lineto\n");
  fprintf(outstream, "  xend yend lineto\n");
  fprintf(outstream, "  x yend lineto\n");
  fprintf(outstream, "  x y lineto\n");
  fprintf(outstream, "  closepath\n");
  fprintf(outstream, "} def\n\n");

}   /* psfuncTextBox() */


/* Function Name:
 *   _psfuncTextBox
 *
 * Description:
 *   Prints the postscript function to draw a box around a text.
 *
 * Arguments:
 *   <void>
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   <none>
 *
 * Globals:
 *   FILE outstream
 */
static void
_psfuncDupXY(void)
{
  fprintf(outstream, "%% duplicate x, y on the stack\n");
  fprintf(outstream, "/dupxy\n");
  fprintf(outstream, "{ exch\n");
  fprintf(outstream, "  dup\n");
  fprintf(outstream, "  3 2 roll\n");
  fprintf(outstream, "  dup\n");
  fprintf(outstream, "  4 3 roll\n");
  fprintf(outstream, "  exch\n");
  fprintf(outstream, "} def\n\n");

}   /* _psfuncDupXY() */


/* Function Name:
 *   _printNode
 *
 * Description:
 *   Prints the given node as postscript.
 *
 * Arguments:
 *   node - the node we are looking at
 *
 * Returns:
 *   <void>
 *
 * Calls:
 *   _printNode - (recursive)
 *
 * Globals:
 *   FILE outstream
 */
static void
_printNode(WNode *node)
{
  int    i;


  fprintf(outstream, "%% name: %s\n", node->name);
  fprintf(outstream, "%% class: %s\n", node->class);
  fprintf(outstream, "%% window: %ld (0x%x)\n", node->window, node->window);
  fprintf(outstream, "%% widget: %ld (0x%x)\n", node->id, node->id);
  fprintf(outstream, "%% depth: %d\n", node->num_parents);
  fprintf(outstream, "%% num children: %d\n", node->num_children);

  /* 1st: the box */
  /* XDrawRectangle (XtDisplay(wid),
   *		  win,
   *		  global_draw_gc,
   *              node->item.x,
   *              node->item.y,
   *              node->item.width,
   *              node->item.height);
   */
  fprintf(outstream, "(%s) %d %d mirrorY textBox\n",
	 node->label,
	 node->item.x,
	 node->item.y + node->item.height);
  /* fprintf(outstream, "2 setlinewidth\n"); */
  if (IsActiveNode(node))
    {
      fprintf(outstream, ".8 setgray\n");
      fprintf(outstream, "fill\n");
      fprintf(outstream, "0 setgray\n");
    }
  else
    {
      fprintf(outstream, "stroke\n");
    }

  /* 2nd: the label */
  fprintf(outstream, "%d %d mirrorY moveto\n",
	 node->item.x,
	 node->item.y + node->item.height);
  if (node->label != NULL)
    {
      /* XDrawString (XtDisplay(wid),
       *       	   win,
       *	   global_draw_gc,
       *	   node->item.x,
       *	   node->item.y + node->item.height
       *	   - global_draw_font->max_bounds.descent,
       *	   node->label,
       *	   strlen(node->label));
       */
      fprintf(outstream, "(%s) show\n", node->label);
    }
  else
    {
      /* XDrawString (XtDisplay(wid),
       *	   win,
       *	   global_draw_gc,
       *	   node->item.x,
       *	   node->item.y + node->item.height
       *	   - global_draw_font->max_bounds.descent,
       *	   "(NULL)?",
       *	   strlen("(NULL)?"));
       */
      fprintf(outstream, "((NULL)?) show\n");
    }

  /* reference point for line to children */
  fprintf(outstream, "xend %d mirrorY\n",
	 node->item.y + (node->item.height / 2) + boxMarginHeight);

  for (i=0; i < node->num_children; i++)
    {
      if (!notShown(node->children[i]))
	{
	  /* XDrawLine(XtDisplay(wid),
	   *	    win,
	   *	    global_draw_gc,
	   *	    node->item.x + node->item.width,
	   *	    node->item.y + (node->item.height / 2),
	   *	    node->children[i]->item.x,
	   *	    node->children[i]->item.y
	   *	    + (node->children[i]->item.height / 2));
	   */
	  fprintf(outstream, "newpath\n");
	  fprintf(outstream, "dupxy moveto\n");
	  fprintf(outstream, "%d %d mirrorY lineto\n",
		 node->children[i]->item.x - boxMarginWidth,
		 node->children[i]->item.y
		 + (node->children[i]->item.height / 2) + boxMarginHeight);
	  fprintf(outstream, "stroke\n");

	  _printNode(node->children[i]);
	}
    }   /* for (all children) */

  fprintf(outstream, "pop pop\n\n");

}   /* _printNode() */
