#ifndef GD_H
#define GD_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct gdImage {
	int sx;
	int sy;
	int trueColor;
	int transparent;
	int colorsTotal;
	unsigned int palette[256];
	unsigned char *pixels;
	unsigned int *tpixels;
};

typedef struct gdImage *gdImagePtr;

gdImagePtr gdImageCreate(int sx, int sy);
gdImagePtr gdImageCreateTrueColor(int sx, int sy);
int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
void gdImageFill(gdImagePtr im, int x, int y, int color);
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageSetPixel(gdImagePtr im, int x, int y, int color);
void gdImageCopyResized(gdImagePtr dst, gdImagePtr src,
    int dstX, int dstY, int srcX, int srcY,
    int dstW, int dstH, int srcW, int srcH);
void gdImageCopyResampled(gdImagePtr dst, gdImagePtr src,
    int dstX, int dstY, int srcX, int srcY,
    int dstW, int dstH, int srcW, int srcH);
int gdImageTrueColor(gdImagePtr im);
void gdImageColorTransparent(gdImagePtr im, int color);
unsigned char *gdImagePngPtr(gdImagePtr im, int *out_size);
void gdImageDestroy(gdImagePtr im);
void gdFree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif /* GD_H */
