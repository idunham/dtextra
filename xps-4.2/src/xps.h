/* Include file for xps. 
 * $Id: xps.h,v 1.1 2002/12/01 22:00:56 rockyb Exp $
*/

#define MAXLABEL 16
/* Implimentation note. For a while I had parallel data structures 
   such as for the level
   field or parent field. This may have the advantage of making access a bit 
   faster, but it has the disadvantage of making initialization of such 
   structures harder. For example one can't use a single memset to clear out 
   fields. 

   Beware! The below data structure uses 0 (such as in level) as a special
   sentinal value for say unitialized data.

   Also, moving or swapping structures can be easier if all of the 
   information is contained inside the structure. For example to 
   swap or move structures with parallel arrays, one must use an index 
   into an array rather than a pointer. For pointer-based swaps, this 
   would mean computing the index from the pointer and pointer base, the 
   latter of which may not be known.
*/
typedef struct node
   {
   pid_t  pid;             /* process id */
   pid_t  ppid;            /* parent process id */
   int    color;           /* foreground color */
   char   label[MAXLABEL]; /* printstring for this node */
   char   label1[MAXLABEL];/* real printstring */
   int    show_username;   /* whether label is user-name (or process-name) */
   int    wid;             /* dimensions of node (pixels) */
   int    x, y;            /* window coordinates of node center (pixels) */
   struct node *parent;    /* link to parent node */
   uid_t  uid;             /* userid */
   unsigned int children;  /* Number of children that this node has. This 
			      information is useful for tree display. 
			      Prodigious nodes should be near the center. */
   unsigned int rank;      /* Node's position among siblings within level. 
			      Goes from 1..Breadth[level] and is used
			      for arranging display. 
			    */
   unsigned long int depth;
   unsigned int virtual_rank; /* Node's rank taking into account
				  space adjustments due to previous level */
 } TREENODE;

extern TREENODE    Node[];             /* corresponding graphical tree
					  representation */
extern TREENODE    *EndNode;           /* end marker beyond the last use */

#define ROOT_PID    0     /* root process id */
#define INIT_PID    1     /* init process id */

/* Initializations that might be specific to a given platform. 
   See also common_init() for cross-the-board initializations. */
extern void machine_specific_init(void);

/* Fills in process table info into global Proc and 
   returns the number of entries. */
extern unsigned int GetProcInfo(void);

/* Number of processes we are working with. The return value
  of GetProcInfo(). */
extern unsigned int NumProcs;

/* Points to array of process information */
extern ProcInfo *Proc; 

/* Store node info: pid, ppid, color, ... */
extern Boolean GetStatus(ProcInfo *proc);

extern void GetProcName(ProcInfo *proc, TREENODE *node);

