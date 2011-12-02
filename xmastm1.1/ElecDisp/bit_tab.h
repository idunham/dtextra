/*
** bit_tab.h for STD
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul 18 18:49:43 1994 hydrix
** Last update Thu Oct 27 15:59:01 1994 hydrix
*/

#ifndef __BIT_TAB_H__
#define __BIT_TAB_H__	1

typedef char			bit_type;

#define BIT_TYPE_SIZE		(sizeof (bit_type)*8)

#define BIT_OFFSET(x,y,w)	(((y)*(((w)-1)/(BIT_TYPE_SIZE-1)))+((x)/BIT_TYPE_SIZE))
#define BIT_MASK(x)		(1 <<  ( (x) % BIT_TYPE_SIZE ) )

#define BIT_VALUE(t,x,y,w)	((t)[BIT_OFFSET((x),(y),(w))])

#define BIT_TEST(t,x,y,w)	(BIT_VALUE((t),(x),(y),(w))&BIT_MASK(x))

#define BIT_SET(t,x,y,w)	((t)[BIT_OFFSET((x),(y),(w))] |= BIT_MASK(x))
#define BIT_UNSET(t,x,y,w)	((t)[BIT_OFFSET((x),(y),(w))] &= (~BIT_MASK(x)))

#define BIT_TAB_SIZE(width,height)	(((width)*(height))/BIT_TYPE_SIZE)

#endif /* __BIT_TAB_H__ */
