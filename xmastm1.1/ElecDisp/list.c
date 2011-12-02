/*
** list.c for Hydrix
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul  4 11:36:12 1994 hydrix
** Last update Thu Nov 17 16:35:23 1994 hydrix
*/

#include <stdio.h>
#include <sysdep.h>
#include <list.h>

std_list		list_nil =
{
   NULL,NULL,NULL,0
};

void		list_add_last(l,new)
std_list	*l;
std_list_elt	*new;
{
   if (l->first == NULL)
      l->first = new;
   else
      l->last->next = new;
   new->prev = l->last;
   l->last = new;
   new->next = NULL;
   l->current = new;
   l->num_elt++;
}

void		list_add_first(l,new)
std_list	*l;
std_list_elt	*new;
{
   if (l->last == NULL)
      l->last = new;
   else
      l->first->prev = new;
   new->next = l->first;
   l->first = new;
   new->prev = NULL;
   l->current = new;
   l->num_elt++;  
}

void		list_destroy_current(l)
std_list	*l;
{
   std_list_elt	*tmp;
   
   if (l->current == NULL)
      return ;
   tmp = l->current->next;
   if (l->current->prev != NULL)
      l->current->prev->next = l->current->next;
   else
      l->first = l->current->next;
   if (l->current->next != NULL)
      l->current->next->prev = l->current->prev;
   else
      l->last = l->current->prev;
   free(l->current);
   l->current = tmp;
   l->num_elt--;
}

void		list_destroy_first(l)
std_list	*l;
{
   std_list_elt	*tmp;
   
   if (l->first == NULL)
      return ;
   tmp = l->first->next;
   if (l->current == l->first)
      l->current = tmp;
   free(l->first);
   if (tmp == NULL)
      l->last = NULL;
   else
      tmp->prev = NULL;
   l->first = tmp;
   l->num_elt--;  
}

void		list_destroy_last(l)
std_list	*l;
{
   std_list_elt	*tmp;
   
   if (l->last == NULL)
      return ;
   tmp = l->last->prev;
   if (l->current == l->last)
      l->current = tmp;
   free(l->last);
   l->last = tmp;
   if (tmp == NULL)
      l->first = NULL;
   else
      tmp->next = NULL;
   l->last = tmp;
   l->num_elt--;
}

void		list_empty(l)
std_list	*l;
{
   while (l->first)
      list_destroy_first(l);
}

void		list_prev(l)
std_list	*l;
{
   if (l->current == NULL)
      return ;
   l->current = l->current->next;
}

void		list_next(l)
std_list	*l;
{
   if (l->current == NULL)
      return ;
   l->current = l->current->next;
}

void		*list_get_current(l)
std_list	*l;
{
   return (l->current);
}

void		*list_get_elt_n(l,nb)
std_list	*l;
unsigned int	nb;
{
   unsigned int	n;
   
   n = 0;
   LIST_REWIND(l);
   while (l->current && n < nb)
   {
      list_next(l);
      n ++;
   }
   return (l->current);
}

void		*list_get_first(l)
std_list	*l;
{
   l->current = l->first;
   return (l->current);
}

void		*list_get_last(l)
std_list	*l;
{
   l->current = l->last;
   return (l->current);
}

void		*list_get_next(l)
std_list	*l;
{
   if (l->current == NULL)
      return (NULL);
   l->current = l->current->next;
   if (l->current == NULL)
      return (NULL);
   else
      return (l->current);
}

void		*list_get_prev(l)
std_list	*l;
{
   if (l->current == NULL)
      return (NULL);
   l->current = l->current->prev;
   if (l->current == NULL)
      return (NULL);
   else
      return (l->current);
}

void		list_apply(l,proc)
std_list	*l;
void		(*proc)();
{
   LIST_REWIND(l);
   while (l->current != NULL)
   {
      proc(l);
      list_next(l);
   }
}

void		list_apply_and_free(l,proc)
std_list	*l;
void		(*proc)();
{
   LIST_REWIND(l);
   while (l->current != NULL)
   {
      proc(l);
      list_destroy_current(l);
   }
}
