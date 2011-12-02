/*
** Xstd.h for STD
** 
** Made by Hydrix
** Login   <hydrix@epita.fr>
** 
** Started on  Mon Jul 25 13:07:44 1994 hydrix
** Last update Wed Oct 19 15:21:04 1994 hydrix
*/

#ifndef __X_STD_H__
#define __X_STD_H__	1

/* Xlib */

#define LOOKUP_STRING_BUFSIZ	512

#define XFS_WIDTH(xfs)		(((xfs)->max_bounds).width)
#define XFS_HEIGHT(xfs)		((xfs)->descent+(xfs)->ascent)
#define XFS_DESCENT(xfs)	((xfs)->descent)
#define XFS_FID(xfs)		((xfs)->fid)

/* Xtoolkit */

#define W_SCREEN(w)		DefaultScreen(XtDisplay(w))
#define W_DEPTH(w)		DefaultDepth(XtDisplay(w),W_SCREEN(w))
#define W_BPX(w)		BlackPixel(XtDisplay(w),W_SCREEN(w))
#define W_WPX(w)		WhitePixel(XtDisplay(w),W_SCREEN(w))
#define W_ROOT(w)		DefaultRootWindow(XtDisplay(w))
#define W_CMAP(w)		DefaultColormap(XtDisplay(w),W_SCREEN(w))

#endif /* __X_STD_H__ */
