/*
 *  BestPixel() -   Returns the best pixel 
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      This function returns the closest possible color rquested.
 *      It simply determines the closest point in the RGB space of the
 *      requested R, G, B values.
 *
 *      Adapted from Imagemagick 3.0.0
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      Display         *display;
 *      Colormap        colormap;
 *      XColor          *colors;
 *      unsigned int    number_colors;
 *      XColor          *color;
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      XColor  *color;
 *
 *  Return Values:
 *      value   description
 *
 *  Side Effects:
 *      text
 *
 *  Limitations and Comments:
 *      text
 *
 *  Development History:
 *      when    who     why
 *  6/3/94      mm      ????
 */

#include "xhead.h"
#include "mxkill.h"

void BestPixel(display,colormap,colors,number_colors,color)
Display
  *display;

Colormap
  colormap;

XColor
  *colors;

unsigned int
  number_colors;

XColor
  *color;
{
  int
    ii,
    m_colors;

  register int
    blue_distance,
    green_distance,
    i,
    red_distance;

  unsigned long
    distance,
    min_distance;

  m_colors = colors == (XColor *) NULL;

  if (m_colors)
  {
    colors = (XColor *) malloc(number_colors*sizeof(XColor));
    if (colors == (XColor *) NULL)
    {
        fprintf (stderr, "Failed to read X server colormap.\n");
        goto ExitProcessing;
    }

    for (i=0; i < number_colors; i++)
    {
        colors[i].pixel = (unsigned long) i;
    }

    if (number_colors > 256)
    {
        number_colors = 256;
    }

     XQueryColors(display,colormap,colors,number_colors);
  }

  color->pixel=0;
  min_distance=(unsigned long) (~0);    /* just a big number */
  ii=0;

  for (i=0; i < number_colors; i++)
  {
    red_distance=(int) (colors[i].red >> 8)-(int) (color->red >> 8);
    green_distance=(int) (colors[i].green >> 8)-(int) (color->green >> 8);
    blue_distance=(int) (colors[i].blue >> 8)-(int) (color->blue >> 8);

    /*
    ** distance is distance, the co-ordinates values do not make any
    ** diffetence, i.e., negative co-ordinate value will give the same
    ** distance, because the points are squired.  We do not care about
    ** taking square root of the distance, because, we just need to find
    ** the point closest to the requested point. distance is not important
    */
    
    distance=red_distance*red_distance+green_distance*green_distance+
      blue_distance*blue_distance;

    if (distance < min_distance)
    {
        min_distance=distance;
        color->pixel=colors[i].pixel;
        ii=i;
    }
  }

    /*
    ** this is the closest pixel. So, share it, allocate as Read only
    */

    (void) XAllocColor(display,colormap,&colors[ii]);
    if (m_colors)
    {
        (void) free((char *) colors);
    }

ExitProcessing:
    return;
}
