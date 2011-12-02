/* 
 $Id: loadavg.c,v 1.1 2002/12/02 15:45:51 rockyb Exp $
 */
#include "config.h"
#include "loadavg.h"
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

int 
xps_getloadavg (double *one, double *five, double *fifteen) {
  double loadavg[3];

  getloadavg(loadavg, 3);
  *one = loadavg[0];
  *five = loadavg[1];
  *fifteen = loadavg[2];

  return 1;
}

#ifdef STANDALONE
#include <stdio.h>
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
