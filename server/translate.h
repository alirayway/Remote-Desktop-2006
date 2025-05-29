

/* translate.h - prototypes of functions in translate.cpp */

#ifndef TRANSLATE_H__
#define TRANSLATE_H__

#include "stdhdrs.h"
#include "rfb.h"

// Translate function prototype!
typedef void (*rfbTranslateFnType)(char *table, rfbPixelFormat *in,
				   rfbPixelFormat *out,
				   char *iptr, char *optr,
				   int bytesBetweenInputLines,
				   int width, int height);

// Init function prototype!
typedef void (*rfbInitTableFnType)(char **table, rfbPixelFormat *in,
				   rfbPixelFormat *out);


// External translation stuff
extern void rfbTranslateNone(char *table, rfbPixelFormat *in,
			     rfbPixelFormat *out,
			     char *iptr, char *optr,
			     int bytesBetweenInputLines,
			     int width, int height);

// Macro to compare pixel formats.
#define PF_EQ(x,y)												\
	((x.bitsPerPixel == y.bitsPerPixel) &&						\
	 (x.depth == y.depth) &&									\
	 (x.trueColour == y.trueColour) &&							\
	 ((x.bigEndian == y.bigEndian) || (x.bitsPerPixel == 8)) &&	\
	 (!x.trueColour || ((x.redMax == y.redMax) &&				\
			   (x.greenMax == y.greenMax) &&					\
			   (x.blueMax == y.blueMax) &&						\
			   (x.redShift == y.redShift) &&					\
			   (x.greenShift == y.greenShift) &&				\
			   (x.blueShift == y.blueShift))))

// Translation functions themselves
extern rfbInitTableFnType rfbInitTrueColourSingleTableFns[];
extern rfbInitTableFnType rfbInitColourMapSingleTableFns[];
extern rfbInitTableFnType rfbInitTrueColourRGBTablesFns[];
extern rfbTranslateFnType rfbTranslateWithSingleTableFns[3][3];
extern rfbTranslateFnType rfbTranslateWithRGBTablesFns[3][3];

/*
extern Bool rfbSetTranslateFunction(rfbClientPtr cl);
extern void rfbSetClientColourMaps(int firstColour, int nColours);
extern Bool rfbSetClientColourMap(rfbClientPtr cl, int firstColour,
				  int nColours);
*/

#endif
