/*
 * Windows BMP file definitions for OpenGL.
 *
 * Written by Michael Sweet.
 */

#ifndef _BITMAP_H_
#define _BITMAP_H_

/*
 * Make this header file work with C and C++ source code...
 */

#  ifdef __cplusplus
extern "C" {
#  endif /* __cplusplus */

typedef unsigned char GLubyte;

/*
 * Bitmap file data structures (these are defined in <wingdi.h> under
 * Windows...)
 *
 * Note that most Windows compilers will pack the following structures, so
 * when reading them under MacOS or UNIX we need to read individual fields
 * to avoid differences in alignment...
 */

typedef struct                       /**** BMP file header structure ****/
    {
    unsigned short bfType;           /* 2 Magic number for file */
    unsigned int   bfSize;           /* 4 Size of file */
    unsigned short bfReserved1;      /* 2 Reserved */
    unsigned short bfReserved2;      /* 2 ... */
    unsigned int   bfOffBits;        /* 4 Offset to bitmap data */
    } BITMAPFILEHEADER;

# define BF_TYPE 0x4D42             /* "MB" */

typedef struct                       /**** BMP file info structure ****/
    {
    unsigned int   biSize;           /* Size of info header */
    int            biWidth;          /* Width of image */
    int            biHeight;         /* Height of image */
    unsigned short biPlanes;         /* Number of color planes */
    unsigned short biBitCount;       /* Number of bits per pixel */
    unsigned int   biCompression;    /* Type of compression to use */
    unsigned int   biSizeImage;      /* Size of image data */
    int            biXPelsPerMeter;  /* X pixels per meter */
    int            biYPelsPerMeter;  /* Y pixels per meter */
    unsigned int   biClrUsed;        /* Number of colors used */
    unsigned int   biClrImportant;   /* Number of important colors */
    } BITMAPINFOHEADER;

/*
 * Constants for the biCompression field...
 */

#  define BI_RGB       0             /* No compression - straight BGR data */
#  define BI_RLE8      1             /* 8-bit run-length compression */
#  define BI_RLE4      2             /* 4-bit run-length compression */
#  define BI_BITFIELDS 3             /* RGB bitmap with RGB masks */

typedef struct                       /**** Colormap entry structure ****/
    {
    unsigned char  rgbBlue;          /* Blue value */
    unsigned char  rgbGreen;         /* Green value */
    unsigned char  rgbRed;           /* Red value */
    unsigned char  rgbReserved;      /* Reserved */
    } RGBQUAD;

typedef struct                       /**** Bitmap information structure ****/
    {
    BITMAPINFOHEADER bmiHeader;      /* Image header */
    RGBQUAD          bmiColors[256]; /* Image colormap */
    } BITMAPINFO;


/*
 * Prototypes...
 */

extern GLubyte *LoadDIBitmap(const char *filename, BITMAPINFO **info);
extern int     SaveDIBitmap(const char *filename, BITMAPINFO *info,
                            GLubyte *bits);


#  ifdef __cplusplus
}
#  endif /* __cplusplus */
#endif /* !_BITMAP_H_ */
