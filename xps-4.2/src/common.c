/* 
 $Id: common.c,v 1.4 2003/04/24 00:21:29 rockyb Exp $
 This code is the same for any platform... 
 This code however does have OS-specific code as set up by os.h and
 #ifdef's. It also has X-specific calls.
 To do: better divison between this and xstuff.c
        separate tree layout stuff into another file.

 Copyright (C) 1998-2001 Rocky Bernstein

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "user_config.h"
#include "os.h"

#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/param.h>
#include <string.h> /* strchr */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>

#include "xstuff.h"
#include "xps.h"
#include "xpsplay.h"
#include "common.h"
#include "loadavg.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

/* name of the local system. */
char nodename[MAXHOSTNAMELEN]; 

/* Parameters concerning how this program was built. */
char config_info[1024];


#ifdef RUN_SUID_ROOT
uid_t real_uid;        /* who originally was I */
#endif

/* Minimum and maximum allowed distance between levels and siblings in the
   displayed tree */
#define MIN_XSPACE 10
#define MAX_XSPACE 30  /* Make artificially too small for testing. */
#define MIN_YSPACE 10

#define MIN_SHRINK 40  /* width has to decrease more than this before
			  we start considering decreasing overal dimensions */

/* Note we don't impose a maximum y spacing value because we want the 
   nodes to be centered within the level. However this can lead to 
   arbitrarily large spacing. 
*/

#define ACTIVE_PROCS (EndNode-Node)    /* number of processes in tree */
#define ROOT_LEVEL (0)
#define untraversed(i) (Level[i] < 0)

typedef unsigned long int treelevel;

/* Maximum number of notes amongst any level. This is used to compute
   how big the canvas should be. 
*/
static long         maxDepth;  
static treelevel    TreeDepth;         /* depth of tree in nodes */

/* widest label at each level (in chars) */
static unsigned int MaxLabel[XPS_MAX_PROCS];

/* breadth of each level (nodes) */
static unsigned int Breadth[XPS_MAX_PROCS]; 
static unsigned int Virtual_Breadth[XPS_MAX_PROCS];

/* The vertical tree depth of node within a tree. The leaves have level 
   or depth 0. -1 means that the corresponding node entry has not been 
   assigned yet.
*/
static int Level[XPS_MAX_PROCS];

/* Pointers to nodes for the head of each level */
TREENODE   *LevelHead[XPS_MAX_PROCS];

/* 
  Keep track to see if the maximum out-degree of the parent level is 1.
  If so, we'll draw straight lines from the parent level to this one.

  The root level, 0, is a special case -- we want it to alway be arranged.

  I believe the other heuristics will catch this case so we could
  in theory remove this. However handling this special case is easy,
  and fast.
*/
static Boolean IsDegree1[XPS_MAX_PROCS];

static 
void printTree(Boolean show_parent)
{
  TREENODE *src;
  int   i;
  printf("------\n");
  for (i = 0, src = Node; i < ACTIVE_PROCS; i++, src++) {
    printf("node: 0x%08x, name: %s, user %ld, level %d, pid %ld, ppid: %ld",
	   (unsigned int) src, src->label, (long) src->uid, Level[i], 
	   (long int) src->pid, (long int) src->ppid);
    if (show_parent)
      printf("parent: 0x%08x", (unsigned int) src->parent);
    printf("\n");
  }
}

/* 
   Traverse tree gathering information which will be used to sort and 
   print the tree.  In particular:
   Assign tree-depths to nodes, parent pointers, 
   calculate the number of entries in each tree depth,
   and the maximum number of characters in a label at each depth.
   Also put the nodes into depth chains. (A tree depth is called a level.)
 */
static void 
traverse(nodeIndex cur)
{
  int i;
  treelevel level = ROOT_LEVEL;

  /* 
    Search for the parent process of the current one (cur).
  */

  for (i = 0; i < ACTIVE_PROCS; i++) {
     if (Node[i].pid == Node[cur].ppid)
       {
         if (cur == i) break;
	 if (untraversed(i)) traverse(i);
	 /* The parent level number should now have been assigned */
	 level=Level[i]+1;
	 Node[i].depth = level;
	 
	 /* If the parent already has a child then we don't have 
	    a level whose parent has maximum out degree 1. 
	 */
	 if (Node[i].children++) {
	   IsDegree1[level] = False;
	 }
	 /* found the parent. There is or (should be) only one. */
	 break; 
       }
   }
  /* 
    Either we found the parent process of this one in the above loop,
    or there is none (in which case level was initialized to 0).

    Update current process's level number, update 
    the breadth at the current level and the largest label name size at 
    this level. Also add this node to the chain at this level.
  */
  Level[cur] = level;
  ++Breadth[level];
  maxDepth  = MAX(maxDepth, Breadth[level]);
  TreeDepth = MAX(TreeDepth, level);
  { 
    int len = XTextWidth(active.font, Node[cur].label, strlen(Node[cur].label));
    MaxLabel[level] = MAX(MaxLabel[level], len); 
  }

}

/* Convert a Node pointer to a tree index */
#define toIndex(a) (a-Node)

/* Swap routine used by sort below. */
static void 
swap(TREENODE *a, TREENODE *b)
 {
   if (a != b) {
     int i = toIndex(a);
     int j = toIndex(b);
     int k;
     TREENODE temp[1];
     *temp = *a; 
     *a = *b;
     *b = *temp;
     k = Level[i]; 
     Level[i] = Level[j];
     Level[j] = k;
   }
 }     

/* Swap routine used by sort below, macro version. */
/*
#define swap(a, b) \
if (a != b) {     \
  NODE temp[1];   \
  *temp = *a;     \
  *a = *b;        \
  *b = *temp;     \
}     
*/

/* 
   Sort tree level by level, grouping nodes which have the same parent. 
   Sort by parent process id; within that userid; within that process id.
*/
static void 
sort(void)
{
  TREENODE *dst;
  TREENODE *beglevel;  /* Points to the first item of this level. */
  TREENODE *endlevel;  /* Points to the beginning of the *next* level. */
  treelevel level;
  int   i;
  TREENODE *parent;
  TREENODE *src;
  TREENODE *temp1;
  
  if (base.debugging & DBG_TREESORT) printTree(False); 

  /* root */
  src=dst = Node;
  beglevel = dst;
  endlevel = dst;

  LevelHead[ROOT_LEVEL]  = Node;
  LevelHead[TreeDepth+1] = EndNode; 
  
    
  /* Copy all nodes which are roots to the beginning of the Node structure. */
  for (i = 0; i < ACTIVE_PROCS; i++, src++) {
    if (Level[i] == ROOT_LEVEL) {
      swap(src, dst);
      ++dst;
    }
  }
  endlevel=dst;
  
  /* Bubble sort the root level by uid and within that pid. 
     This and the second bubble sort below should probably be
     put into a subroutine.

     Although bubble sort is a O(n**2) sort in the worst case,
     it can be O(n) for a pretty-much sorted array, and here it may be
     even beat an O(n log n) sort. 

     Processes are pretty much sorted by process id and this is
     roughly how we want them to be arranged. Furthermore, sorting is
     by number of children having a common parent, except for the root
     level where there is no parent and the whole level is sorted. The
     number of items with a common parent generally isn't a large
     number and a bubble sort has a little less overhead than
     quicksort. (That is, each iteration of a "loop" of bubble sort is
     faster than the corresponding "loop" of quick sort.

     We add the hack to test for complete sortedness.

     Still someone might want to benchmark against Quicksort.  */
  for (temp1 = beglevel; temp1 < endlevel; temp1++) {
    Boolean swapped = False;
    TREENODE *temp2, *temp3;
    
    temp3=endlevel; temp2 = --temp3;
    for (temp2--; temp2 >= temp1; temp2--, temp3--) {
      if (temp2->uid > temp3->uid || 
	  ((temp2->uid==temp3->uid) && (temp2->pid>temp3->pid)))
	{
	  swap(temp2, temp3);
	  swapped = True;
	} /*if*/
    } /*inner bubble-sort for*/
    if (!swapped) { 
      if (base.debugging & DBG_TREESORT) 
	printf("sorted early (1), %d skipped.\n", endlevel - temp1);
      break; 
    }
  } /*outer bubble-sort for*/
  
  /* 
     For all the non-root remaining levels,
     go through each node of the level and find the children for that
     level. (The parents are already in uid/pid sorted order.)
     For each set of children we bubble sort that to maintain 
     uid/pid order. 

     The sorting to arrange things into levels by parent
     is O(n*p*c**2) for n nodes, p non-leaf nodes, and c is the maximum
     number of children a parent has. This is a rough analysis
     and looks bad, but in practice is probably pretty good.
     I suppose radix sort could/should
     be done on the levels. Then we'd have O(n+p*c**2).
    */
  for (level = ROOT_LEVEL+1; level <= TreeDepth; level++) { 
    LevelHead[level-1] = beglevel;
    
   for (parent = beglevel; parent != endlevel; parent++) { 
     TREENODE *begdst, *enddst;
     /* copy all nodes from current level which have the same parent */
     begdst = dst;
     for (src = endlevel; src < EndNode; ++src) {
       if (Level[toIndex(src)] == level && src->ppid == parent->pid) {
	 swap(src, dst);
	 dst->parent = parent;
	 ++dst;
       }
     }
     
     /* Now group all processes having a common parent by user, and
	within that by process id. Note that the parent pointer does
	not have to be recalculated because we compute parent links
	AFTER the parents have been rearranged.
	
	A bubble sort is used to sort this non-root level.
	See the comments above concerning bubble sort vs. quicksort.  */
     enddst = dst;
     for (temp1 = begdst; temp1 < enddst; temp1++) {
       TREENODE *temp2, *temp3;
       Boolean swapped = False;
       
       temp2=temp3=enddst; temp2 = --temp3;
       for (temp2--; temp2 >= temp1; temp2--, temp3--) {
	 if (temp2->uid > temp3->uid || 
	     ((temp2->uid==temp3->uid) && (temp2->pid>temp3->pid)))
	   {
	     swap(temp2, temp3);
	     swapped = True;
	   } /*if*/
       } /*inner bubble-sort for*/
       if (!swapped) { 
	 if (base.debugging & DBG_TREESORT) 
	   printf("sorted early (2), %d skipped.\n", enddst - temp1);
	 break; 
       }
     } /*outer bubble-sort for*/
   } /*parent for*/
   
    beglevel = endlevel;
    endlevel = dst;
  }
  LevelHead[level-1] = beglevel;
}

/* Arrange printing coordinates. This routine assumes that 
   Nodes have been sorted by level and position so that the nodes appear
   from ROOT_LEVEL and within that top to bottom to
   TreeDepth and within that top to bottom.
*/
static void 
arrange(void)
{
  TREENODE *node;
  unsigned int xpos, next_xpos;
  unsigned int xfill;
  unsigned int xspace, yspace=0, last_yspace;
  int new_width = 0;
  unsigned int i;
  treelevel level;

  /* Compute the new height of the canvas. */
  int new_height = maxDepth * (active.fontHeight+MIN_YSPACE);

  if (maxDepth == 0) return;

  /* Compute the number of empty pixels to distribute horizontally 
     between words. 
  */
  xfill = active.width;
  for (level=ROOT_LEVEL; level <= TreeDepth; level++) {
    Virtual_Breadth[level] = Breadth[level];
    /* If we know there is going to be a big fanout, increase the 
       width between levels by adding to MaxLabel */
    if (level != TreeDepth && (Breadth[level]+5 < Breadth[level+1])) {
      MaxLabel[level] += 50;
    }
    if (xfill > MaxLabel[level])
      xfill -= MaxLabel[level];
  }
  
  xspace = xfill / (TreeDepth + 1);
  
  if (base.debugging & DBG_DISPLAY) {
    printf("active.width %d, active.height %d, xfill: %d, xspace %d\n", 
	   active.width, active.height, xfill, xspace);
    printf("maxDepth %ld\n", maxDepth);
  }
  
  
  /* Keep horizontal spacing withing acceptable bounds */
  xspace = MIN(MAX_XSPACE, xspace);
  xspace = MAX(MIN_XSPACE, xspace);
  
  next_xpos = xspace / 2;
  
  /* Position each node of the tree. Keep track of the maximum boundaries...
   */
  node = Node;
  for (level = ROOT_LEVEL; level <= TreeDepth; level++) {
    /* Compute the next level's horizontal position. */
    xpos      = next_xpos;
    next_xpos = xpos + MaxLabel[level] + xspace;
    
    new_width = MAX(new_width, next_xpos - xspace);
    
    if (IsDegree1[level]) {
      /* Set position of each node on this level horizontally across from 
	 its parent. */
      /* I believe the other heuristics will catch this case so we could
	 in theory remove this. However handling this special case is easy,
	 and fast. */

      if (base.debugging & DBG_DISPLAY) {
	printf("Level %ld degree 1 parent\n", level);
      }
      Virtual_Breadth[level] = Virtual_Breadth[level-1];
      for (i=1; i<=Breadth[level]; i++, node++) {
	node->y    = node->parent->y;
	node->x    = xpos;
	node->rank = i;
	node->virtual_rank = node->parent->virtual_rank;
      }
    } else {
      /* Can't position all nodes on this level so that it is horizontally
	 across from its parent.
       */

      unsigned int ypos=0;
      TREENODE *last_node = NULL;
      unsigned int virtual_rank;
      int virtual_rank_diff;       /* Must allow negative! */

      unsigned int child=0;        /* Which child of parent am I? */
      last_yspace = yspace;

      /* Use previous spacing unless this level has more children than last. */
      if (level == ROOT_LEVEL || Breadth[level] > Virtual_Breadth[level-1]) {
	yspace = (new_height - Breadth[level]*active.fontHeight) 
	         / Breadth[level];
    
	/* Keep vertical spacing withing acceptable bounds. */
	yspace = MAX(MIN_YSPACE, yspace);
      } else {
	Virtual_Breadth[level] = Virtual_Breadth[level-1];
      }
      
      
      if (base.debugging & DBG_DISPLAY) printf("yspace %d\n", yspace);

      virtual_rank = 0;
      /* Position vertically by equally distributing the space. */
      for (i=1; i<=Breadth[level]; i++, last_node=node++) {
	int y    = ypos + (yspace+1)/2;
	node->y  = y + active.fontHeight;
	ypos     = node->y + yspace / 2;
	
	node->x    = xpos;
	node->rank = i;
	node->virtual_rank = ++virtual_rank;

	if ( level == ROOT_LEVEL) continue;

	/* Which child of the parent are we? */
	if (last_node && node->parent == last_node->parent) {
	  child++;
	} else {
	  child=1;
	}
	
	virtual_rank_diff =  node->virtual_rank - node->parent->virtual_rank;
	
	if (virtual_rank_diff <= 0 
	    && Virtual_Breadth[level] == Virtual_Breadth[level-1]) {
	  /* Parent is further down than this node and we the virtual
	     we are using the same virtual breadth.

	     Try position the virtual rank so the mid child is
	     as close to or less to the rank of the parent, but not so much 
	     that the overall virtual breadth is not increased. 

	     NOTE: all *diff things are measured in the negative! (or up)
	  */
	  int togo = Breadth[level] - node->rank;
	  int diff = Virtual_Breadth[level-1] - node->parent->virtual_rank 
	           - togo;
	  int new_ypos;
	  int child_diff = child - (node->parent->children/2+1);
	  int new_virtual_rank;

	  if (diff >= 0) {
	    /* This node room to spare, even if it is across from 
	       the parent. Set for this since we don't want it further
	       down than that. */
	    diff = 0;
	  }
	  
	  if (child_diff < diff) {
	    /* As positioned now, the mid child would be further down
	       than its parent. Adjust the mid child up if possible. */
	    if (virtual_rank_diff < child_diff) {
	      diff = child_diff;
	    } else {
	      /* Just keep things the way they were going to be anyway. */
	      diff = virtual_rank_diff;
	    }
	  }
	  new_virtual_rank = node->parent->virtual_rank + diff;

	  new_ypos = node->parent->y + (diff*(yspace+active.fontHeight));

	  /* The below tends to position the first children across
	     from the parent when we'd really like the mid child positioned
	     across, all things being equal. */

	  if ( node->virtual_rank <= new_virtual_rank ) {
	    virtual_rank       = new_virtual_rank;
	    node->virtual_rank = virtual_rank;
	    node->y            = new_ypos;
	    ypos               = node->y + yspace / 2;
	  }
	}
      }
    }
  }

  /* Readjust the overall canvas dimensions? Don't adjust width unless
     it has shrunk by a significant amount. */
  if ( new_width > active.width || (active.width - new_width) > MIN_SHRINK
      || active.height != new_height ) {
    if (base.debugging & DBG_DISPLAY) 
      printf("new_width %d, new_height: %d\n", new_width, new_height);

    active.width  = new_width;
    active.height = new_height;
    XtVaSetValues(active.canvas, XmNunitType, XmPIXELS,
		  XmNwidth, new_width, XmNheight, new_height, NULL);
  }
  
}

static unsigned int num_processes = 0;
static unsigned int refresh_rate  = 0;
static Boolean      beep          = False;
static Boolean      Zoomed        = False;

/* Give status in message area. */
extern void 
updateMessage(void)
{
  char buf[MAXHOSTNAMELEN+100];
  double one, five, fifteen;
  
  beep          = active.beep;
  num_processes = ACTIVE_PROCS;

  if (refresh_rate != base.interval || base.zoomed != Zoomed) {
    Zoomed = base.zoomed;
    refresh_rate  = base.interval;
    sprintf(buf, "%s version %s: Refresh rate %3.1f seconds", 
  	    base.program, VERSION, (double) (refresh_rate / 1000.0));
    set_label(buf, active.message2);
  }
  
  if (xps_getloadavg(&one, &five, &fifteen)) {
    sprintf(buf, "Load averages: %4.2f, %4.2f, %4.2f, %d processes", 
	    one, five, fifteen, num_processes );
    set_label(buf, active.message);
  }

#ifdef EXPERIMENTAL
  /* Make sure titles of various buttons are set correctly. 
     Should add perhaps a bitmask to indicate what might have changed:
     e.g. 1 = zoom, 2=beep, 3=all procs
   */
  
  if (base.zoomed != Zoomed) {
    Zoomed = base.zoomed;
    if (base.zoomed ) {
      set_label("Zoom out", active.zoom_button);
    } else {
      set_label("Zoom in" , active.zoom_button);
    }
  }
#endif
}

/* Draw the process forest/tree. */
extern void 
draw(Display *display, Window window)
{
  TREENODE *node;
  
  for (node = Node; node != EndNode; ++node)
    {
      /* draw node */
      
      XSetForeground(display, base.nodeStyle, node->color);
      if (base.debugging & DBG_DRAW) 
	printf("(%d,%d): %s\n", node->x, node->y, node->label);
      
      /* draw arc to parent node */
      if (node->parent) {
	/* How much space to leave between end of parent label and
           beginning of line? */
	/*unsigned int xspace = node->parent->wid + 
	  (MaxLabel[node->parent->depth] - node->parent->wid)/2;*/
	unsigned int xspace = node->parent->wid; 
	XDrawLine(display, window, base.lineStyle, 
		  node->x - 1, node->y - active.fontHeight / 3,
		  node->parent->x + xspace, 
		  node->parent->y - active.fontHeight / 3);
      }

      XDrawString(display, window, base.nodeStyle, 
		  node->x, node->y, node->label, strlen(node->label));
      
    }
  
  /* Draw process count and other status, but only if it's changed. */
  if ( refresh_rate != base.interval || num_processes != ACTIVE_PROCS 
       || beep != active.beep) {
    updateMessage();
  }
}

/* Figure out which node was selected with the mouse and
  ask user to confirm that the corresponding process is to be killed.
  Taken:       mouse coordinates
  Returned:    True if clicked on a node (and update pid arg),
               or False if no node was found at the mouse position.
 */
Boolean
find_pid_under_mouse(Display *dpy, Window window, int x, int y, pid_t *pid)
{
  TREENODE *node;
  treelevel level;
  
  /* bring all node positions up to date */
  ForceRedisplay();

  /* Find node which was selected. 
   */
#ifdef OLD  
  /* This code does a linear search over all nodes! 
     We can do better!
   */
  for (node = Node; node < EndNode; ++node)
     if ( x >= node->x                                       &&
	  x <  node->x + XTextWidth(active.font, node->label, 
				    strlen(node->label))     &&
	  y <  node->y                                       &&
	  y >= node->y - active.fontHeight ) {
       *pid = node->pid;
       return True;
     }
#endif

   /* 
      This code does a linear search over the tree levels and then
      within that a linear search within the level.  A binary search
      might speed things up here and make cursor selection a tad
      faster. 

      But then again, it might not be a bottle neck.  
   */
   for (level=TreeDepth; level >= ROOT_LEVEL; level--) {
     if (x >= LevelHead[level]->x) {
       /* Found the level that mouse has to be in. */
       for (node = LevelHead[level]; node < LevelHead[level+1]; ++node) {
	 if (y < node->y) {
	   /* It's either this node or nothing at all. */
	   if (	x <  node->x + XTextWidth(active.font, node->label, 
					  strlen(node->label))     &&
		y >= node->y - active.fontHeight ) {
	     *pid = node->pid;
	     return True;
	   } else {
	     return False;
	   }
	 }
       }
       return False;
     }
   }
   return False;
}

#define HASH_TABLE_SIZE 53

typedef struct {
  uid_t uid;
  Pixel color;
} userHashRec;

userHashRec hashtable[HASH_TABLE_SIZE];

/* Hash a uid to the range */
unsigned int 
hash_uid (uid_t uid)
{
   return (uid % HASH_TABLE_SIZE);
}

Boolean 
insert_uid(uid_t uid) 
{
#ifdef USE_HASH
  unsigned int h = hash_uid(uid);
  unsigned int i;
#endif
  return True;
}

/* Generate a color based on a hash of the uid. */
static unsigned int 
new_color_hash(uid_t uid) 
{
  unsigned int color;
  unsigned int mod2, mod3, mod5;

  mod2 = (uid % 2)*15;
  mod3 = (uid % 3)*7;
  mod5 = (uid % 5)*3;
  color = (mod2 << 8) + (mod3 << 4) + mod5;
  return color;
}


/* 
   Find uid in base.uid[] and return the index of this. 
   If not in table return the index of a free index. 
   If the table is full and uid is not found then return BOGUS_UID.
*/
user_color_index
find_uid_color_index(uid_t uid) 
{
  user_color_index i;
  user_color_index last_free=NUM_USER_COLORS;
  for (i = 0; i<NUM_USER_COLORS; i++) {
    if (uid == base.uid[i]) {
      return i;
    } else if (base.uid[i] == BOGUS_UID && last_free != NUM_USER_COLORS) {
      last_free = i;
    }
  }
  return last_free;
}

/* To speed up color lookup, remember the last uid found and its color. */

static uid_t last_uid   = BOGUS_UID; /* This is sort of bogus...*/
static Pixel last_color = 0;

/* Return the color associated with uid. */
Pixel 
GetUID_Color (uid_t uid) 
{
  unsigned int i;
  /* Check one-variable cache. */
  if (uid == last_uid) {
    return last_color;
  }

  last_uid = uid;
  i = find_uid_color_index(uid);
  if (i != NUM_USER_COLORS) {
    /* In table, but could still be a new free uid. */
    if (base.uid[i] != uid) {
      base.uid[i] = uid;
      base.userColor[i] = new_color_hash(uid);
    }
    /* Is now in table. */
    last_color = base.userColor[i];
  } else {
    /* Not in table, and table is full. Generate a color hash. */
    last_color = new_color_hash(uid);
  }
  return last_color;
}


#ifndef HAVE_IDTOUSER
char *
IDtouser(uid_t uid) {
     struct passwd *p = getpwuid(uid);
     return (p == NULL) ? NULL : p->pw_name;
}
#else 
#ifdef HAVE_USERSEC_H
#include <usersec.h>
#endif
#endif

/* 
   Get uid from user name "name." 
   Returns True if found, and sets uid. 
*/
Boolean
usertoID(const char *name, /*out*/ uid_t *uid ) {
     struct passwd *p = getpwnam(name);
     if (p == NULL) {
       return False;
     } else {
       *uid = p->pw_uid;
       return True;
     }
}

/* Number of processes we are working with. */
unsigned int NumProcs;
ProcInfo *Proc;               /* Points to array of process information */

/* See xps.h for a description of what these arrays do. */
TREENODE         Node[XPS_MAX_PROCS];
TREENODE        *EndNode;             /* end marker */

/* Top-level routine to gather process information, sort processes
   layout tree and cause it to be displayed. */
void 
display(Boolean forceRedisplay, Display *display, Window window)
{
  static long unsigned int PidChecksum;
  static long unsigned int WidChecksum;
  static long          int ColorChecksum;
  static TREENODE      OldNode[XPS_MAX_PROCS];
  static TREENODE      *OldEndNode;      /* end marker of previous copy */
  
  unsigned int i;

  unsigned long int pidChecksum = 0;
  unsigned long int widChecksum = 0;
  unsigned long int colorChecksum = 0;

  ProcInfo *proc;
  int cnt;
  
  if (base.debugging & DBG_DISPLAY) 
    printf ("display: forceRedisplay %d\n", forceRedisplay);

  NumProcs=GetProcInfo();
  EndNode = Node;

  if (NumProcs==0) {
    /* Nothing to display. Could be something wrong, or we are 
       looking at a user that is not running right now. */
    return;
  }
    
  
  /* extract live process table entries */
  OldEndNode = OldNode;

  memset(Node, 0, sizeof(struct node)*NumProcs);
  for (cnt = NumProcs, proc = Proc; cnt--; ++proc)
    {
      if (!GetStatus(proc))
        continue;
      
      GetProcName(proc,EndNode);
      
      EndNode->wid   = XTextWidth(active.font, EndNode->label,
				  strlen(EndNode->label));
      
      pidChecksum   += EndNode->pid;
      widChecksum   += EndNode->wid;
      colorChecksum += EndNode->color;
      ++EndNode;
    }

  /* stop here if there's nothing to do */
  if (pidChecksum == PidChecksum &&
      widChecksum == WidChecksum &&
      colorChecksum == ColorChecksum &&
      OldEndNode    == EndNode &&
      forceRedisplay == False)
    return;
  
  if (active.beep) {
      /* Number of processes increased or decreased? */
      if (pidChecksum > PidChecksum)
	xpsplay(display, base.beep_sound_up); 
      else if (pidChecksum < PidChecksum)
	xpsplay(display, base.beep_sound_down); 
  } /* endif */
  
  /* initialize variables */
  TreeDepth = 0;     
  maxDepth  = 0;
  memset(Breadth,      0, sizeof(int)         * MAX(ACTIVE_PROCS, 1));
  memset(MaxLabel,     0, sizeof(unsigned int)* MAX(ACTIVE_PROCS, 1));
  memset(Level,       -1, sizeof(unsigned int)* MAX(ACTIVE_PROCS, 1));
  memset(IsDegree1, True, sizeof(Boolean)     * MAX(ACTIVE_PROCS, 1));

  /* The root level (0), is a special case we want it to alway be arranged. */
  IsDegree1[0] = False;

  /* 
     Measure tree and node dimensions. Since the process structure has 
     parent links we will traverse from the bottom up.
     */
  for (i = 0; i < ACTIVE_PROCS; i++) if (untraversed(i)) traverse(i);
  
  /* sort nodes by level and parent process id */
  sort();
  
  /* Save a copy of what the Node structure looked like before it 
     gets permuted, for the next go around. */
  memcpy(OldNode, Node, sizeof(struct node)*ACTIVE_PROCS);
  
  /* arrange nodes for printing */
  arrange();
  
  /* clear */
  if (pidChecksum != PidChecksum || widChecksum != WidChecksum 
      || ColorChecksum != colorChecksum || forceRedisplay) {
    XClearWindow(display, window);
  
    /* draw */
    draw(display, window);
  }

  /* remember what we drew */
  PidChecksum = pidChecksum;
  WidChecksum = widChecksum;
  ColorChecksum = colorChecksum;
}

/* Print usage help and exit */
void 
usage(int exitcode)
{
  fprintf(stderr,"\n%s: version %s\n", base.program, VERSION);
  fprintf(stderr,"\nusage: %s [X options] [xps options]\n",base.program);
  fprintf(stderr,"\nwhere %s options are:\n\n", base.program);
  fprintf(stderr,
"\t-interval  secs      Screen update frequency\n\
\t-kernel              Show kernel processes in display?\n\
\t-allprocs            Show  all process info\n\
\t-zoomed              Start in zoomed mode?\n\
\t-debugging dbg-val   Print debugging information\n\
\t-beep                Beep on process changes\n\
\t-zfont     font      Font to use in zoomed window\n\
\t-zfn       font      Synonym for -zfont\n\
\t-zgeometry WxH+X+Y   Geometry for zoomed window\n\
\t-swap      color     Color for swapped  processes\n\
\t-waiting   color     Color for waiting  processes\n\
\t-running   color     Color for running  processes\n\
\t-active    color     Color for active   processes\n\
\t-selected  color     Color for the selected process\n\
\t-idle      color     Color for idle     processes\n\
\t-zombie    color     Color for zombie   processes\n\
\t-stopped   color     Color for stopped  processes\n\
\t-kill                Enable Quick Kill\n\
\t-metoo               Show me running too.\n\
\t-pscmdline string    Command to give ps cmdline. Should have %%d for pid\n\
\t-pscmdinfo string    Command to give ps info. Should have %%d for pid\n\
\t-version             Print version information and build info\n\
\t-help                Print this usage information\n\
\n");
  exit(exitcode);
}

void 
show_version(void)
{
  printf (config_info, MyPid, MyPid);
  exit(2);
}

/* Initializations common to all platforms */
void 
common_init(int argc, char *argv[]) {

  char *user;   /* remember who we are */
  int b;
  uid_t u = getuid();

  base.program = argv[0];
  
  if (gethostname(nodename, MAXHOSTNAMELEN) != 0)
    strcpy(nodename, UNKNOWN_USER);

  MyPid = getpid();
  user = IDtouser(u);

  if (user == NULL) {
    user = UNKNOWN_USER;
  }
  
  for (b=0; b<=1; b++) {
    if (NULL != (base.info[b].user_name = 
		 (char *) malloc(sizeof(char) * strlen(user) + 1))) {
      strcpy(base.info[b].user_name, user);
    }
    base.info[b].uid_displayed = u;
#ifdef HAVE_REGEX
    base.info[b].match_regexp = False;
#endif
  }

  for (b=0; b<NUM_USER_COLORS; b++) {
    base.uid[b] = BOGUS_UID;
  }
  
#ifdef HAVE_KILL
  base.sig = SIGTERM;
#endif

  memset(Node, 0, sizeof(struct node)*NumProcs);

#ifdef RUN_SUID_ROOT
  real_uid = getuid();
  seteuid(0);
#endif

  {
    char buf[600];

#define append_config(x) sprintf x; strcat(config_info, buf);
    
    sprintf(config_info, "\n%s: version %s, by Rocky Bernstein\n\n", 
	    base.program, VERSION);
    append_config((buf, 
"Copyright (C) 1998-2001,2003 Free Software Foundation, Inc.\n\
This is free software; see the source for copying conditions. There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\
\n\n"));

    append_config((buf, "Tick rate: %d milliseconds\n", XPS_TICK));
    append_config((buf, "Smallest refresh rate: %d ticks\n", 
	     XPS_MIN_REFRESH_RATE));
    append_config((buf, "Largest refresh rate: %d ticks\n", 
	     XPS_MAX_REFRESH_RATE));
    append_config((buf, "Bogus PID used: %d\n", BOGUS_PID));
    append_config((buf, "Maximum number of processes we can handle: %d\n", 
	     XPS_MAX_PROCS));

#ifdef HAVE_REGEX
    strcat(config_info, "Built with regular expression support.\n");
#else 
    strcat(config_info, "Not built with regular expression support.\n");
#endif 
#ifdef XPS_HAVE_KERNEL_PROCS
    strcat(config_info, "OS supports distinguishing kernel processes.\n");
#else 
    strcat(config_info, 
	   "OS does not support distinguishing kernel processes.\n");
#endif 
#ifdef PSCMD    
    append_config((buf,
	    "\nDefault ps command used to show ps status:\n\t%s.\n", 
	    PSCMD));
#endif
#ifdef PS_CMDLINE
    append_config((buf, 
	     "\nDefault ps command used to show ps command line:\n\t%s.\n", 
	     PS_CMDLINE));
#endif
  }
  
}
