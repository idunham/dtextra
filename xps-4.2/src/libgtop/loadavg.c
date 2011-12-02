/* 
   Subroutine and standalone program to get/print Linux load averages.
   $Id: loadavg.c,v 1.1 2002/12/01 23:52:55 rockyb Exp $ 
   Copyright (C) 1998-2002 Rocky Bernstein
   
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

#include "loadavg.h"

#include <glibtop/union.h>

int 
gnopstree_getloadavg (double *one, double *five, double *fifteen) {
  glibtop_union data;

  glibtop_get_loadavg (&data.loadavg);
  *one     = data.loadavg.loadavg[0];
  *five    = data.loadavg.loadavg[1];
  *fifteen = data.loadavg.loadavg[2];
  return 3;
}
  
#ifdef STANDALONE
#include <stdio.h>

int 
main() 
{
  double a=-1.0;
  double b=-1.0;
  double c=-1.0;
  gnopstree_getloadavg(&a, &b, &c);
  printf("one: %4.2f, five: %4.2f, fifteen: %4.2f\n", a, b, c);
  return 0;
}
#endif  
