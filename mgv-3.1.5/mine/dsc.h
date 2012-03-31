/*
 * dsc.h -- DSC comment parser
 * Copyright (C) 1992  Timothy O. Theisen
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
 *   Author: Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 *
 * This used to be ps.h from ghostview but I'm in the middle of
 * re-writing it.
 *	--mu@trends.net
 */
#ifndef	MGV_DSC_H
#define	MGV_DSC_H
MU_HID(dsc_h, "$Mu: mgv/mine/dsc.h 1.12 1998/08/27 03:40:03 $")

/*
 * Constants used to index into the bounding box array.
 */
#define	DSC_LLX	0
#define	DSC_LLY	1
#define	DSC_URX	2
#define	DSC_URY	3

/*
 * Constants used to store keywords that are scanned.
 * DSC_NONE is not a keyword, it tells when a field was not set
 */
#define	DSC_ATEND	-1
#define	DSC_NONE	 0
#define	DSC_PORTRAIT	 1
#define	DSC_LANDSCAPE	 2
#define	DSC_ASCEND	 3
#define	DSC_DESCEND	 4
#define	DSC_SPECIAL	 5

/*
 * document types
 */
#define	DSC_PS		0
#define	DSC_EPS		1
#define	DSC_PDF		2

/*
 * media description
 */
typedef struct {
	char	*name;
	int	width, height;	/* points */
} DSCM;

/*
 * a section
 * (%%BeginSetup/%%EndSetup, etc...)
 */
typedef struct {
	off_t	begin, end;
	size_t	len;
} DSCS;

/*
 * one page
 */
typedef struct {
	int 	mark;
	char	*label;
	int	bbox[4];
	DSCM	*media;
	int	orientation;		/* PORTRAIT, LANDSCAPE */
	DSCS	sect;
} DSCP;

typedef struct {
	int	type;		/* EPS/PS/PDF			*/
	char	*title;		/* Title of document.		*/
	char	*date;		/* Creation date.		*/
	char	*forwho;	/* %%For			*/
	char	*creator;	/* %%Creator			*/
	int	pageorder;	/* ASCEND, DESCEND, SPECIAL	*/

	int	bbox[4];
	int	defbbox[4];
	int	orientation;	/* PORTRAIT, LANDSCAPE */
	int	deforient;	/* PORTRAIT, LANDSCAPE */
	DSCM	*media;
	int	n_media;
	DSCM	*defmedia;

	DSCS	head;
	DSCS	preview;
	DSCS	defaults;
	DSCS	prolog;
	DSCS	setup;
	DSCP	*pages;
	int	n_pages;
	DSCS	trailer;
} DSC;

/*
 * list of standard paper sizes from Adobe's PPD.
 */
extern DSCM dsc_media[];

CDECLS_BEGIN
/*
 * scans a PostScript file and returns a pointer to the document
 * structure.  Returns NULL if file does not Conform to commenting
 * conventions or memory runs out.
 */
extern	DSC	*dsc_scan(FILE *);

/*
 * free data structure malloc'ed by psscan
 */
extern	DSC	*dsc_free(DSC *);

/*
 * Copy a portion of the PostScript file
 */
extern	void	dsc_cp(FILE *, FILE *, off_t, off_t);

/*
 * Copy a portion of the PostScript file upto a comment
 */
extern	char	*dsc_cpuntil(FILE *, FILE *, off_t, off_t, char *);
CDECLS_END

#endif
