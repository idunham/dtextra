/*
** parse.c for STD
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul 18 13:36:34 1994 hydrix
** Last update Mon Nov 21 09:36:07 1994 hydrix
*/

#include <stdio.h>
#include <sysdep.h>
#include "parse.h"

static void	parse_free(ps)
parse_state	*ps;
{
   int		i;
   compiled_cmd	*cc;

   i = 0;
   LIST_REWIND(&(ps->compiled_list));
   while (i < LIST_NUMBER_ELT(&(ps->compiled_list)))
   {
      cc = list_get_current(&(ps->compiled_list));
      if (cc->params)
      {
	 int	j;

	 j = 0;
	 while (j < cc->num_params)
	 {
	    free(cc->params[j]);
	    j++;
	 }
	 free(cc->params);
      }
      list_next(&(ps->compiled_list));
      i++;
   }
   list_empty(&(ps->compiled_list));
}

int		parse_compile(ps,cmd_defs,num_cmd_defs,buf,len,data)
parse_state	*ps;
cmd_def		*cmd_defs;
int		num_cmd_defs;
char		*buf;
int		len;
void		*data;
{
   int		i;
   int		match;
   compiled_cmd	*new;
   int		skip;
   
   LIST_INIT(&(ps->compiled_list));
   ps->buf = buf;
   ps->len = len;
   ps->data = data;
   ps->bp = buf;
   ps->state = PARSE_KEYWORD;
   ps->parse_len = 0;
   skip = 0;
   while (1)
   {
      switch (*(ps->bp))
      {
      case ' ':
      case '\t':
      case '\n':
	 (ps->bp)++;
	 continue ;
      case '\0':
	 if ((ps->state == PARSE_KEYWORD) &&
	     (ps->parse_len == 0))
	    return (PARSE_OK);
	 else
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }
      	 break ;
      case '(':
	 if (ps->parse_len == 0)
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }
	 ps->parse_buf[ps->parse_len] = '\0';
	 i = 0;
	 match = 0;
	 while (i < num_cmd_defs)
	 {
	    if (!strcmp(cmd_defs[i].keyword,ps->parse_buf))
	    {
	       MALLOC(new,sizeof (compiled_cmd),"parse");
	       new->func = cmd_defs[i].func;
	       new->params = NULL;
	       new->num_params = 0;
	       list_add_last(&(ps->compiled_list),new);
	       ps->parse_len = 0;
	       ps->state = PARSE_PARAM;
	       match = 1;
	       break ;
	    }
	    i++;
	 }
	 if (!match)
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }
	 break ;
      case ')':
	 if (ps->state != PARSE_PARAM)
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }  
      	 ps->state = PARSE_KEYWORD;
	 skip = 1;
      case ',':
	 if (skip)
	    skip = 0;
	 else
	    if (ps->state != PARSE_PARAM)
	    {
	       parse_free(ps);
	       return (PARSE_ERROR);
	    }    
      	 ps->parse_buf[ps->parse_len] = '\0';
	 (new->num_params)++;
	 if (new->params == NULL)
	    MALLOC(new->params,sizeof (char **),"parse")
	 else
	    REALLOC(new->params,
		    new->num_params*sizeof (char **),
		    "parse");
/*	 MALLOC(new->params[new->num_params-1],ps->parse_len,"parse");*/
     new->params[new->num_params-1]=XtMalloc(4096*(ps->parse_len));
	 strcpy(new->params[new->num_params-1],ps->parse_buf);
	 ps->parse_len = 0;
	 break ;
      case '\\':
	 (ps->bp)++;
	 if ((*(ps->bp)) == '\0')
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }      
      default:
	 if (ps->parse_len == MAX_KEYWORD_LEN)
	 {
	    parse_free(ps);
	    return (PARSE_ERROR);
	 }        
      	 ps->parse_buf[(ps->parse_len)++] = *(ps->bp);
      }
      (ps->bp)++;
   }
}

void		parse_rewind(ps)
parse_state	*ps;
{
   if (LIST_NUMBER_ELT(&(ps->compiled_list)) == 0)
   {
      fprintf(stderr,"warning: nothing to do!\n");
      return ;
   }
   LIST_REWIND(&(ps->compiled_list));
}

/* thanks to prasad@ug.eds.com */
int		parse_next(ps)
parse_state	*ps;
{
   compiled_cmd	*cc;
   
   cc = list_get_current(&(ps->compiled_list));
   if (cc == NULL)
      return (PARSE_EOP);
   cc->func(ps->data,cc->params,cc->num_params);
   list_next(&(ps->compiled_list));
   return (PARSE_OK);
}

void		parse_destroy(ps)
parse_state	*ps;
{
   parse_free(ps);  
}
