#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#define SHIFT_R         (0)
#define SHIFT_G         (8)
#define SHIFT_B         (16)
#define SHIFT_A         (24)

#define RVAL(l)         ((int)(((l)>>SHIFT_R)&0xff))
#define GVAL(l)         ((int)(((l)>>SHIFT_G)&0xff))
#define BVAL(l)         ((int)(((l)>>SHIFT_B)&0xff))
#define AVAL(l)         ((int)(((l)>>SHIFT_A)&0xff))

#define CPACK(r, g, b, a)  (((r)<<SHIFT_R) | ((g)<<SHIFT_G) | ((b)<<SHIFT_B) | ((a)<<SHIFT_A))

typedef struct canvas {
    unsigned int *data;
    int sizex, sizey;
} canvas;

float flerp(float f0, float f1, float p)
{
    if(f0==f1)
        return f0;
    return ((f0*(1.0-p))+(f1*p));
}

canvas *canvas_new(int sizex, int sizey)
{
    canvas *c = (canvas *)malloc(sizeof(canvas));
    c->sizex = sizex;
    c->sizey = sizey;
    c->data = (unsigned int *)malloc(sizex*sizey*sizeof(unsigned int));
    return c;
}

void canvas_free(canvas *c)
{
    if(!c)
        return;
    free(c->data);
    free(c);
}

canvas *canvas_maxrgb(canvas *in)
{
    canvas *out = canvas_new(in->sizex, in->sizey);
    unsigned int *iptr = in->data;
    unsigned int *optr = out->data;
    int n = in->sizex*in->sizey;
    while(n--) {
        int r = RVAL(*iptr);
        int g = GVAL(*iptr);
        int b = BVAL(*iptr);
        int max = r;
        if(max < g) max = g;
        if(max < b) max = b;
        *optr = CPACK(max, max, max, 255);
        iptr++;
        optr++;
    }
    return out;
}

canvas *canvas_brighten(canvas *in, canvas *maxrgbblur, float param)
{
    float illummin = 1.0/flerp(1.0, 10.0, param*param);
    float illummax = 1.0/flerp(1.0, 1.111, param*param);
    canvas *out = canvas_new(in->sizex, in->sizey);
    unsigned int *iptr = in->data;
    unsigned int *bptr = maxrgbblur->data;
    unsigned int *optr = out->data;
    int n = in->sizex*in->sizey;
    while(n--) {
        float illum = RVAL(*bptr)/255.0;
        int r = RVAL(*iptr);
        int g = GVAL(*iptr);
        int b = BVAL(*iptr);
        if(illum < illummin)
            illum = illummin;
        if(illum < illummax) {
            float p = illum/illummax;
            float scale = (0.4+p*0.6)*(illummax/illum);
            r = scale*r;
            if(r>255) r = 255;
            g = scale*g;
            if(g>255) g = 255;
            b = scale*b;
            if(b>255) b = 255;
        }
        *optr = CPACK(r, g, b, 255);
        iptr++;
        bptr++;
        optr++;
    }
    return out;
}

float canvas_diameter(canvas *c)
{
    return sqrt(c->sizex*c->sizex + c->sizey*c->sizey);
}

canvas *canvas_resize(canvas *in, int sizex, int sizey)
{
    canvas *out = canvas_new(sizex, sizey);
    stbir_resize_uint8((unsigned char *) in->data,  in->sizex,  in->sizey, 0,
                       (unsigned char *)out->data, out->sizex, out->sizey, 0, 4);
    return out;
}

canvas *canvas_blur(canvas *in, float smalldiam) 
{
    float indiam = canvas_diameter(in);
    float scaledown = smalldiam/indiam;
    int smallsizex = round(scaledown*in->sizex);
    int smallsizey = round(scaledown*in->sizey);
    canvas *small = canvas_resize(in, smallsizex, smallsizey); 
    canvas *big = canvas_resize(small, in->sizex, in->sizey);
    canvas_free(small);
    return big;
}

canvas *canvas_frompng(const char *filename)
{
    int sizex, sizey, n;
    unsigned char *data = stbi_load(filename, &sizex, &sizey, &n, 0);
    if(!data) {
        fprintf(stderr, "canvas_frompng: error: problem reading %s\n", filename);
        exit(1);
    }
    canvas *pic = canvas_new(sizex, sizey);
    free(pic->data);
    pic->data = (unsigned int *)data;
    return pic;
}

void canvas_topng(canvas *in, const char *filename)
{
    stbi_write_png(filename, in->sizex, in->sizey, 4, in->data, 4*in->sizex);
}

canvas *canvas_enlighten(canvas *in, float param)
{
    // make a b/w image that has max of [r, g, b] of the input
    canvas *maxrgb = canvas_maxrgb(in);

    // blur this image. can do more or less - try 10.0 try 40.0 up to you
    canvas *maxrgbblur = canvas_blur(maxrgb, 20.0);
    canvas_free(maxrgb);

    // divide the input image by maxrgbblur to brighten the image
    // param is in the range 0.0 .. 1.0 and normally controlled by a slider.
    canvas *ret = canvas_brighten(in, maxrgbblur, param);
    canvas_free(maxrgbblur);
    return ret;
}

int main(int argc, char **argv) 
{ 
    if(argc<4) {
        fprintf(stderr, "usage: enlighten in.png out.png param\n");
        exit(1);
    }
    float param = atof(argv[3]);
    canvas *in = canvas_frompng(argv[1]);
    canvas *out = canvas_enlighten(in, param);
    canvas_free(in);
    canvas_topng(out, argv[2]);
    canvas_free(out);
    return 0;
}
