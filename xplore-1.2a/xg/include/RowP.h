/*@
 * Copyright(c) 1995-1997 Gregory M. Messner
 * All rights reserved
 *
 * Permission to use, copy, modify and distribute this material for
 * non-commercial personal and educational use without fee is hereby
 * granted, provided that the above copyright notice and this permission 
 * notice appear in all copies, and that the name of Gregory M. Messner
 * not be used in advertising or publicity pertaining to this material
 * without the specific, prior written permission of Gregory M. Messner 
 * or an authorized representative.
 *
 * GREGORY M. MESSNER MAKES NO REPRESENTATIONS AND EXTENDS NO WARRANTIES, 
 * EXPRESS OR IMPLIED, WITH RESPECT TO THE SOFTWARE, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * ANY PARTICULAR PURPOSE, AND THE WARRANTY AGAINST INFRINGEMENT OF PATENTS
 * OR OTHER INTELLECTUAL PROPERTY RIGHTS. THE SOFTWARE IS PROVIDED "AS IS",
 * AND IN NO EVENT SHALL GREGORY M. MESSNER BE LIABLE FOR ANY DAMAGES,
 * INCLUDING ANY LOST PROFITS OR OTHER INCIDENTAL OR CONSEQUENTIAL DAMAGES
 * RELATING TO THE SOFTWARE.
 *
 */


#ifndef ROWP_H
#define ROWP_H

#if XmVersion > 1001
#  include    <Xm/ManagerP.h>
#endif

#include    "Row.h"



#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */




typedef struct _XgRowClassPart {
          int    empty;
} XgRowClassPart;

typedef struct _XgRowClassRec {
    CoreClassPart       core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart  manager_class;
    XgRowClassPart      row_class;
} XgRowClassRec;

extern XgRowClassRec XgrowClassRec;

typedef struct {
    unsigned char     	orientation;
    Boolean     	hstretch,
                	vstretch;

    Dimension 		spacing;

    int         	align_children;
} XgRowPart;


typedef struct _XgRowRec {
    CorePart        core;
    CompositePart   composite;
    ConstraintPart  constraint;
    XmManagerPart   manager;
    XgRowPart       row;
}  XgRowRec;


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* ROWP_H */
