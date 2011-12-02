/**
 *
 * $Id: DisplayOptions.c,v 1.6 2001/06/09 18:38:57 amai Exp $
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

#include <X11/Intrinsic.h>

#include <Xlt.h>

#ifdef WITH_DMALLOC
#include <dmalloc.h>
#endif

static const char rcsid[] = "$Id: DisplayOptions.c,v 1.6 2001/06/09 18:38:57 amai Exp $";

void 
XltDisplayOptions(XrmOptionDescRec *opTable, Cardinal num_options)
{
	Cardinal i;

	fprintf(stderr, "The command line options available are as follows:\n");
	for (i = 0; i < num_options; i++)
	{
		fprintf(stderr, "\t%s", opTable[i].option);
		switch (opTable[i].argKind)
		{
		case XrmoptionSepArg:
			fprintf(stderr, " arg");
			break;
		default:
			break;
		}
		fprintf(stderr, "\n");
	}
}
