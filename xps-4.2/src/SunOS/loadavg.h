/*
  $Id: loadavg.h,v 1.1 2002/12/02 15:25:45 rockyb Exp $
*/

void loadavg_init();

/* 
   Set the 1-, 5-, and 15-minute load averages and return the number
   of items 0..3 that were set. 
*/
int  xps_getloadavg (double *one, double *five, double *fifteen);



