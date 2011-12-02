/* 
   Subroutine and standalone program to get/print Solaris load averages.
   $Id: loadavg.c,v 1.1 2002/12/02 15:25:45 rockyb Exp $
   Copyright (C) 1998,99 Rocky Bernstein
   
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
#include "config.h"
#include "os.h"
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <nlist.h>
#include "loadavg.h"
#include <kvm.h>
#include <kstat.h>

#ifndef FSCALE
#define FSHIFT  8		/* bits to right of fixed binary point */
#define FSCALE  (1<<FSHIFT)
#endif /* FSCALE */

#define loaddouble(la) ((double)(la) / FSCALE)

/* 
   Set the 1-, 5-, and 15-minute load averages and return the number
   of items 0..3 that were set. 
*/
int 
xps_getloadavg (double *one, double *five, double *fifteen) {
  kstat_ctl_t *kc;
  kstat_t *ksp;
  kstat_named_t *kn;
  int nelem = 0;

  kc = kstat_open ();
  if (kc == 0) return 0;

  ksp = kstat_lookup (kc, "unix", 0, "system_misc");
  if (ksp == 0 ) return 0;
  if (kstat_read (kc, ksp, 0) == -1) return 0;


  kn = kstat_data_lookup (ksp, "avenrun_1min");
  if (kn != 0) {
    *one = loaddouble(kn->value.ul);
    nelem++;
  }

  kn = kstat_data_lookup (ksp, "avenrun_5min");
  if (kn != 0) 	{
    *five = loaddouble(kn->value.ul);
    nelem++;
  }
  
  kn = kstat_data_lookup (ksp, "avenrun_15min");
  if (kn != 0) {
    *fifteen = loaddouble(kn->value.ul);
    nelem++;
  }
  kstat_close (kc);
  return nelem;
}
 
#ifdef STANDALONE
int 
main() 
{
  double a=-1.0;
  double b=-1.0;
  double c=-1.0;

  xps_getloadavg(&a, &b, &c);
  printf("one: %4.2f, five: %4.2f, fifteen: %4.2f\n", a, b, c);
  return 0;
}
#endif  
