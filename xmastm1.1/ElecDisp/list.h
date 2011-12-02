/*
** list.h for Hydrix
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul  4 11:32:43 1994 hydrix
** Last update Mon Nov 21 09:19:20 1994 hydrix
*/

#ifndef __STD_LIST_H__
#define __STD_LIST_H__	1

#include <stdio.h>

typedef struct		_std_list_elt
{
   struct _std_list_elt	*next;
   struct _std_list_elt	*prev;
} std_list_elt;

typedef void		*std_list_header[2];

typedef struct		_std_list
{
   std_list_elt		*first;
   std_list_elt		*last;
   std_list_elt		*current;
   unsigned int		num_elt;
} std_list;

#define LIST_INIT(list)		(list)->first = (list)->last =\
(list)->current = NULL,\
(list)->num_elt = 0;
#define LIST_REWIND(list)	(list)->current = (list)->first
#define LIST_END_SEEK(list)	(list)->current = (list)->last
#define LIST_NUMBER_ELT(list)	((list)->num_elt)

extern void		list_add_last();
extern void		list_add_first();
extern void		list_destroy_current();
extern void		list_destroy_first();
extern void		list_destroy_last();
extern void		list_empty();
extern void		list_prev();
extern void		list_next();
extern void		*list_get_current();
extern void		*list_get_first();
extern void		*list_get_last();
extern void		*list_get_next();
extern void		*list_get_prev();
extern void		*list_get_elt_n();
extern void		list_apply();
extern void		list_apply_and_free();

extern std_list		list_nil;

#endif /* __STD_LIST_H__ */
