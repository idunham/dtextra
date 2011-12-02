/*
 *  GetColor()  -   gets the pixel index from the colormap
 *
 *  RCS:
 *      $Revision$
 *      $Date$
 *
 *  Security:
 *      Unclassified
 *
 *  Description:
 *      text
 *
 *  Input Parameters:
 *      type    identifier  description
 *
 *      text
 *
 *  Output Parameters:
 *      type    identifier  description
 *
 *      text
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
 *  9/23/93     mm      first cut
 *  6/8/94      mm      returns best pixel if colormap is full
 */

#include "xhead.h"
#include "defines.h"
#include "xmastm.h"

unsigned int AllocateNamedColor(display, name)
Display
    *display;

char
    *name;
{
    XColor
        def;
    int
        status;

    int
        screen;

    Colormap
        colormap;

    screen = DefaultScreen(display);
    colormap = DefaultColormap (display, screen);

    if (XParseColor (display, colormap, name, &def ))
    {
        status = XAllocColor (display, colormap, &def);
        if (status == 0)
        {
            /*
            ** colormap is full, so find the closest color
            */

            (void) BestPixel (display, colormap, (XColor *) NULL,
                (unsigned int) Min(DisplayCells(display,screen),256),
                &def);
        }
    }
    else
    {
        Warning("Could not Parse Color:", name);
        return(257);
    }

return(def.pixel);
}
