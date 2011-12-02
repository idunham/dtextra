/*
** sysdep.h for Hydrix
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul  4 11:55:31 1994 hydrix
** Last update Fri Nov  4 21:45:23 1994 hydrix
*/

#ifndef __SYSDEP_H__
#define __SYSDEP_H__	1

#include <stdio.h>
#include <errno.h>

void	*malloc();
void	*realloc();

#define EXIT_FAILURE		exit(255)
#define EXIT_SUCCEED		exit(0)

#define BUF_SIZE		512

#define SYSCALL_ERROR(syscall)	{perror(syscall);fprintf(stderr,"%s %d\n",\
							 __FILE__,__LINE__);\
				    EXIT_FAILURE;}		

#define MALLOC(ptr,size,comm)	{if (((ptr) = malloc(size)) == NULL)\
				 {perror("malloc");\
				     printf("File %s Line %d : %s\n",\
					    __FILE__,__LINE__,comm);\
				     EXIT_FAILURE;}}

#define REALLOC(ptr,size,comm)	{if (((ptr) = realloc((ptr),(size))) == NULL)\
				 {perror("realloc");\
				     printf("File %s Line %d : %s\n",\
					    __FILE__,__LINE__,comm);\
				     EXIT_FAILURE;}}

#define LISTEN_BACKLOG		4

#endif /* __SYSDEP_H__ */


