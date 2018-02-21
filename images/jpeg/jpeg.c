
#include <stdio.h>
#include <stdlib.h>
#include "jpeglib.h"
#include <setjmp.h>
#include <string.h>
#include "bitmap.h"
#include <stdint.h>


void write_JPEG_file(char *filename, int quality, int image_width, int image_height, JSAMPLE *image_buffer)
{
  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * outfile;
  JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
  int row_stride;			/* physical row width in image buffer */

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  if ((outfile = fopen(filename, "wb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  cinfo.image_width = image_width; 	/* image width and height, in pixels */
  cinfo.image_height = image_height;
  cinfo.input_components = 3;		/* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);
  jpeg_start_compress(&cinfo, TRUE);

  row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */
  while (cinfo.next_scanline < cinfo.image_height) {
    row_pointer[0] = &image_buffer[(cinfo.image_height-cinfo.next_scanline-1) * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  jpeg_finish_compress(&cinfo);
  fclose(outfile);
  jpeg_destroy_compress(&cinfo);
}



struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */
  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

void my_error_exit (j_common_ptr cinfo)
{
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}


uint8_t *read_JPEG_file(const char *filename,int *w,int *h)
{
struct jpeg_decompress_struct cinfo;
struct my_error_mgr jerr;
FILE * infile;
uint8_t *buffer;	/* Output row buffer */
int row_stride;		/* physical row width in output buffer */

    if ((infile = fopen(filename, "rb")) == NULL) {
	fprintf(stderr, "can't open %s\n", filename);
	return NULL;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
	return NULL;
    }
    jpeg_create_decompress(&cinfo);

    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);

    *w = cinfo.output_width;
    *h = cinfo.output_height;

    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (uint8_t *)calloc(row_stride, cinfo.output_height);
    uint8_t *ptr = buffer+row_stride*(cinfo.output_height-1);
    while (cinfo.output_scanline < cinfo.output_height) {
	(void) jpeg_read_scanlines(&cinfo, &ptr, 1);
	ptr -= row_stride;
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return buffer;
}



int endsWith(const char *s,const char *pat)
{
int len = strlen(s);
int patlen = strlen(pat);
    if(patlen > len) return 0;
    return strncasecmp(s+len-patlen,pat,patlen) == 0;
}


int main(int argc,char **argv)
{
    if(argc < 3) {
	fprintf(stderr,"Usage:\n");
	fprintf(stderr," bmp2jpeg <image.jpg> <image.bmp>\n");
	fprintf(stderr,"or\n");
	fprintf(stderr," bmp2jpeg <image.bmp> <image.jpg>\n");
	return 1;
    }

    if(endsWith(argv[1],".bmp")) {
	BITMAPINFO *info=NULL;
	uint8_t *p = LoadDIBitmap(argv[1],&info);
	if(p) {
	    printf("%dx%d:%d\n", info->bmiHeader.biWidth, info->bmiHeader.biHeight, info->bmiHeader.biBitCount);
	    write_JPEG_file(argv[2],95,info->bmiHeader.biWidth,info->bmiHeader.biHeight,p);
	    free(p);
	} else {
	    fprintf(stderr,"can\'t load %s\n",argv[1]);
	}
    } else if(endsWith(argv[1],".jpg")) {
	int w = 0;
	int h = 0;
	int bcnt = 0;
	uint8_t *p = read_JPEG_file(argv[1],&w,&h);
	if(p) {
	    BITMAPINFO info;
	    memset(&info,0,sizeof(info));
	    info.bmiHeader.biWidth = w;
	    info.bmiHeader.biHeight = h;
	    info.bmiHeader.biPlanes = 1;
	    info.bmiHeader.biBitCount = 24;
	    SaveDIBitmap(argv[2],&info,p);
	    free(p);
	} else {
	    fprintf(stderr,"can\'t load %s\n",argv[1]);
	}
    }

    return 0;
}
