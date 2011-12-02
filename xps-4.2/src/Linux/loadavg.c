/* 
   Subroutine and standalone program to get/print Linux load averages.
   $Id: loadavg.c,v 1.1 2002/12/02 15:34:14 rockyb Exp $ 
   Copyright (C) 1998 Rocky Bernstein
   
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
#include <stdlib.h> /* For sscanf()... */
#include <unistd.h> /* For close()... */
#include <stdio.h>
#include "loadavg.h"

#define LOADINFO "/proc/loadavg"
#define LINESIZE 255

int 
xps_getloadavg (double *one, double *five, double *fifteen) {
  static FILE *fp_loadavg = NULL;
  char line[LINESIZE];
  
  if ( (fp_loadavg = fopen(LOADINFO, "r")) == NULL ) {
    return 0;
  }

  if( fgets( line, LINESIZE, fp_loadavg ) != NULL ) {
    fclose(fp_loadavg);
    return 3==sscanf(line, "%lf %lf %lf", one, five, fifteen);
  }
  fclose(fp_loadavg);
  return 0;
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
}
#endif  
