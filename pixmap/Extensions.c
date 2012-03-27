/*
 * $Id: Extensions.c,v 1.4 1994/05/13 14:12:07 mallet Exp $
 *
 * Copyright (c) 1991-1994  Lionel MALLET
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL Lionel MALLET	BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Lionel MALLET shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from Lionel MALLET.
 *
 * Author:  Tim Wise - Scientific & Engineering Software (SES), Inc.
 */

#define UNKNOWN -1

/*****************************************************************************/
/*
			Extension List Routines 

	Routines to deal with array of XpmExtensions. 
*/
/*****************************************************************************/


/*--------------------------------------------------------------------------*/
/*
			C o p y E x t e n s i o n 

	Copy an xpm extension.
*/
/*--------------------------------------------------------------------------*/
void CopyExtension( dst, src )
    XpmExtension *dst;
    XpmExtension *src;
{
    int          i;

    if ( dst == NULL ) 
        return;

    dst->name   = XtNewString( src->name );
    dst->nlines = src->nlines;

    dst->lines  = (char **) XtCalloc( src->nlines, sizeof(char**) );
    for ( i=0; i < src->nlines; i++ ) 
        dst->lines[i] = XtNewString( src->lines[i] );
}


/*--------------------------------------------------------------------------*/
/*
			F r e e E x t e n s i o n 

	Free the name string and the lines of an XpmExtension but don't
	free the XpmExtension struct itself.
*/
/*--------------------------------------------------------------------------*/
void FreeExtension( extension )
    XpmExtension  *extension;
{
    int          i;

    if ( extension == NULL ) 
        return;

    XtFree( extension->name );
    for ( i=0; i < extension->nlines; i++ ) 
       XtFree( extension->lines[i] );
    XtFree((char *)extension->lines);
}


/*--------------------------------------------------------------------------*/
/*
                        F i n d E x t e n s i o n

        Find an extension given its name. If found, return index of
        extension; otherwise return UNKNOWN.
*/
/*--------------------------------------------------------------------------*/
int FindExtension( exts, n, name )
    XpmExtension *exts;         /* array of extensions */
    int          n;             /* number of extensions in array */
    char         *name;         /* name of extension to find */
{
    int           i;            /* index of found element */

    if ( exts == NULL ) 
        return UNKNOWN;

    for (i=0; i < n; i++, exts++) {
        if (strcmp(exts->name, name) == 0) {
            return i;
        }
    }
    return UNKNOWN;
}


/*--------------------------------------------------------------------------*/
/*
			A d d E x t e n s i o n

	Add extension with given name to end of list. Return index of 
	new extension.
*/
/*--------------------------------------------------------------------------*/
int AddExtension( exts, n, name )
    XpmExtension **exts;	/* array of extensions */
    int		 *n;		/* number of extensions in array */
    char         *name;		/* name of new extension */
{
    XpmExtension *e;
    int          i;		/* index of new element */

    if ( exts == NULL )
        return UNKNOWN;

    /* allocate new extension array */
    i = *n;	
    (*n)++;			/* increment count */
    if (*exts != NULL) 
        *exts = (XpmExtension *) XtRealloc((char *)*exts,
					   *n * sizeof(XpmExtension) );
    else 
        *exts = (XpmExtension *) XtNew( XpmExtension );

    /* put new element at end of array and initialize */
    e = *exts + i;
    e->name = XtNewString( name );
    e->nlines = 0;
    e->lines = NULL;

    return i;
}




/*--------------------------------------------------------------------------*/
/*
			R e m o v e E x t e n s i o n

	Delete extension i from list. Shift elements above i to left.
*/
/*--------------------------------------------------------------------------*/
void RemoveExtension( exts, n, i )
    XpmExtension **exts;	/* array of extensions */
    int		 *n;		/* number of extensions in array */
    int		 i;		/* index to delete */
{
    XpmExtension *t, *s, *e, *last;
    int          j;

    if ( exts == NULL || i == UNKNOWN ) 
        return;

    /* free extension info */
    e = *exts + i;
    XtFree(e->name);
    for (j = 0; j < e->nlines; j++)
        if (e->lines[j] != NULL)
            XtFree(e->lines[j]);
    if (e->lines != NULL)
        XtFree((char *)e->lines);

    last = *exts + *n;		/* last element */
    (*n)--; 

    /* if other extensions, shift left those after deleted one */
    /* Note: we're doing a structure assignment! */
    if (*n > 0) 
        for ( t=(*exts)+i, s=(*exts)+i+1; s < last ; *t++ = *s++ );
    else {
        XtFree((char *)*exts);
        *exts = NULL;
    }
}


/*--------------------------------------------------------------------------*/
/*
                        C o p y E x t e n s i o n s

        Copy an array of extensions.
*/
/*--------------------------------------------------------------------------*/
void CopyExtensions( dst_e, dst_n, src_e, src_n )
    XpmExtension **dst_e;
    int          *dst_n;
    XpmExtension *src_e;
    int          src_n;
{
    XpmExtension *te, *se;

    if ( dst_e == NULL )
        return;

    *dst_e = NULL;
    *dst_n = 0;

    if (src_e != NULL && src_n > 0) {
        *dst_n = src_n;
        *dst_e = (XpmExtension *) XtCalloc( src_n, sizeof(XpmExtension) );
        for ( te = *dst_e, se = src_e; se < src_e + src_n; te++, se++ ) 
            CopyExtension( te, se );
    }
}



/*--------------------------------------------------------------------------*/
/*
			P r i n t E x t e n s i o n s
*/
/*--------------------------------------------------------------------------*/
void PrintExtensions( exts, n )
    XpmExtension *exts;
    int          n;
{
    int i, j;
    if ( exts != NULL ) {
        for (i = 0; i < n; i++) {
            fprintf(stderr, "Xpm extension : %s\n", exts[i].name);
            for (j = 0; j < exts[i].nlines; j++) {
                fprintf( stderr, "\t\t%s\n", exts[i].lines[j] );
            }
        }
    }
}


/*****************************************************************************/
/*
		Interface to PW extensions
*/
/*****************************************************************************/


/*--------------------------------------------------------------------------*/
/*
			P W G e t E x t e n s i o n N a m e s

	Return an array of extension names. The list is terminated with
	a null name;
*/
/*--------------------------------------------------------------------------*/
char **PWGetExtensionNames( w )
    Widget w;
{
    PixmapWidget PW = (PixmapWidget) w;
    int          i;
    char         **names = NULL;

    names = (char **) XtCalloc( PW->pixmap.nextensions + 1, sizeof(char *) );
    names[ PW->pixmap.nextensions ] = NULL;

    for ( i = 0; i < PW->pixmap.nextensions ; i++ ) 
        names[i] = XtNewString( PW->pixmap.extensions[i].name );

    return names;
}


/*--------------------------------------------------------------------------*/
/*
			P W F i n d E x t e n s i o n
*/
/*--------------------------------------------------------------------------*/
XpmExtension *PWFindExtension( w, name )
    Widget w;
    String name;
{
    PixmapWidget PW = (PixmapWidget) w;
    int          i;
    XpmExtension *extension = NULL;

    if ( (i=FindExtension(PW->pixmap.extensions, PW->pixmap.nextensions, name)) 
        != UNKNOWN ) {
        extension = XtNew( XpmExtension );
        CopyExtension( extension, PW->pixmap.extensions + i );  
    }

    return extension;
}


/*--------------------------------------------------------------------------*/
/*
			P W A d d E x t e n s i o n

	Add an extension if it doesn't already exist.
*/
/*--------------------------------------------------------------------------*/
void PWAddExtension( w, name )
    Widget w;
    String name;
{
    PixmapWidget PW = (PixmapWidget) w;

    if ( FindExtension(PW->pixmap.extensions, PW->pixmap.nextensions, name)
         == UNKNOWN ) {

        AddExtension( &(PW->pixmap.extensions), 
                      &(PW->pixmap.nextensions), name );
    }
}


/*--------------------------------------------------------------------------*/
/*
			P W R e m o v e E x t e n s i o n
*/
/*--------------------------------------------------------------------------*/
void PWRemoveExtension( w, name )
    Widget w;
    String name;
{
    PixmapWidget PW = (PixmapWidget) w;
    int          i;

    if ( (i=FindExtension(PW->pixmap.extensions, PW->pixmap.nextensions, name))
         != UNKNOWN ) {

        RemoveExtension(&(PW->pixmap.extensions), &(PW->pixmap.nextensions), i);
    }
}


/*--------------------------------------------------------------------------*/
/*
			P W U p d a t e  E x t e n s i o n
*/
/*--------------------------------------------------------------------------*/
void PWUpdateExtension( w, new_extension )
    Widget w;
    XpmExtension *new_extension;
{
    PixmapWidget PW = (PixmapWidget) w;
    int          i;

    if ( (i=FindExtension(PW->pixmap.extensions, PW->pixmap.nextensions, 
                          new_extension->name))
         != UNKNOWN ){

        if ( new_extension->nlines > 0 ) {
            FreeExtension( PW->pixmap.extensions + i );
            CopyExtension( PW->pixmap.extensions + i, new_extension );
        } else {
            RemoveExtension( &(PW->pixmap.extensions), 
                             &(PW->pixmap.nextensions), i );
        }
    }
}
