/*
 * tcltkcommon.c: data and functions shared by TclMagick.c TkMagick.c
 */

#include "libttkcommon.h"
#include "TclMagick.h"

TMHT TM = {0};

CONST char *objTypeNames[] = {
    "wand", "drawing", "pixel", (char *) NULL
};

TclMagickObj* findMagickObj(Tcl_Interp *interp, int type, char *name)
{
    Tcl_HashEntry *hPtr = Tcl_FindHashEntry( &TM.hashTable, name );
    TclMagickObj  *mPtr;

    if( hPtr == NULL ) {
        Tcl_AppendResult(interp, "Magick object '", name, "' not found", NULL);
        return (TclMagickObj *)NULL;
    } else {
        mPtr = (TclMagickObj *)Tcl_GetHashValue(hPtr);
        if( (type != TM_TYPE_ANY) && (mPtr->type != type) ) {
            Tcl_AppendResult(interp,"Magick object '",name,"' is not a ",objTypeNames[type]," object",NULL);
            return (TclMagickObj *)NULL;
        }
        return mPtr;
    }
}

MagickWand *findMagickWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_WAND, name);
    if( mPtr == NULL ) {
        return (MagickWand *)NULL;
    }
    return (MagickWand *)mPtr->wandPtr;
}

DrawingWand *findDrawingWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_DRAWING, name);
    if( mPtr == NULL ) {
        return (DrawingWand *)NULL;
    }
    return (DrawingWand *)mPtr->wandPtr;
}

PixelWand *findPixelWand(Tcl_Interp *interp, char *name)
{
    TclMagickObj *mPtr = findMagickObj(interp, TM_TYPE_PIXEL, name);
    if( mPtr == NULL ) {
        return (PixelWand *)NULL;
    }
    return (PixelWand *)mPtr->wandPtr;
}

/*----------------------------------------------------------------------
 * Return Magick error description as a TCL result
 * Used by both TclMagick and TkMagick.
 *----------------------------------------------------------------------
 */
int myMagickError(Tcl_Interp  *interp, MagickWand *wandPtr )
{
    char *description;

    ExceptionType severity;
    char msg[40];

    description = MagickGetException(wandPtr, &severity);
    if( (description == NULL) || (strlen(description) == 0) ) {
        Tcl_AppendResult(interp, MagickGetPackageName(), ": Unknown error", NULL);
    } else {
        sprintf(msg, "%s: #%d:", MagickGetPackageName(), severity); /* FIXME, not used! */
        Tcl_AppendResult(interp, description, NULL);
    }
    if( description != NULL ) {
        MagickRelinquishMemory(description);
    }
    /*
     * if(severity < ErrorException) --> warning
     * return TCL_OK ???
     */
    return TCL_ERROR;
}

/* vim: set ts=8 sts=8 sw=8 noet: */

