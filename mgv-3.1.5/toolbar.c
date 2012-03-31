/*
 * toolbar.c
 *	Toolbar extras for mgv.
 *
 * Copyright (C) 1998 Eric A. Howe
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   Authors:	Eric A. Howe (mu@trends.net)
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/toolbar.c 1.2 1998/08/29 00:07:50 $")

#include <X11/Intrinsic.h>

#include <wlib/WlToolBar.h>
#include <mine/toolbar.h>
#include <lib/toolbar/fileOpen-i.xbm>
#include <lib/toolbar/fileOpen-i.xpm>
#include <lib/toolbar/fileOpen.xbm>
#include <lib/toolbar/fileOpen.xpm>
#include <lib/toolbar/filePrint-i.xbm>
#include <lib/toolbar/filePrint-i.xpm>
#include <lib/toolbar/filePrint.xbm>
#include <lib/toolbar/filePrint.xpm>
#include <lib/toolbar/helpContext-i.xbm>
#include <lib/toolbar/helpContext-i.xpm>
#include <lib/toolbar/helpContext.xbm>
#include <lib/toolbar/helpContext.xpm>
#include <lib/toolbar/magstepDecr-i.xbm>
#include <lib/toolbar/magstepDecr-i.xpm>
#include <lib/toolbar/magstepDecr.xbm>
#include <lib/toolbar/magstepDecr.xpm>
#include <lib/toolbar/magstepIncr-i.xbm>
#include <lib/toolbar/magstepIncr-i.xpm>
#include <lib/toolbar/magstepIncr.xbm>
#include <lib/toolbar/magstepIncr.xpm>
#include <lib/toolbar/mark-i.xbm>
#include <lib/toolbar/mark-i.xpm>
#include <lib/toolbar/mark.xbm>
#include <lib/toolbar/mark.xpm>
#include <lib/toolbar/markAll-i.xbm>
#include <lib/toolbar/markAll-i.xpm>
#include <lib/toolbar/markAll.xbm>
#include <lib/toolbar/markAll.xpm>
#include <lib/toolbar/markEven-i.xbm>
#include <lib/toolbar/markEven-i.xpm>
#include <lib/toolbar/markEven.xbm>
#include <lib/toolbar/markEven.xpm>
#include <lib/toolbar/markNone-i.xbm>
#include <lib/toolbar/markNone-i.xpm>
#include <lib/toolbar/markNone.xbm>
#include <lib/toolbar/markNone.xpm>
#include <lib/toolbar/markOdd-i.xbm>
#include <lib/toolbar/markOdd-i.xpm>
#include <lib/toolbar/markOdd.xbm>
#include <lib/toolbar/markOdd.xpm>
#include <lib/toolbar/pageNext-i.xbm>
#include <lib/toolbar/pageNext-i.xpm>
#include <lib/toolbar/pageNext.xbm>
#include <lib/toolbar/pageNext.xpm>
#include <lib/toolbar/pagePrev-i.xbm>
#include <lib/toolbar/pagePrev-i.xpm>
#include <lib/toolbar/pagePrev.xbm>
#include <lib/toolbar/pagePrev.xpm>

static WlToolBarImage internal[] = {
	{
		"fileOpen",
		fileOpen_xpm, fileOpen_i_xpm,
		fileOpen_width, fileOpen_height,
		fileOpen_bits, fileOpen_i_bits
	}, {
		"filePrint",
		filePrint_xpm, filePrint_i_xpm,
		filePrint_width, filePrint_height,
		filePrint_bits, filePrint_i_bits
	}, {
		"helpContext",
		helpContext_xpm, helpContext_i_xpm,
		helpContext_width, helpContext_height,
		helpContext_bits, helpContext_i_bits
	}, {
		"magstepDecr",
		magstepDecr_xpm, magstepDecr_i_xpm,
		magstepDecr_width, magstepDecr_height,
		magstepDecr_bits, magstepDecr_i_bits
	}, {
		"markEven",
		markEven_xpm, markEven_i_xpm,
		markEven_width, markEven_height,
		markEven_bits, markEven_i_bits
	}, {
		"magstepIncr",
		magstepIncr_xpm, magstepIncr_i_xpm,
		magstepIncr_width, magstepIncr_height,
		magstepIncr_bits, magstepIncr_i_bits
	}, {
		"markAll",
		markAll_xpm, markAll_i_xpm,
		markAll_width, markAll_height,
		markAll_bits, markAll_i_bits
	}, {
		"markNone",
		markNone_xpm, markNone_i_xpm,
		markNone_width, markNone_height,
		markNone_bits, markNone_i_bits
	}, {
		"mark",
		mark_xpm, mark_i_xpm,
		mark_width, mark_height,
		mark_bits, mark_i_bits
	}, {
		"markOdd",
		markOdd_xpm, markOdd_i_xpm,
		markOdd_width, markOdd_height,
		markOdd_bits, markOdd_i_bits
	}, {
		"pageNext",
		pageNext_xpm, pageNext_i_xpm,
		pageNext_width, pageNext_height,
		pageNext_bits, pageNext_i_bits
	}, {
		"pagePrev",
		pagePrev_xpm, pagePrev_i_xpm,
		pagePrev_width, pagePrev_height,
		pagePrev_bits, pagePrev_i_bits
	}
};
#define	N_INTERNAL	(sizeof(internal)/sizeof(internal[0]))

void
mgv_toolbar_init(Widget w)
{
	WlToolBarSetInternalImages(w, internal, N_INTERNAL);
}
