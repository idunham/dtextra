/*
** parse.h for STD
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul 18 13:36:50 1994 hydrix
** Last update Mon Nov 21 13:50:58 1994 hydrix
*/

#ifndef __PARSE_H__
#define __PARSE_H__	1

#include "list.h"

#define MAX_KEYWORD_LEN	1024

#define PARSE_KEYWORD	0
#define PARSE_PARAM	1

#define PARSE_OK	0
#define PARSE_ERROR	1

#define PARSE_EOP	1

typedef struct		_cmd_def
{
   char			*keyword;
   void			(*func)(); 
} cmd_def;

typedef struct		_compiled_cmd
{
   std_list_header	slh;
   void			(*func)();
   char			**params;
   int			num_params;
} compiled_cmd;

typedef struct		_parse_state
{
   std_list		compiled_list;
   char			*buf;
   int			len;
   void			*data;
   char			*bp;				/* buf pointer */
   int			state;
   char			parse_buf[MAX_KEYWORD_LEN];
   int			parse_len;
} parse_state;

extern int		parse_compile();
extern void		parse_rewind();
extern int		parse_next();
extern void		parse_destroy();

#endif /* __PARSE_H__ */
