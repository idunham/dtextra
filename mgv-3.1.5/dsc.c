/*
 * dsc.c -- Postscript scanning and copying routines.
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
 */
#include <wlib/rcs.h>
MU_ID("$Mu: mgv/dsc.c 1.22 1998/09/01 20:49:37 $")

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>

#include <wlib/stdlib.h>
#include <mine/dsc.h>
#include <mine/util.h>

#ifndef BUFSIZ
#	define BUFSIZ 1024
#endif

/*
 * 255 characters (max line length for PS) plus a newline and
 * terminating NUL
 */
#define	LINELEN	257

/*
 * length calculates string length at compile time
 * can only be used with character constants
 */
#define	length(a)	(sizeof(a)-1)
#define	iscomment(a, b)	(strncmp(a, b, length(b)) == 0)
#define	DSCcomment(a)	(a[0] == '%' && a[1] == '%')

/*
 * list of standard paper sizes from Adobe's PPD.
 */
DSCM dsc_media[] = {
	{"letter",	 612,  792},
	{"lettersmall",	 612,  792},
	{"tabloid",	 792, 1224},
	{"ledger",	1224,  792},
	{"legal",	 612, 1008},
	{"statement",	 396,  612},
	{"executive",	 540,  720},
	{"a3",		 842, 1190},
	{"a4",		 595,  842},
	{"a4small",	 595,  842},
	{"a5",		 420,  595},
	{"b4",		 729, 1032},
	{"b5",		 516,  729},
	{"folio",	 612,  936},
	{"quarto",	 610,  780},
	{"10x14",	 720, 1008},
	{NULL,		   0,    0}
};

/*
 * determine whether the line contains nothing but whitespace.
 */
static int
blank(char *line)
{
	char	*cp = line;

	while(*cp == ' ' || *cp == '\t')
		++cp;
	return *cp == '\n' || (*cp == '%' && (line[0] != '%' || line[1] != '%'));
}

/*
 *	readline -- Read the next line in the postscript file.
 *                  Automatically skip over data (as indicated by
 *                  %%BeginBinary/%%EndBinary or %%BeginData/%%EndData
 *		    comments.)
 *		    Also, skip over included documents (as indicated by
 *		    %%BeginDocument/%%EndDocument comments.)
 */

static char *
readline(char *line, int size, FILE *fp,long *position, unsigned *line_len)
{
    char text[LINELEN];	/* Temporary storage for text */
    char save[LINELEN];	/* Temporary storage for text */
    char *cp;
    unsigned int num;
    unsigned int nbytes;
    int i;
    char buf[BUFSIZ];

    if (position) *position = ftell(fp);
    cp = fgets(line, size, fp);
    if (cp == NULL) line[0] = '\0';
    *line_len = strlen(line);
    if (!(DSCcomment(line) && iscomment(line+2, "Begin"))) {
	/* Do nothing */
    } else if (iscomment(line+7, "Document:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndDocument"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "Feature:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndFeature"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "File:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndFile"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "Font:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndFont"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "ProcSet:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndProcSet"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "Resource:")) {
	strcpy(save, line+7);
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndResource"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "Data:")) {
	text[0] = '\0';
	strcpy(save, line+7);
	if (sscanf(line+length("%%BeginData:"), "%d %*s %s", &num, text) >= 1) {
	    if (strcmp(text, "Lines") == 0) {
		for (i=0; i < (int)num; i++) {
		    cp = fgets(line, size, fp);
		    *line_len += cp ? strlen(line) : 0;
		}
	    } else {
		while (num > BUFSIZ) {
		    fread(buf, sizeof (char), BUFSIZ, fp);
		    *line_len += BUFSIZ;
		    num -= BUFSIZ;
		}
		fread(buf, sizeof (char), num, fp);
		*line_len += num;
	    }
	}
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndData"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    } else if (iscomment(line+7, "Binary:")) {
	strcpy(save, line+7);
	if(sscanf(line+length("%%BeginBinary:"), "%d", &num) == 1) {
	    while (num > BUFSIZ) {
		fread(buf, sizeof (char), BUFSIZ, fp);
		*line_len += BUFSIZ;
		num -= BUFSIZ;
	    }
	    fread(buf, sizeof (char), num, fp);
	    *line_len += num;
	}
	while (readline(line, size, fp, NULL, &nbytes) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndBinary"))) {
	    *line_len += nbytes;
	}
	*line_len += nbytes;
	strcpy(line, save);
    }
    return cp;
}


/*
 *	gettext -- return the next text string on the line.
 *		   return NULL if nothing is present.
 */

static char *
gettext(char *line, char **next_char)
{
    char text[LINELEN];	/* Temporary storage for text */
    char *cp;
    int quoted=0;

    while (*line && (*line == ' ' || *line == '\t')) line++;
    cp = text;
    if (*line == '(') {
	int level = 0;
	quoted=1;
	line++;
	while (*line && !(*line == ')' && level == 0 )) {
	    if (*line == '\\') {
		if (*(line+1) == 'n') {
		    *cp++ = '\n';
		    line += 2;
		} else if (*(line+1) == 'r') {
		    *cp++ = '\r';
		    line += 2;
		} else if (*(line+1) == 't') {
		    *cp++ = '\t';
		    line += 2;
		} else if (*(line+1) == 'b') {
		    *cp++ = '\b';
		    line += 2;
		} else if (*(line+1) == 'f') {
		    *cp++ = '\f';
		    line += 2;
		} else if (*(line+1) == '\\') {
		    *cp++ = '\\';
		    line += 2;
		} else if (*(line+1) == '(') {
		    *cp++ = '(';
		    line += 2;
		} else if (*(line+1) == ')') {
		    *cp++ = ')';
		    line += 2;
		} else if (*(line+1) >= '0' && *(line+1) <= '9') {
		    if (*(line+2) >= '0' && *(line+2) <= '9') {
			if (*(line+3) >= '0' && *(line+3) <= '9') {
			    *cp++ = ((*(line+1) - '0')*8 + *(line+2) - '0')*8 +
				    *(line+3) - '0';
			    line += 4;
			} else {
			    *cp++ = (*(line+1) - '0')*8 + *(line+2) - '0';
			    line += 3;
			}
		    } else {
			*cp++ = *(line+1) - '0';
			line += 2;
		    }
		} else {
		    line++;
		    *cp++ = *line++;
		}
	    } else if (*line == '(') {
		level++;
		*cp++ = *line++;
	    } else if (*line == ')') {
		level--;
		*cp++ = *line++;
	    } else {
		*cp++ = *line++;
	    }
	}
    } else {
	while (*line && !(*line == ' ' || *line == '\t' || *line == '\n'))
	    *cp++ = *line++;
    }
    *cp = '\0';
    if (next_char) *next_char = line;
    if (!quoted && strlen(text) == 0) return NULL;
    if((cp = malloc(strlen(text)+1)) == NULL) {
	fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
	exit(EXIT_FAILURE);
    }
    strcpy(cp, text);
    return cp;
}

/*
 * skip over white space and return the rest of the line.
 * If the text begins with '(' return the text string
 * using gettext().
 */
static char *
gettextline(char *line)
{
	char	*cp;
	int	len;

	while(*line != '\0' && (*line == ' ' || *line == '\t'))
		++line;

	if(*line == '(')
		return gettext(line, NULL);
	if(*line == '\0')
		return NULL;
	len = strlen(line);
	if((cp = malloc(len)) == NULL) {
		fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		exit(EXIT_FAILURE);
	}
	strncpy(cp, line, len - 1);
	cp[len - 1] = '\0';
	return cp;
}

/*
 *	dsc_scan -- scan the PostScript file for document structuring comments.
 *
 *	This scanner is designed to retrieve the information necessary for
 *	the ghostview previewer.  It will scan files that conform to any
 *	version (1.0, 2.0, 2.1, or 3.0) of the document structuring conventions.
 *	It does not really care which version of comments the file contains.
 *	(The comments are largely upward compatible.)  It will scan a number
 *	of non-conforming documents.  (You could have part of the document
 *	conform to V2.0 and the rest conform to V3.0.  It would be similar
 *	to the DC-2 1/2+, it would look funny but it can still fly.)
 *
 *	This routine returns a pointer to the document structure.
 *	The structure contains the information relevant to previewing.
 *      These include EPSF flag (to tell if the file is a encapsulated figure),
 *      Page Media (for the Page Size), Bounding Box (to minimize backing
 *      pixmap size or determine window size for encapsulated PostScript), 
 *      Orientation of Paper (for default transformation matrix), and
 *      Page Order.  The title and CreationDate are also retrieved to
 *      help identify the document.
 *
 *      The following comments are examined:
 *
 *      Header section: 
 *      Must start with %!PS-Adobe-.  Version numbers ignored.
 *
 *      %!PS-Adobe-* [EPSF-*]
 *      %%BoundingBox: <int> <int> <int> <int>|(atend)
 *      %%CreationDate: <textline>
 *      %%Orientation: Portrait|Landscape|(atend)
 *      %%Pages: <uint> [<int>]|(atend)
 *      %%PageOrder: Ascend|Descend|Special|(atend)
 *      %%Title: <textline>
 *      %%DocumentMedia: <text> <real> <real> <real> <text> <text>
 *      %%DocumentPaperSizes: <text>
 *	%%For: <text>
 *	%%Creator: <text>
 *      %%EndComments
 *
 *      Note: Either the 3.0 or 2.0 syntax for %%Pages is accepted.
 *            Also either the 2.0 %%DocumentPaperSizes or the 3.0
 *            %%DocumentMedia comments are accepted as well.
 *
 *      The header section ends either explicitly with %%EndComments or
 *      implicitly with any line that does not begin with %X where X is
 *      a not whitespace character.
 *
 *      If the file is encapsulated PostScript the optional Preview section
 *      is next:
 *
 *      %%BeginPreview
 *      %%EndPreview
 *
 *      This section explicitly begins and ends with the above comments.
 *
 *      Next the Defaults section for version 3 page defaults:
 *
 *      %%BeginDefaults
 *      %%PageBoundingBox: <int> <int> <int> <int>
 *      %%PageOrientation: Portrait|Landscape
 *      %%PageMedia: <text>
 *      %%EndDefaults
 *
 *      This section explicitly begins and ends with the above comments.
 *
 *      The prolog section either explicitly starts with %%BeginProlog or
 *      implicitly with any nonblank line.
 *
 *      %%BeginProlog
 *      %%EndProlog
 *
 *      The Prolog should end with %%EndProlog, however the proglog implicitly
 *      ends when %%BeginSetup, %%Page, %%Trailer or %%EOF are encountered.
 *
 *      The Setup section is where the version 2 page defaults are found.
 *      This section either explicitly begins with %%BeginSetup or implicitly
 *      with any nonblank line after the Prolog.
 *
 *      %%BeginSetup
 *      %%PageBoundingBox: <int> <int> <int> <int>
 *      %%PageOrientation: Portrait|Landscape
 *      %%PaperSize: <text>
 *      %%EndSetup
 *
 *      The Setup should end with %%EndSetup, however the setup implicitly
 *      ends when %%Page, %%Trailer or %%EOF are encountered.
 *
 *      Next each page starts explicitly with %%Page and ends implicitly with
 *      %%Page or %%Trailer or %%EOF.  The following comments are recognized:
 *
 *      %%Page: <text> <uint>
 *      %%PageBoundingBox: <int> <int> <int> <int>|(atend)
 *      %%PageOrientation: Portrait|Landscape
 *      %%PageMedia: <text>
 *      %%PaperSize: <text>
 *
 *      The tralier section start explicitly with %%Trailer and end with %%EOF.
 *      The following comment are examined with the proper (atend) notation
 *      was used in the header:
 *
 *      %%Trailer
 *      %%BoundingBox: <int> <int> <int> <int>|(atend)
 *      %%Orientation: Portrait|Landscape|(atend)
 *      %%Pages: <uint> [<int>]|(atend)
 *      %%PageOrder: Ascend|Descend|Special|(atend)
 *      %%EOF
 *
 *
 *  + A DC-3 received severe damage to one of its wings.  The wing was a total
 *    loss.  There was no replacement readily available, so the mechanic
 *    installed a wing from a DC-2.
 */

DSC *
dsc_scan(FILE *file)
{
	DSC		*dsc;
	int		bb_set = DSC_NONE;
	int		pages_set = DSC_NONE;
	int		page_order_set = DSC_NONE;
	int		orientation_set = DSC_NONE;
	int		page_bb_set = DSC_NONE;
	int		page_media_set = DSC_NONE;
	int		preread;	/* readline needed? */
	int		i;
	unsigned	maxpages = 0;
	unsigned	nextpage = 1;	/* next expected page */
	unsigned	thispage;
	int		ignore = 0;	/* ignore page ordinals? */
	char		*label;
	char		line[LINELEN];
	char		text[LINELEN];
	long		position;	/* position of current line */
	long		beginsection;	/* start of current section */
	unsigned	line_len; 	/* length of the current line */
	unsigned	section_len;	/* current section length */
	char		*next_char;
	char		*cp;
	DSCM		*dmp;

	memset((void *)&line[0], '\0', sizeof(line));
	memset((void *)&text[0], '\0', sizeof(text));
	rewind(file);
	if(readline(line, sizeof line, file, &position, &line_len) == NULL) {
		fprintf(stderr, "Warning: empty file.\n");
		return(NULL);
	}

	/*
	 * Header comments
	 *
	 * We can get away with only looking for "%!" here.
	 * We should be looking for the DSC standard "%!PS-Adobe-"
	 * but some programs can't write proper PostScript (even
	 * FrameMaker is guilty of this sometimes).  If the document
	 * isn't DSC compliant, then the DSC* we're building will have
	 * "n_pages" equal to zero and we'll return NULL at the end.
	 */
	if(!iscomment(line, "%!")
	|| (dsc = calloc(1, sizeof(DSC))) == NULL)
		return NULL;
	dsc->type = DSC_PS;

	sscanf(line, "%*s %s", text);
	if(iscomment(text, "EPSF-"))
		dsc->type = DSC_EPS;
	dsc->head.begin = position;
	section_len = line_len;

    preread = 0;
    while (preread || readline(line, sizeof line, file, &position, &line_len)) {
	if (!preread) section_len += line_len;
	preread = 0;
	if (line[0] != '%' ||
	    iscomment(line+1, "%EndComments") ||
	    line[1] == ' ' || line[1] == '\t' || line[1] == '\n' ||
	    !isprint((int)line[1] & 0xff)) {
	    break;
	} else if (line[1] != '%') {
	    /* Do nothing */
	} else if (dsc->forwho == NULL && iscomment(line+2, "For:")) {
	    dsc->forwho = gettextline(line+length("%%For:"));
	} else if (dsc->creator == NULL && iscomment(line+2, "Creator:")) {
	    dsc->creator = gettextline(line+length("%%Creator:"));
	} else if (dsc->title == NULL && iscomment(line+2, "Title:")) {
	    dsc->title = gettextline(line+length("%%Title:"));
	} else if (dsc->date == NULL && iscomment(line+2, "CreationDate:")) {
	    dsc->date = gettextline(line+length("%%CreationDate:"));
	} else if (bb_set == DSC_NONE && iscomment(line+2, "BoundingBox:")) {
	    sscanf(line+length("%%BoundingBox:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		bb_set = DSC_ATEND;
	    } else {
		if (sscanf(line+length("%%BoundingBox:"), "%d %d %d %d",
			   &(dsc->bbox[DSC_LLX]),
			   &(dsc->bbox[DSC_LLY]),
			   &(dsc->bbox[DSC_URX]),
			   &(dsc->bbox[DSC_URY])) == 4)
		    bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%BoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			bb_set = 1;
			dsc->bbox[DSC_LLX] = fllx;
			dsc->bbox[DSC_LLY] = flly;
			dsc->bbox[DSC_URX] = furx;
			dsc->bbox[DSC_URY] = fury;
			if (fllx < dsc->bbox[DSC_LLX])
			    dsc->bbox[DSC_LLX]--;
			if (flly < dsc->bbox[DSC_LLY])
			    dsc->bbox[DSC_LLY]--;
			if (furx > dsc->bbox[DSC_URX])
			    dsc->bbox[DSC_URX]++;
			if (fury > dsc->bbox[DSC_URY])
			    dsc->bbox[DSC_URY]++;
		    }
		}
	    }
	} else if (orientation_set == DSC_NONE &&
		   iscomment(line+2, "Orientation:")) {
	    sscanf(line+length("%%Orientation:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		orientation_set = DSC_ATEND;
	    } else if (strcmp(text, "Portrait") == 0) {
		dsc->orientation = DSC_PORTRAIT;
		orientation_set = 1;
	    } else if (strcmp(text, "Landscape") == 0) {
		dsc->orientation = DSC_LANDSCAPE;
		orientation_set = 1;
	    }
	} else if (page_order_set == DSC_NONE && iscomment(line+2, "PageOrder:")) {
	    sscanf(line+length("%%PageOrder:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		page_order_set = DSC_ATEND;
	    } else if (strcmp(text, "Ascend") == 0) {
		dsc->pageorder = DSC_ASCEND;
		page_order_set = 1;
	    } else if (strcmp(text, "Descend") == 0) {
		dsc->pageorder = DSC_DESCEND;
		page_order_set = 1;
	    } else if (strcmp(text, "Special") == 0) {
		dsc->pageorder = DSC_SPECIAL;
		page_order_set = 1;
	    }
	} else if (pages_set == DSC_NONE && iscomment(line+2, "Pages:")) {
	    sscanf(line+length("%%Pages:"), "%s", text);
	    if (strcmp(text, "(atend)") == 0) {
		pages_set = DSC_ATEND;
	    } else {
		switch (sscanf(line+length("%%Pages:"), "%d %d",
			       &maxpages, &i)) {
		    case 2:
			if (page_order_set == DSC_NONE) {
			    if (i == -1) {
				dsc->pageorder = DSC_DESCEND;
				page_order_set = 1;
			    } else if (i == 0) {
				dsc->pageorder = DSC_SPECIAL;
				page_order_set = 1;
			    } else if (i == 1) {
				dsc->pageorder = DSC_ASCEND;
				page_order_set = 1;
			    }
			}
		    case 1:
			if (maxpages > 0) {
			    dsc->pages = calloc(maxpages, sizeof(DSCP));
			    if (dsc->pages == NULL) {
				fprintf(stderr,
				    "Fatal Error: Dynamic memory exhausted.\n");
				exit(EXIT_FAILURE);
			    }
			}
		}
	    }
	} else if (dsc->n_media == DSC_NONE &&
		   iscomment(line+2, "DocumentMedia:")) {
	    float w, h;
	    dsc->media = malloc(sizeof (DSCM));
	    if (dsc->media == NULL) {
		fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		exit(EXIT_FAILURE);
	    }
	    dsc->media[0].name = gettext(line+length("%%DocumentMedia:"),
					 &next_char);
	    stolower(dsc->media[0].name);
	    if (dsc->media[0].name != NULL) {
		if (sscanf(next_char, "%f %f", &w, &h) == 2) {
		    dsc->media[0].width = w + 0.5;
		    dsc->media[0].height = h + 0.5;
		}
		if (dsc->media[0].width != 0 && dsc->media[0].height != 0)
		    dsc->n_media = 1;
		else
		    free(dsc->media[0].name);
	    }
	    preread=1;
	    while (readline(line, sizeof line, file, &position, &line_len) &&
		   DSCcomment(line) && iscomment(line+2, "+")) {
		section_len += line_len;
		dsc->media = realloc(dsc->media, (dsc->n_media+1)* sizeof (DSCM));
		if (dsc->media == NULL) {
		    fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		    exit(EXIT_FAILURE);
		}
		dsc->media[dsc->n_media].name = gettext(line+length("%%+"),
							 &next_char);
		if (dsc->media[dsc->n_media].name != NULL) {
		    if (sscanf(next_char, "%f %f", &w, &h) == 2) {
			dsc->media[dsc->n_media].width = w + 0.5;
			dsc->media[dsc->n_media].height = h + 0.5;
		    }
		    if (dsc->media[dsc->n_media].width != 0 &&
			dsc->media[dsc->n_media].height != 0) dsc->n_media++;
		    else
			free(dsc->media[dsc->n_media].name);
		}
	    }
	    section_len += line_len;
	    if (dsc->n_media != 0) dsc->defmedia = dsc->media;
	} else if (dsc->n_media == DSC_NONE &&
		   iscomment(line+2, "DocumentPaperSizes:")) {
	    dsc->media = malloc(sizeof (DSCM));
	    if (dsc->media == NULL) {
		fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		exit(EXIT_FAILURE);
	    }
	    dsc->media[0].name = gettext(line+length("%%DocumentPaperSizes:"),
					 &next_char);
	    stolower(dsc->media[0].name);
	    if (dsc->media[0].name != NULL) {
		dsc->media[0].width = 0;
		dsc->media[0].height = 0;
		for (dmp=dsc_media; dmp->name != NULL; dmp++) {
		    if (strcmp(dsc->media[0].name, dmp->name) == 0) {
			free(dsc->media[0].name);
			dsc->media[0].name = malloc(strlen(dmp->name)+1);
			if (dsc->media[0].name == NULL) {
			    fprintf(stderr,
				    "Fatal Error: Dynamic memory exhausted.\n");
			    exit(EXIT_FAILURE);
			}
			strcpy(dsc->media[0].name, dmp->name);
			dsc->media[0].width = dmp->width;
			dsc->media[0].height = dmp->height;
			break;
		    }
		}
		if (dsc->media[0].width != 0 && dsc->media[0].height != 0)
		    dsc->n_media = 1;
		else
		    free(dsc->media[0].name);
	    }
	    while ((cp = gettext(next_char, &next_char)) != NULL) {
		dsc->media = realloc(dsc->media, (dsc->n_media+1)* sizeof (DSCM));
		if (dsc->media == NULL) {
		    fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		    exit(EXIT_FAILURE);
		}
		dsc->media[dsc->n_media].name = stolower(cp);
		dsc->media[dsc->n_media].width = 0;
		dsc->media[dsc->n_media].height = 0;
		for (dmp=dsc_media; dmp->name != NULL; dmp++) {
		    if (strcmp(dsc->media[dsc->n_media].name, dmp->name) == 0){
			free(dsc->media[dsc->n_media].name);
			dsc->media[dsc->n_media].name =
				malloc(strlen(dmp->name)+1);
			if (dsc->media[dsc->n_media].name == NULL) {
			    fprintf(stderr,
				    "Fatal Error: Dynamic memory exhausted.\n");
			    exit(EXIT_FAILURE);
			}
			strcpy(dsc->media[dsc->n_media].name, dmp->name);
			dsc->media[dsc->n_media].name = dmp->name;
			dsc->media[dsc->n_media].width = dmp->width;
			dsc->media[dsc->n_media].height = dmp->height;
			break;
		    }
		}
		if (dsc->media[dsc->n_media].width != 0 &&
		    dsc->media[dsc->n_media].height != 0) dsc->n_media++;
		else
		    free(dsc->media[dsc->n_media].name);
	    }
	    preread=1;
	    while (readline(line, sizeof line, file, &position, &line_len) &&
		   DSCcomment(line) && iscomment(line+2, "+")) {
		section_len += line_len;
		next_char = line + length("%%+");
		while ((cp = gettext(next_char, &next_char)) != NULL) {
		    dsc->media = realloc(dsc->media,
					 (dsc->n_media+1)*
					 sizeof (DSCM));
		    if (dsc->media == NULL) {
			fprintf(stderr,
				"Fatal Error: Dynamic memory exhausted.\n");
			exit(EXIT_FAILURE);
		    }
		    dsc->media[dsc->n_media].name = stolower(cp);
		    dsc->media[dsc->n_media].width = 0;
		    dsc->media[dsc->n_media].height = 0;
		    for (dmp=dsc_media; dmp->name != NULL; dmp++) {
			if (strcmp(dsc->media[dsc->n_media].name,
				   dmp->name) == 0) {
			    dsc->media[dsc->n_media].width = dmp->width;
			    dsc->media[dsc->n_media].height = dmp->height;
			    break;
			}
		    }
		    if (dsc->media[dsc->n_media].width != 0 &&
			dsc->media[dsc->n_media].height != 0) dsc->n_media++;
		    else
			free(dsc->media[dsc->n_media].name);
		}
	    }
	    section_len += line_len;
	    if (dsc->n_media != 0) dsc->defmedia = dsc->media;
	}
    }

    if (DSCcomment(line) && iscomment(line+2, "EndComments")) {
	readline(line, sizeof line, file, &position, &line_len);
	section_len += line_len;
    }
    dsc->head.end = position;
    dsc->head.len = section_len - line_len;

    /* Optional Preview comments for encapsulated PostScript files */ 

    beginsection = position;
    section_len = line_len;
    while (blank(line) &&
	   readline(line, sizeof line, file, &position, &line_len)) {
	section_len += line_len;
    }

    if (dsc->type == DSC_EPS
    && DSCcomment(line)
    && iscomment(line+2, "BeginPreview")) {
	dsc->preview.begin = beginsection;
	beginsection = 0;
	while (readline(line, sizeof line, file, &position, &line_len) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndPreview"))) {
	    section_len += line_len;
	}
	section_len += line_len;
	readline(line, sizeof line, file, &position, &line_len);
	section_len += line_len;
	dsc->preview.end = position;
	dsc->preview.len = section_len - line_len;
    }

    /* Page Defaults for Version 3.0 files */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) &&
	   readline(line, sizeof line, file, &position, &line_len)) {
	section_len += line_len;
    }

    if (DSCcomment(line) && iscomment(line+2, "BeginDefaults")) {
	dsc->defaults.begin = beginsection;
	beginsection = 0;
	while (readline(line, sizeof line, file, &position, &line_len) &&
	       !(DSCcomment(line) && iscomment(line+2, "EndDefaults"))) {
	    section_len += line_len;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (dsc->deforient == DSC_NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    dsc->deforient = DSC_PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    dsc->deforient = DSC_LANDSCAPE;
		}
	    } else if (page_media_set == DSC_NONE &&
		       iscomment(line+2, "PageMedia:")) {
		cp = stolower(gettext(line+length("%%PageMedia:"), NULL));
		for (dmp = dsc->media, i=0; i<dsc->n_media; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			dsc->defmedia = dmp;
			page_media_set = 1;
			break;
		    }
		}
		free(cp);
	    } else if (page_bb_set == DSC_NONE &&
		       iscomment(line+2, "PageBoundingBox:")) {
		if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			   &(dsc->defbbox[DSC_LLX]),
			   &(dsc->defbbox[DSC_LLY]),
			   &(dsc->defbbox[DSC_URX]),
			   &(dsc->defbbox[DSC_URY])) == 4)
		    page_bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%PageBoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			page_bb_set = 1;
			dsc->defbbox[DSC_LLX] = fllx;
			dsc->defbbox[DSC_LLY] = flly;
			dsc->defbbox[DSC_URX] = furx;
			dsc->defbbox[DSC_URY] = fury;
			if (fllx < dsc->defbbox[DSC_LLX])
			    dsc->defbbox[DSC_LLX]--;
			if (flly < dsc->defbbox[DSC_LLY])
			    dsc->defbbox[DSC_LLY]--;
			if (furx > dsc->defbbox[DSC_URX])
			    dsc->defbbox[DSC_URX]++;
			if (fury > dsc->defbbox[DSC_URY])
			    dsc->defbbox[DSC_URY]++;
		    }
		}
	    }
	}
	section_len += line_len;
	readline(line, sizeof line, file, &position, &line_len);
	section_len += line_len;
	dsc->defaults.end = position;
	dsc->defaults.len = section_len - line_len;
    }

    /* Document Prolog */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) &&
	   readline(line, sizeof line, file, &position, &line_len)) {
	section_len += line_len;
    }

    if (!(DSCcomment(line) &&
	  (iscomment(line+2, "BeginSetup") ||
	   iscomment(line+2, "Page:") ||
	   iscomment(line+2, "Trailer") ||
	   iscomment(line+2, "EOF")))) {
	dsc->prolog.begin = beginsection;
	beginsection = 0;
	preread = 1;

	while ((preread ||
		readline(line, sizeof line, file, &position, &line_len)) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "EndProlog") ||
	          iscomment(line+2, "BeginSetup") ||
	          iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          iscomment(line+2, "EOF")))) {
	    if (!preread) section_len += line_len;
	    preread = 0;
	}
	section_len += line_len;
	if (DSCcomment(line) && iscomment(line+2, "EndProlog")) {
	    readline(line, sizeof line, file, &position, &line_len);
	    section_len += line_len;
	}
	dsc->prolog.end = position;
	dsc->prolog.len = section_len - line_len;
    }

    /* Document Setup,  Page Defaults found here for Version 2 files */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) &&
	   readline(line, sizeof line, file, &position, &line_len)) {
	section_len += line_len;
    }

    if (!(DSCcomment(line) &&
	  (iscomment(line+2, "Page:") ||
	   iscomment(line+2, "Trailer") ||
	   iscomment(line+2, "EOF")))) {
	dsc->setup.begin = beginsection;
	beginsection = 0;
	preread = 1;
	while ((preread ||
		readline(line, sizeof line, file, &position, &line_len)) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "EndSetup") ||
	          iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          iscomment(line+2, "EOF")))) {
	    if (!preread) section_len += line_len;
	    preread = 0;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (dsc->deforient == DSC_NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    dsc->deforient = DSC_PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    dsc->deforient = DSC_LANDSCAPE;
		}
	    } else if (page_media_set == DSC_NONE &&
		       iscomment(line+2, "PaperSize:")) {
		cp = gettext(line+length("%%PaperSize:"), NULL);
		stolower(cp);
		for (dmp = dsc->media, i=0; i<dsc->n_media; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			dsc->defmedia = dmp;
			page_media_set = 1;
			break;
		    }
		}
		free(cp);
	    } else if (page_bb_set == DSC_NONE &&
		       iscomment(line+2, "PageBoundingBox:")) {
		if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			   &(dsc->defbbox[DSC_LLX]),
			   &(dsc->defbbox[DSC_LLY]),
			   &(dsc->defbbox[DSC_URX]),
			   &(dsc->defbbox[DSC_URY])) == 4)
		    page_bb_set = 1;
		else {
		    float fllx, flly, furx, fury;
		    if (sscanf(line+length("%%PageBoundingBox:"), "%f %f %f %f",
			       &fllx, &flly, &furx, &fury) == 4) {
			page_bb_set = 1;
			dsc->defbbox[DSC_LLX] = fllx;
			dsc->defbbox[DSC_LLY] = flly;
			dsc->defbbox[DSC_URX] = furx;
			dsc->defbbox[DSC_URY] = fury;
			if (fllx < dsc->defbbox[DSC_LLX])
			    dsc->defbbox[DSC_LLX]--;
			if (flly < dsc->defbbox[DSC_LLY])
			    dsc->defbbox[DSC_LLY]--;
			if (furx > dsc->defbbox[DSC_URX])
			    dsc->defbbox[DSC_URX]++;
			if (fury > dsc->defbbox[DSC_URY])
			    dsc->defbbox[DSC_URY]++;
		    }
		}
	    }
	}
	section_len += line_len;
	if (DSCcomment(line) && iscomment(line+2, "EndSetup")) {
	    readline(line, sizeof line, file, &position, &line_len);
	    section_len += line_len;
	}
	dsc->setup.end = position;
	dsc->setup.len = section_len - line_len;
    }

    /* Individual Pages */

    if (beginsection == 0) {
	beginsection = position;
	section_len = line_len;
    }
    while (blank(line) &&
	   readline(line, sizeof line, file, &position, &line_len)) {
	section_len += line_len;
    }

newpage:
    while (DSCcomment(line) && iscomment(line+2, "Page:")) {
	if (maxpages == 0) {
	    maxpages = 1;
	    dsc->pages = calloc(maxpages, sizeof(DSCP));
	    if (dsc->pages == NULL) {
		fprintf(stderr,
			"Fatal Error: Dynamic memory exhausted.\n");
		exit(EXIT_FAILURE);
	    }
	}
	label = gettext(line+length("%%Page:"), &next_char);
	if (sscanf(next_char, "%d", &thispage) != 1) thispage = 0;
	if (nextpage == 1) {
	    ignore = thispage != 1;
	}
	if (!ignore && thispage != nextpage) {
	    free(label);
	    dsc->n_pages--;
	    goto continuepage;
	}
	nextpage++;
	if (dsc->n_pages == (int)maxpages) {
	    maxpages++;
	    dsc->pages = realloc(dsc->pages, maxpages*sizeof (DSCP));
	    if (dsc->pages == NULL) {
		fprintf(stderr, "Fatal Error: Dynamic memory exhausted.\n");
		exit(EXIT_FAILURE);
	    }
	}
	memset(&(dsc->pages[dsc->n_pages]), 0, sizeof(DSCP));
	page_bb_set = DSC_NONE;
	dsc->pages[dsc->n_pages].label = label;
	if (beginsection) {
	    dsc->pages[dsc->n_pages].sect.begin = beginsection;
	    beginsection = 0;
	} else {
	    dsc->pages[dsc->n_pages].sect.begin = position;
	    section_len = line_len;
	}
continuepage:
	while (readline(line, sizeof line, file, &position, &line_len) &&
	       !(DSCcomment(line) &&
	         (iscomment(line+2, "Page:") ||
	          iscomment(line+2, "Trailer") ||
	          iscomment(line+2, "EOF")))) {
	    section_len += line_len;
	    if (!DSCcomment(line)) {
		/* Do nothing */
	    } else if (dsc->pages[dsc->n_pages].orientation == DSC_NONE &&
		iscomment(line+2, "PageOrientation:")) {
		sscanf(line+length("%%PageOrientation:"), "%s", text);
		if (strcmp(text, "Portrait") == 0) {
		    dsc->pages[dsc->n_pages].orientation = DSC_PORTRAIT;
		} else if (strcmp(text, "Landscape") == 0) {
		    dsc->pages[dsc->n_pages].orientation = DSC_LANDSCAPE;
		}
	    } else if (dsc->pages[dsc->n_pages].media == NULL &&
		       iscomment(line+2, "PageMedia:")) {
		cp = stolower(gettext(line+length("%%PageMedia:"), NULL));
		for (dmp = dsc->media, i=0; i<dsc->n_media; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			dsc->pages[dsc->n_pages].media = dmp;
			break;
		    }
		}
		free(cp);
	    } else if (dsc->pages[dsc->n_pages].media == NULL &&
		       iscomment(line+2, "PaperSize:")) {
		cp = stolower(gettext(line+length("%%PaperSize:"), NULL));
		for (dmp = dsc->media, i=0; i<dsc->n_media; i++, dmp++) {
		    if (strcmp(cp, dmp->name) == 0) {
			dsc->pages[dsc->n_pages].media = dmp;
			break;
		    }
		}
		free(cp);
	    } else if ((page_bb_set == DSC_NONE || page_bb_set == DSC_ATEND) &&
		       iscomment(line+2, "PageBoundingBox:")) {
		sscanf(line+length("%%PageBoundingBox:"), "%s", text);
		if (strcmp(text, "(atend)") == 0) {
		    page_bb_set = DSC_ATEND;
		} else {
		    if (sscanf(line+length("%%PageBoundingBox:"), "%d %d %d %d",
			    &(dsc->pages[dsc->n_pages].bbox[DSC_LLX]),
			    &(dsc->pages[dsc->n_pages].bbox[DSC_LLY]),
			    &(dsc->pages[dsc->n_pages].bbox[DSC_URX]),
			    &(dsc->pages[dsc->n_pages].bbox[DSC_URY])) == 4) {
			if (page_bb_set == DSC_NONE) {
				page_bb_set = 1;
			}
		    }
		    else {
			float fllx, flly, furx, fury;
			if (sscanf(line+length("%%PageBoundingBox:"),
				   "%f %f %f %f",
				   &fllx, &flly, &furx, &fury) == 4) {
			    if (page_bb_set == DSC_NONE) page_bb_set = 1;
			    dsc->pages[dsc->n_pages].bbox[DSC_LLX] = fllx;
			    dsc->pages[dsc->n_pages].bbox[DSC_LLY] = flly;
			    dsc->pages[dsc->n_pages].bbox[DSC_URX] = furx;
			    dsc->pages[dsc->n_pages].bbox[DSC_URY] = fury;
			    if (fllx <
				    dsc->pages[dsc->n_pages].bbox[DSC_LLX])
				dsc->pages[dsc->n_pages].bbox[DSC_LLX]--;
			    if (flly <
				    dsc->pages[dsc->n_pages].bbox[DSC_LLY])
				dsc->pages[dsc->n_pages].bbox[DSC_LLY]--;
			    if (furx >
				    dsc->pages[dsc->n_pages].bbox[DSC_URX])
				dsc->pages[dsc->n_pages].bbox[DSC_URX]++;
			    if (fury >
				    dsc->pages[dsc->n_pages].bbox[DSC_URY])
				dsc->pages[dsc->n_pages].bbox[DSC_URY]++;
			}
		    }
		}
	    }
	}
	section_len += line_len;
	dsc->pages[dsc->n_pages].sect.end = position;
	dsc->pages[dsc->n_pages].sect.len = section_len - line_len;
	dsc->n_pages++;
    }

    /* Document Trailer */

    if (beginsection) {
	dsc->trailer.begin = beginsection;
	beginsection = 0;
    } else {
	dsc->trailer.begin = position;
	section_len = line_len;
    }

    preread = 1;
    while ((preread ||
	    readline(line, sizeof line, file, &position, &line_len)) &&
	   !(DSCcomment(line) && iscomment(line+2, "EOF"))) {
	if (!preread) section_len += line_len;
	preread = 0;
	if (!DSCcomment(line)) {
	    /* Do nothing */
	} else if (iscomment(line+2, "Page:")) {
	    free(gettext(line+length("%%Page:"), &next_char));
	    if (sscanf(next_char, "%d", &thispage) != 1) thispage = 0;
	    if (!ignore && thispage == nextpage) {
		if (dsc->n_pages > 0) {
		    dsc->pages[dsc->n_pages-1].sect.end  = position;
		    dsc->pages[dsc->n_pages-1].sect.len += section_len - line_len;
		} else {
		    if (dsc->setup.end) {
			dsc->setup.end = position;
			dsc->setup.end += section_len - line_len;
		    } else if (dsc->prolog.end) {
			dsc->prolog.end = position;
			dsc->prolog.end += section_len - line_len;
		    }
		}
		goto newpage;
	    }
	} else if (bb_set == DSC_ATEND && iscomment(line+2, "BoundingBox:")) {
	    if (sscanf(line+length("%%BoundingBox:"), "%d %d %d %d",
		       &(dsc->bbox[DSC_LLX]),
		       &(dsc->bbox[DSC_LLY]),
		       &(dsc->bbox[DSC_URX]),
		       &(dsc->bbox[DSC_URY])) != 4) {
		float fllx, flly, furx, fury;
		if (sscanf(line+length("%%BoundingBox:"), "%f %f %f %f",
			   &fllx, &flly, &furx, &fury) == 4) {
		    dsc->bbox[DSC_LLX] = fllx;
		    dsc->bbox[DSC_LLY] = flly;
		    dsc->bbox[DSC_URX] = furx;
		    dsc->bbox[DSC_URY] = fury;
		    if (fllx < dsc->bbox[DSC_LLX])
			dsc->bbox[DSC_LLX]--;
		    if (flly < dsc->bbox[DSC_LLY])
			dsc->bbox[DSC_LLY]--;
		    if (furx > dsc->bbox[DSC_URX])
			dsc->bbox[DSC_URX]++;
		    if (fury > dsc->bbox[DSC_URY])
			dsc->bbox[DSC_URY]++;
		}
	    }
	} else if (orientation_set == DSC_ATEND &&
		   iscomment(line+2, "Orientation:")) {
	    sscanf(line+length("%%Orientation:"), "%s", text);
	    if (strcmp(text, "Portrait") == 0) {
		dsc->orientation = DSC_PORTRAIT;
	    } else if (strcmp(text, "Landscape") == 0) {
		dsc->orientation = DSC_LANDSCAPE;
	    }
	} else if (page_order_set == DSC_ATEND && iscomment(line+2, "PageOrder:")) {
	    sscanf(line+length("%%PageOrder:"), "%s", text);
	    if (strcmp(text, "Ascend") == 0) {
		dsc->pageorder = DSC_ASCEND;
	    } else if (strcmp(text, "Descend") == 0) {
		dsc->pageorder = DSC_DESCEND;
	    } else if (strcmp(text, "Special") == 0) {
		dsc->pageorder = DSC_SPECIAL;
	    }
	} else if (pages_set == DSC_ATEND && iscomment(line+2, "Pages:")) {
	    if (sscanf(line+length("%%Pages:"), "%*u %d", &i) == 1) {
		if (page_order_set == DSC_NONE) {
		    if (i == -1) dsc->pageorder = DSC_DESCEND;
		    else if (i == 0) dsc->pageorder = DSC_SPECIAL;
		    else if (i == 1) dsc->pageorder = DSC_ASCEND;
		}
	    }
	}
    }
    section_len += line_len;
    if (DSCcomment(line) && iscomment(line+2, "EOF")) {
	readline(line, sizeof line, file, &position, &line_len);
	section_len += line_len;
    }
    dsc->trailer.end = position;
    dsc->trailer.len = section_len - line_len;

	/*
	 * fix the page array
	 */
	if(dsc->pageorder == DSC_DESCEND) {
		DSCP	p;

		for(i = 0; i < dsc->n_pages/2; ++i) {
			p = dsc->pages[i];
			dsc->pages[i] = dsc->pages[dsc->n_pages - i - 1];
			dsc->pages[dsc->n_pages - i - 1] = p;
		}
	}

	/*
	 * Weird (or non-DSC) document.  Keep in mind that
	 * zero pages is the norm with EPS files (I remembered
	 * this the hard way).
	 */
	if(dsc->n_pages == 0
	&& dsc->type    != DSC_EPS)
		dsc = dsc_free(dsc);

	return dsc;
}

/*
 * free dynamic storage associated with document structure.
 */
DSC *
dsc_free(DSC *dsc)
{
	int	i;

	if(dsc == NULL)
		return NULL;

	for(i = 0; i < dsc->n_pages; ++i)
		free((void *)dsc->pages[i].label);
	for(i = 0; i < dsc->n_media; ++i)
		free((void *)dsc->media[i].name);

	free((void *)dsc->title);
	free((void *)dsc->date);
	free((void *)dsc->pages);
	free((void *)dsc->media);
	free((void *)dsc->forwho);
	free((void *)dsc->creator);
	free((void *)dsc);

	return NULL;
}

/*
 *	dsc_cp -- copy lines of Postscript from a section of one file
 *		  to another file.
 *                Automatically switch to binary copying whenever
 *                %%BeginBinary/%%EndBinary or %%BeginData/%%EndData
 *		  comments are encountered.
 */
void
dsc_cp(FILE *from, FILE *to, off_t begin, off_t end)
{
    char line[LINELEN];	/* 255 characters + 1 newline + 1 NULL */
    char text[LINELEN];	/* Temporary storage for text */
    unsigned int num;
    int i;
    char buf[BUFSIZ];

    memset((void *)&line[0], '\0', sizeof(line));
    if (begin >= 0) fseek(from, begin, SEEK_SET);
    while (ftell(from) < end) {

	fgets(line, sizeof line, from);
	fputs(line, to);

	if (!(DSCcomment(line) && iscomment(line+2, "Begin")))
	    continue;
	if (iscomment(line+7, "Data:")) {
	    text[0] = '\0';
	    if (sscanf(line+length("%%BeginData:"),
		       "%d %*s %s", &num, text) >= 1) {
		if (strcmp(text, "Lines") == 0) {
		    for (i=0; i < (int)num; i++) {
			fgets(line, sizeof(line), from);
			fputs(line, to);
		    }
		} else {
		    while (num > sizeof(buf)) {
			fread(buf, 1, sizeof(buf), from);
			fwrite(buf, 1, sizeof(buf), to);
			num -= sizeof(buf);
		    }
		    fread(buf, sizeof (char), num, from);
		    fwrite(buf, sizeof (char), num, to);
		}
	    }
	} else if (iscomment(line+7, "Binary:")) {
	    if(sscanf(line+length("%%BeginBinary:"), "%d", &num) == 1) {
		while (num > sizeof(buf)) {
		    fread(buf, 1, sizeof(buf), from);
		    fwrite(buf, 1, sizeof(buf), to);
		    num -= sizeof(buf);
		}
		fread(buf, 1, num, from);
		fwrite(buf, 1, num, to);
	    }
	}
    }
}

/*
 *	dsc_cpuntil -- copy lines of Postscript from a section of one file
 *		       to another file until a particular comment is reached.
 *                     Automatically switch to binary copying whenever
 *                     %%BeginBinary/%%EndBinary or %%BeginData/%%EndData
 *		       comments are encountered.
 */

char *
dsc_cpuntil(FILE *from, FILE *to, off_t begin, off_t end, char *comment)
{
	char		line[LINELEN], text[LINELEN], buf[BUFSIZ];
	unsigned	num;
	int		i, comment_length;
	char		*cp;

	comment_length = strlen(comment);
	if(begin >= 0)
		fseek(from, begin, SEEK_SET);
	while(ftell(from) < end) {
		fgets(line, sizeof line, from);

		/*
		 * iscomment cannot be used here,
		 * because comment_length is not known at compile time.
		 */
		if(strncmp(line, comment, comment_length) == 0) {
			if((cp = malloc(strlen(line) + 1)) == NULL) {
				fprintf(stderr, "Fatal Error: "
						"Dynamic memory exhausted.\n");
				exit(EXIT_FAILURE);
			}
			return strcpy(cp, line);
		}

		fputs(line, to);
		if(!(DSCcomment(line) && iscomment(line+2, "Begin")))
			continue;

		if(iscomment(line+7, "Data:")) {
			text[0] = '\0';
			i = sscanf(line+length("%%BeginData:"), "%d %*s %s",
								&num, text);
			if(i >= 1) {
				if(strcmp(text, "Lines") == 0) {
					for(i = 0; i < (int)num; ++i) {
						fgets(line, sizeof(line), from);
						fputs(line, to);
					}
				}
				else {
					while(num > sizeof(buf)) {
						fread(buf,  1,sizeof(buf),from);
						fwrite(buf, 1,sizeof(buf),to);
						num -= sizeof(buf);
					}
					fread(buf,  1, num, from);
					fwrite(buf, 1, num, to);
				}
			}
		}
		else if(iscomment(line+7, "Binary:")) {
			i = sscanf(line+length("%%BeginBinary:"), "%d", &num);
			if(i == 1) {
				while(num > sizeof(buf)) {
					fread(buf,  1, sizeof(buf), from);
 					fwrite(buf, 1, sizeof(buf), to);
					num -= sizeof(buf);
				}
				fread(buf,  1, num, from);
				fwrite(buf, 1, num, to);
			}
		}
	}
	return NULL;
}
