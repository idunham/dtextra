/**
 *
 * $Id: DisplayFallbackResources.c,v 1.5 2001/06/09 18:38:57 amai Exp $
 *
 * Copyright (C) 1996 Free Software Foundation, Inc.
 * Copyright © 1999-2001 by the LessTif developers.
 *
 * This file is part of the GNU LessTif Extension Library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 **/

#include "../config.h"

#include <stdio.h>

#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static const char rcsid[] = "$Id: DisplayFallbackResources.c,v 1.5 2001/06/09 18:38:57 amai Exp $";

void 
XltDisplayFallbackResources(char **Fallback)
{
    while (*Fallback != NULL)
    {
	fprintf(stderr, "%s\n", *Fallback);
	Fallback++;
    }
}
