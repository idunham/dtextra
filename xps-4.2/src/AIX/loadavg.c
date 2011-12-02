/* $Id: loadavg.c,v 1.1 2002/12/02 14:55:18 rockyb Exp $ */
#include "config.h"
#include "os.h"
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <stdio.h>
#include <nlist.h>
#include "loadavg.h"

#define X_AVENRUN 0
static struct nlist nlst[] = {
    { {"avenrun"}, 0, 0, {0}, 0, 0 }, /* 0 */
    { { NULL}    , 0, 0, {0}, 0, 0 }
};
#define KMEM "/dev/kmem"

/* offsets in kernel */
static unsigned long avenrun_offset=0;
int    kmem;			/* kernel memory file descriptor */


void
loadavg_init() {
  if ((kmem = open(KMEM, O_RDONLY)) == -1) {
    return;
  }

  /* get kernel symbol offsets */
  if (knlist(nlst, 1, sizeof(struct nlist)) != 0) {
    return;
  }
  avenrun_offset = nlst[X_AVENRUN].n_value;
}

/*
   getkval(offset, ptr, size, refstr) - get a value out of the kernel.
 	"offset" is the byte offset into the kernel for the desired value,
   	"ptr" points to a buffer into which the value is retrieved,
   	"size" is the size of the buffer (and the object to retrieve),
   	"refstr" is a reference string used when printing error meessages,
	if "refstr" starts with a '!', then a failure on read will not
	be fatal (this may seem like a silly way to do things, but I
	really didn't want the overhead of another argument).
 */
static int
getkval(offset, ptr, size, refstr)
    unsigned long offset;
    caddr_t ptr;
    int size;
    char *refstr;
{
    int upper_2gb = 0;

    /* reads above 2Gb are done by seeking to offset%2Gb, and supplying
     * 1 (opposed to 0) as fourth parameter to readx (see 'man kmem')
     */
    if (offset > 1<<31) {
	upper_2gb = 1;
	offset &= 0x7fffffff;
    }

    if (lseek(kmem, offset, SEEK_SET) != offset) {
	return 0;
    }

    if (readx(kmem, ptr, size, upper_2gb) != size) {
      if (*refstr == '!')
	return 0;
      else 
	return 0;
    }
    return 1 ;
}

/* 
   Set the 1-, 5-, and 15-minute load averages and return the number
   of items 0..3 that were set. 
*/
int 
xps_getloadavg (double *one, double *five, double *fifteen) {
    int load_avg[3];
    
    /* get the load avarage array */
    if (!getkval(avenrun_offset, (caddr_t)load_avg, 
		sizeof load_avg, "avenrun")) {
      return 0;
    }
    

    /* convert load avarages to doubles */
    *one     = (double)load_avg[0]/65536.0;
    *five    = (double)load_avg[1]/65536.0;
    *fifteen = (double)load_avg[2]/65536.0;
    return 1;
}
 
#ifdef STANDALONE
int 
main() 
{
  double a=-1.0;
  double b=-1.0;
  double c=-1.0;

  loadavg_init();
  xps_getloadavg(&a, &b, &c);
  printf("one: %4.2f, five: %4.2f, fifteen: %4.2f\n", a, b, c);
  return 0;
}
#endif  
