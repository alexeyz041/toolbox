// png <-> bmp converter
// based on https://gist.github.com/niw/5963798

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <libpng/png.h>
#include "bitmap.h"


int save_png(const char *outfile,int xres,int yres,int bits_per_pixel,uint8_t *buffer,png_colorp palette)
{
    FILE *f = fopen(outfile, "w");
    if (!f) {
        fprintf(stderr, "error: writing file %s: %s\n", outfile, strerror(errno));
        return -1;
    }
    
    png_structp png;
    png_infop info;
    unsigned int r,rowlen;
    unsigned int bytespp;

    png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr,"failed png_create_write_struct\n");
        return -1;
    }

    png_init_io(png, f);
    info = png_create_info_struct(png);
    if (!info) {
	fprintf(stderr,"failed png_create_info_struct\n");
        png_destroy_write_struct(&png, NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png))) {
        fprintf(stderr,"failed png setjmp\n");
        png_destroy_write_struct(&png, NULL);
        return -1;
    }

    bytespp = bits_per_pixel / 8;

    if(bytespp == 1)
	png_set_PLTE(png, info, palette, 256);

    png_set_IHDR(png, info,
        xres, yres, 8 /*bits_per_pixel*/, 
        (bytespp == 3) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
    png_write_info(png, info);


    rowlen = xres * bytespp;

    for(r=0; r < yres; r++) {
        png_write_row(png, (png_bytep)(buffer+rowlen*(yres-r-1)));
    }

    png_write_end(png, info);
    png_destroy_write_struct(&png, NULL);
    return 0;
}


uint8_t *load_png(char* file_name,int *width,int *height,int *bcnt,png_colorp plt)
{
    uint8_t header[8] = {0};    // 8 is the maximum size that can be checked
    png_structp png_ptr;
    png_infop info_ptr;

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        fprintf(stderr,"File %s could not be opened for reading", file_name);
        return NULL;
    }
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        fprintf(stderr,"File %s is not recognized as a PNG file", file_name);
        fclose(fp);
        return NULL;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stderr,"png_create_read_struct failed\n");
        fclose(fp);
        return NULL;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        fprintf(stderr,"png_create_info_struct failed\n");
        fclose(fp);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
    		fprintf(stderr,"Error during init_io");
    		fclose(fp);
    		return NULL;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    *width = png_get_image_width(png_ptr, info_ptr);
    *height = png_get_image_height(png_ptr, info_ptr);
    int color_type = png_get_color_type(png_ptr, info_ptr);
    if(color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_PALETTE) {
    		fprintf(stderr,"wrong color type %d\n",color_type);
    		fclose(fp);
    		return NULL;
    }

    int bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    if(bit_depth != 8) {
    		fprintf(stderr,"wrong bit depth %d\n",bit_depth);
    	    	fclose(fp);
    	    	return NULL;
    }

    int number_of_passes = png_set_interlace_handling(png_ptr);
    if(number_of_passes != 1) {
        	fprintf(stderr,"wrong number of passes %d\n",number_of_passes);
        	fclose(fp);
        	return NULL;
    }
    png_read_update_info(png_ptr, info_ptr);

    int rowlen = png_get_rowbytes(png_ptr,info_ptr);
    uint8_t **row_pointers = (png_bytep*) malloc(*height * sizeof(png_bytep));
    if(!row_pointers) {
    		fclose(fp);
    	    	return NULL;
    }
    uint8_t *p = malloc(*height * rowlen);
    if(!p) {
    		free(row_pointers);
    		fclose(fp);
    		return NULL;
    }
    for(int y=0; y < *height; y++)
        row_pointers[y] = p + rowlen*(*height-y-1);

    png_read_image(png_ptr, row_pointers);

    *bcnt = 24;
    if(color_type == PNG_COLOR_TYPE_PALETTE) {
	*bcnt = 8;
        int num_palette = 0;
	png_colorp palette = NULL;
	if (!png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette)) {
	    fprintf(stderr,"can\'t read palette\n");
	}
	if(num_palette > 256) {
	    fprintf(stderr,"palette too long (%d)\n",num_palette);
	}
	memcpy(plt,palette,num_palette*sizeof(png_color));
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    free(row_pointers);
    return p;
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
		fprintf(stderr," png2bmp <image.png> <image.bmp>\n");
		fprintf(stderr,"or\n");
		fprintf(stderr," png2bmp <image.bmp> <image.png>\n");
		return 1;
	}

	if(endsWith(argv[1],".bmp")) {
		BITMAPINFO *info=NULL;
		uint8_t *p = LoadDIBitmap(argv[1],&info);
		if(p) {
			png_color palette[256] = {0};
			printf("%dx%d:%d\n", info->bmiHeader.biWidth, info->bmiHeader.biHeight, info->bmiHeader.biBitCount);
			if(info->bmiHeader.biBitCount == 8) {
			    for(int i=0; i < 256; i++) {
				palette[i].red = info->bmiColors[i].rgbRed;
				palette[i].green = info->bmiColors[i].rgbGreen;
				palette[i].blue = info->bmiColors[i].rgbBlue;
			    }
			}
			save_png(argv[2],info->bmiHeader.biWidth,info->bmiHeader.biHeight,info->bmiHeader.biBitCount,p,palette);
			free(p);
		} else {
		    fprintf(stderr,"can\'t load %s\n",argv[1]);
		}
	} else if(endsWith(argv[1],".png")) {
		int w = 0;
		int h = 0;
		int bcnt = 0;
		png_color palette[256] = {0};
		uint8_t *p = load_png(argv[1],&w,&h,&bcnt,palette);
		if(p) {
		    BITMAPINFO info;
		    memset(&info,0,sizeof(info));
		    info.bmiHeader.biWidth = w;
		    info.bmiHeader.biHeight = h;
		    info.bmiHeader.biPlanes = 1;
		    info.bmiHeader.biBitCount = bcnt;
		    if(bcnt == 8) {
		    	for(int i=0; i < 256; i++) {
		    		info.bmiColors[i].rgbRed = palette[i].red;
		    		info.bmiColors[i].rgbGreen = palette[i].green;
		    		info.bmiColors[i].rgbBlue = palette[i].blue ;
		    	}
		    	info.bmiHeader.biClrUsed = 256;
		    	info.bmiHeader.biClrImportant = 256;
		    }
		    SaveDIBitmap(argv[2],&info,p);
		    free(p);
		} else {
		    fprintf(stderr,"can\'t load %s\n",argv[1]);
		}
	}

    return 0;
}


