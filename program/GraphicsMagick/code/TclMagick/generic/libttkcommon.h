
#ifndef _ttkcommon_h_
#define _ttkcommon_h_

#include "TclMagick.h"

struct TMHT {
    int             initialized; /* Whether module has been initialized */
    Tcl_HashTable   hashTable;   /* HasTable for MagickWand objects */
};
typedef struct TMHT TMHT;

EXPORT(TclMagickObj, *findMagickObj)(Tcl_Interp *interp, int type, char *name);
EXPORT(MagickWand, *findMagickWand)(Tcl_Interp *interp, char *name);
EXPORT(DrawingWand, *findDrawingWand)(Tcl_Interp *interp, char *name);
EXPORT(PixelWand, *findPixelWand)(Tcl_Interp *interp, char *name);
EXPORT(int, myMagickError)(Tcl_Interp *interp,MagickWand *wandPtr);
#endif

/* vim: set ts=8 sts=8 sw=8 noet: */
