/*
** bit_tab.c for STD
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Wed Jul 20 12:33:56 1994 hydrix
** Last update Mon Nov 21 14:08:15 1994 hydrix
*/

#include <sysdep.h>
#include <bit_tab.h>

void		bit_tab_create(tab,width,height,size)
bit_type	**tab;
int		width;
int		height;
int		*size;
{
   *size = (BIT_TAB_SIZE(width,height));
   *size = (*size)*2;
   MALLOC(*tab,*size,"bit_tab");
}

void		bit_tab_scroll_left(tab,width,height)
bit_type	*tab;
int		width;
int		height;
{
   int		i;
   int		j;
   
   j = 0;
   while (j < height)
   {
      BIT_UNSET(tab,width-1,j,width);
      j++;
   }
   i = 1;
   while (i < width)
   {
      j = 0;
      while (j < height)
      {
	 if (BIT_TEST(tab,i,j,width))
	    BIT_SET(tab,i-1,j,width);
	 else
	    BIT_UNSET(tab,i-1,j,width);
	 j++;
      }
      i++;
   }
}

void		bit_tab_scroll_right(tab,width,height)
bit_type	*tab;
int		width;
int		height;
{
   int		i;
   int		j;
   
   j = 0;
   while (j < height)
   {
      BIT_UNSET(tab,0,j,width);
      j++;
   }
   i = width-2;
   while (i >= 0)
   {
      j = 0;
      while (j < height)
      {
	 if (BIT_TEST(tab,i,j,width))
	    BIT_SET(tab,i+1,j,width);
	 else
	    BIT_UNSET(tab,i+1,j,width);
	 j++;
      }
      i--;
   }
}

void		bit_tab_scroll_down(tab,width,height)
bit_type	*tab;
int		width;
int		height;
{
   int		i;
   int		j;
   
   i = 0;
   while (i < width)
   {
      BIT_UNSET(tab,i,0,width);
      i++;
   }
   j = height-2;
   while (j >= 0)
   {
      i = 0;
      while (i < width)
      {
	 if (BIT_TEST(tab,i,j,width))
	    BIT_SET(tab,i,j+1,width);
	 else
	    BIT_UNSET(tab,i,j+1,width);
	 i++;
      }
      j--;
   }
}

void		bit_tab_scroll_up(tab,width,height)
bit_type	*tab;
int		width;
int		height;
{
   int		j;
   int		i;
   
   i = 0;
   while (i < width)
   {
      BIT_UNSET(tab,i,height-1,width);
      i++;
   }
   j = 1;
   while (j < height)
   {
      i = 0;
      while (i < width)
      {
	 if (BIT_TEST(tab,i,j,width))
	    BIT_SET(tab,i,j-1,width);
	 else
	    BIT_UNSET(tab,i,j-1,width);
	 i++;
      }
      j++;
   }  
}
