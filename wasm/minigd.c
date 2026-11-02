#include "gd.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#define STBI_WRITE_NO_STDIO
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include "stb_image_write.h"
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#include <stdlib.h>
#include <string.h>

static gdImagePtr
allocate_image(int sx, int sy, int trueColor)
{
    if (sx <= 0 || sy <= 0)
        return NULL;

    gdImagePtr im = (gdImagePtr)calloc(1, sizeof(*im));
    if (!im)
        return NULL;

    im->sx = sx;
    im->sy = sy;
    im->trueColor = trueColor;
    im->transparent = -1;

    if (trueColor) {
        im->tpixels = (unsigned int *)calloc((size_t)sx * (size_t)sy, sizeof(unsigned int));
        if (!im->tpixels) {
            free(im);
            return NULL;
        }
    } else {
        im->pixels = (unsigned char *)calloc((size_t)sx * (size_t)sy, sizeof(unsigned char));
        if (!im->pixels) {
            free(im);
            return NULL;
        }
    }

    return im;
}

gdImagePtr
gdImageCreate(int sx, int sy)
{
    return allocate_image(sx, sy, 0);
}

gdImagePtr
gdImageCreateTrueColor(int sx, int sy)
{
    return allocate_image(sx, sy, 1);
}

static unsigned int
pack_color(int r, int g, int b, int a)
{
    if (r < 0)
        r = 0;
    if (r > 255)
        r = 255;
    if (g < 0)
        g = 0;
    if (g > 255)
        g = 255;
    if (b < 0)
        b = 0;
    if (b > 255)
        b = 255;
    if (a < 0)
        a = 0;
    if (a > 255)
        a = 255;
    return ((unsigned int)a << 24) | ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
}

int
gdImageColorAllocate(gdImagePtr im, int r, int g, int b)
{
    if (!im)
        return 0;

    unsigned int color = pack_color(r, g, b, 255);

    if (im->trueColor)
        return (int)color;

    for (int i = 0; i < im->colorsTotal; i++) {
        if (im->palette[i] == color)
            return i;
    }

    if (im->colorsTotal >= 256)
        return 0;

    int idx = im->colorsTotal++;
    im->palette[idx] = color;
    return idx;
}

static inline void
set_palette_pixel(gdImagePtr im, int x, int y, int color)
{
    size_t offset = (size_t)y * (size_t)im->sx + (size_t)x;
    if (offset < (size_t)im->sx * (size_t)im->sy)
        im->pixels[offset] = (unsigned char)color;
}

static inline void
set_truecolor_pixel(gdImagePtr im, int x, int y, unsigned int color)
{
    size_t offset = (size_t)y * (size_t)im->sx + (size_t)x;
    if (offset < (size_t)im->sx * (size_t)im->sy)
        im->tpixels[offset] = color;
}

void
gdImageFill(gdImagePtr im, int x, int y, int color)
{
    (void)x;
    (void)y;

    if (!im)
        return;

    gdImageFilledRectangle(im, 0, 0, im->sx - 1, im->sy - 1, color);
}

static void
normalize_rect(int *x1, int *y1, int *x2, int *y2)
{
    if (*x1 > *x2) {
        int tmp = *x1;
        *x1 = *x2;
        *x2 = tmp;
    }
    if (*y1 > *y2) {
        int tmp = *y1;
        *y1 = *y2;
        *y2 = tmp;
    }
}

static inline int
clamp(int value, int min, int max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}

void
gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
    if (!im)
        return;

    normalize_rect(&x1, &y1, &x2, &y2);

    x1 = clamp(x1, 0, im->sx - 1);
    x2 = clamp(x2, 0, im->sx - 1);
    y1 = clamp(y1, 0, im->sy - 1);
    y2 = clamp(y2, 0, im->sy - 1);

    if (im->trueColor) {
        unsigned int packed = (unsigned int)color;
        for (int y = y1; y <= y2; y++) {
            for (int x = x1; x <= x2; x++)
                set_truecolor_pixel(im, x, y, packed);
        }
    } else {
        if (color >= im->colorsTotal)
            color = im->colorsTotal ? im->colorsTotal - 1 : 0;
        for (int y = y1; y <= y2; y++) {
            for (int x = x1; x <= x2; x++)
                set_palette_pixel(im, x, y, color);
        }
    }
}

void
gdImageSetPixel(gdImagePtr im, int x, int y, int color)
{
    if (!im)
        return;

    if (x < 0 || x >= im->sx || y < 0 || y >= im->sy)
        return;

    if (im->trueColor)
        set_truecolor_pixel(im, x, y, (unsigned int)color);
    else
        set_palette_pixel(im, x, y, color);
}

int
gdImageTrueColor(gdImagePtr im)
{
    return im ? im->trueColor : 0;
}

void
gdImageColorTransparent(gdImagePtr im, int color)
{
    if (!im)
        return;

    im->transparent = color;
}

static unsigned int
get_pixel_color(gdImagePtr im, int x, int y)
{
    if (!im)
        return 0;

    x = clamp(x, 0, im->sx - 1);
    y = clamp(y, 0, im->sy - 1);

    size_t offset = (size_t)y * (size_t)im->sx + (size_t)x;

    if (im->trueColor) {
        return im->tpixels[offset];
    } else {
        int idx = im->pixels[offset];
        if (idx < 0 || idx >= im->colorsTotal)
            return 0;
        return im->palette[idx];
    }
}

static int
ensure_palette_color(gdImagePtr im, unsigned int color)
{
    for (int i = 0; i < im->colorsTotal; i++) {
        if (im->palette[i] == color)
            return i;
    }

    if (im->colorsTotal >= 256)
        return 0;

    int idx = im->colorsTotal++;
    im->palette[idx] = color;
    return idx;
}

static void
set_pixel_from_color(gdImagePtr im, int x, int y, unsigned int color)
{
    if (im->trueColor)
        set_truecolor_pixel(im, x, y, color);
    else {
        int idx = ensure_palette_color(im, color);
        set_palette_pixel(im, x, y, idx);
    }
}

static void
copy_scaled(gdImagePtr dst, gdImagePtr src,
    int dstX, int dstY, int srcX, int srcY,
    int dstW, int dstH, int srcW, int srcH)
{
    if (!dst || !src)
        return;

    if (dstW <= 0 || dstH <= 0 || srcW <= 0 || srcH <= 0)
        return;

    for (int y = 0; y < dstH; y++) {
        int dy = dstY + y;
        if (dy < 0 || dy >= dst->sy)
            continue;

        double srcYOffset = (double)y / (double)dstH;
        double syf = srcYOffset * srcH;
        int sy = clamp(srcY + (int)(syf + 0.5 - 0.5), srcY, srcY + srcH - 1);

        for (int x = 0; x < dstW; x++) {
            int dx = dstX + x;
            if (dx < 0 || dx >= dst->sx)
                continue;

            double srcXOffset = (double)x / (double)dstW;
            double sxf = srcXOffset * srcW;
            int sx = clamp(srcX + (int)(sxf + 0.5 - 0.5), srcX, srcX + srcW - 1);

            unsigned int color = get_pixel_color(src, sx, sy);
            set_pixel_from_color(dst, dx, dy, color);
        }
    }
}

void
gdImageCopyResized(gdImagePtr dst, gdImagePtr src,
    int dstX, int dstY, int srcX, int srcY,
    int dstW, int dstH, int srcW, int srcH)
{
    copy_scaled(dst, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
}

void
gdImageCopyResampled(gdImagePtr dst, gdImagePtr src,
    int dstX, int dstY, int srcX, int srcY,
    int dstW, int dstH, int srcW, int srcH)
{
    copy_scaled(dst, src, dstX, dstY, srcX, srcY, dstW, dstH, srcW, srcH);
}

static void
rgba_from_color(unsigned int color, unsigned char *rgba)
{
    rgba[0] = (unsigned char)((color >> 16) & 0xFF);
    rgba[1] = (unsigned char)((color >> 8) & 0xFF);
    rgba[2] = (unsigned char)(color & 0xFF);
    rgba[3] = (unsigned char)((color >> 24) & 0xFF);
}

struct png_buffer {
    unsigned char *data;
    size_t size;
    size_t capacity;
    int failed;
};

static void
png_write(void *context, void *data, int size)
{
    struct png_buffer *buf = (struct png_buffer *)context;
    if (buf->failed)
        return;

    size_t required = buf->size + (size_t)size;
    if (required > buf->capacity) {
        size_t new_capacity = buf->capacity ? buf->capacity * 2 : 1024;
        while (new_capacity < required)
            new_capacity *= 2;
        unsigned char *new_data = (unsigned char *)realloc(buf->data, new_capacity);
        if (!new_data) {
            free(buf->data);
            buf->data = NULL;
            buf->size = buf->capacity = 0;
            buf->failed = 1;
            return;
        }
        buf->data = new_data;
        buf->capacity = new_capacity;
    }

    memcpy(buf->data + buf->size, data, (size_t)size);
    buf->size += (size_t)size;
}

unsigned char *
gdImagePngPtr(gdImagePtr im, int *out_size)
{
    if (!im || !out_size)
        return NULL;

    size_t pixel_count = (size_t)im->sx * (size_t)im->sy;
    unsigned char *rgba = (unsigned char *)malloc(pixel_count * 4);
    if (!rgba)
        return NULL;

    for (size_t i = 0; i < pixel_count; i++) {
        unsigned int color;
        if (im->trueColor) {
            color = im->tpixels[i];
            if (im->transparent != -1 && (int)color == im->transparent)
                color &= 0x00FFFFFF;
        } else {
            int idx = (int)im->pixels[i];
            if (idx < 0 || idx >= im->colorsTotal)
                idx = 0;
            color = im->palette[idx];
            if (idx == im->transparent)
                color &= 0x00FFFFFF;
        }

        rgba_from_color(color, &rgba[i * 4]);

        if (!im->trueColor) {
            int idx = (int)im->pixels[i];
            if (idx == im->transparent)
                rgba[i * 4 + 3] = 0;
            else
                rgba[i * 4 + 3] = 255;
        }
    }

    struct png_buffer buf = {0};
    int stride = im->sx * 4;
    int ok = stbi_write_png_to_func(png_write, &buf, im->sx, im->sy, 4, rgba, stride);
    free(rgba);

    if (!ok || !buf.data || buf.failed)
        return NULL;

    unsigned char *result = (unsigned char *)malloc(buf.size);
    if (!result) {
        free(buf.data);
        return NULL;
    }

    memcpy(result, buf.data, buf.size);
    free(buf.data);

    *out_size = (int)buf.size;
    return result;
}

void
gdImageDestroy(gdImagePtr im)
{
    if (!im)
        return;

    free(im->pixels);
    free(im->tpixels);
    free(im);
}

void
gdFree(void *ptr)
{
    free(ptr);
}
