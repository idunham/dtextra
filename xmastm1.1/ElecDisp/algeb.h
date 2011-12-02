/*
** algeb.h for Hydrix
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Sep  5 11:09:13 1994 hydrix
** Last update Mon Sep  5 11:53:30 1994 hydrix
*/

#define ABS(x)			(((x) < 0)?-(x):(x))

#ifndef MAX
#define MAX(x,y)	((x)>(y)?(x):(y))
#endif /* MAX */

#ifndef MIN
#define MIN(x,y)	((x)<(y)?(x):(y))
#endif /* MIN */
