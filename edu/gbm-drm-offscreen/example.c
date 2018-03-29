// https://stackoverflow.com/questions/41271580/offscreen-rendering-in-opengl-or-directx
//
// This example program creates an EGL surface from a GBM surface.
//
// If the macro EGL_MESA_platform_gbm is defined, then the program
// creates the surfaces using the methods defined in this specification.
// Otherwise, it uses the methods defined by the EGL 1.4 specification.
//
// Compile with `cc -std=c99 example.c -lgbm -lEGL`.

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libpng/png.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GL/gl.h>
#include <gbm.h>

int fd;
struct gbm_device *gbm;
struct gbm_surface *gbmSurface;

EGLDisplay eglDisplay;
EGLConfig eglConfig;
EGLContext eglContext;
EGLSurface eglSurface;


int renderBufferHeight = 1920;
int renderBufferWidth = 1080;


int save_png(const char *outfile,int xres,int yres,int bits_per_pixel,uint8_t *buffer)
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

    png_set_IHDR(png, info,
        xres, yres, 8 /*bits_per_pixel*/, 
        (bytespp == 3) ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGBA,
    PNG_INTERLACE_NONE,
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


void check_extensions(void)
{
    const char *client_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!client_extensions) {
        fprintf(stderr,"EGL_EXT_client_extensions is unsupported.\n");
        exit(1);
    }
    printf("Available extensions: %s\n",client_extensions);
    if (!strstr(client_extensions, "EGL_MESA_platform_gbm")) {
        exit(1);
    }
}


#define DEV "/dev/dri/card0"

void get_display(void)
{
    fd = open(DEV, O_RDWR | FD_CLOEXEC);
    if(fd < 0) {
	fprintf(stderr,"can\'t open %s\n",DEV);
        exit(1);
    }

    gbm = gbm_create_device(fd);
    if(!gbm) {
	fprintf(stderr,"can\'t create gbm device for %s\n",DEV);
        exit(1);
    }

#ifdef EGL_MESA_platform_gbm
    eglDisplay = eglGetPlatformDisplayEXT(EGL_PLATFORM_GBM_MESA, gbm, NULL);
#else
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)gbm);
#endif

    if(eglDisplay == EGL_NO_DISPLAY) {
	fprintf(stderr,"can\'t create display for %s\n",DEV);
        exit(1);
    }

    EGLint major, minor;
    if (!eglInitialize(eglDisplay, &major, &minor)) {
	fprintf(stderr,"can\'t initialize\n");
        exit(1);
    }

    printf("major = %d, minor = %d\n",major,minor);
}



void get_config()
{
    EGLint egl_config_attribs[] = {
        EGL_BUFFER_SIZE,        32,
        EGL_DEPTH_SIZE,         EGL_DONT_CARE,
        EGL_STENCIL_SIZE,       EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,       EGL_PBUFFER_BIT, //EGL_WINDOW_BIT,
        EGL_NONE,
    };

    EGLint num_configs = 0;
    if (!eglGetConfigs(eglDisplay, NULL, 0, &num_configs)) {
	fprintf(stderr,"eglGetConfigs failed\n");
        exit(1);
    }

    EGLConfig *configs = malloc(num_configs * sizeof(EGLConfig));
    if (!eglChooseConfig(eglDisplay, egl_config_attribs, configs, num_configs, &num_configs)) {
	fprintf(stderr,"eglChooseConfig failed\n");
        exit(1);
    }
    if (num_configs == 0) {
	fprintf(stderr,"no configs found\n");
        exit(1);
    }

    // Find a config whose native visual ID is the desired GBM format.
    for (int i = 0; i < num_configs; ++i) {
        EGLint gbm_format;
        if (!eglGetConfigAttrib(eglDisplay, configs[i], EGL_NATIVE_VISUAL_ID, &gbm_format)) {
	    fprintf(stderr,"eglGetConfigAttrib failed\n");
            exit(1);
        }
        if (gbm_format == GBM_FORMAT_XRGB8888) {
            eglConfig = configs[i];
            free(configs);
            return;
        }
    }

    fprintf(stderr,"Failed to find a config with matching GBM format.\n");
    exit(1);
}


void get_window()
{
    gbmSurface = gbm_surface_create(gbm, renderBufferHeight, renderBufferWidth,
                                    GBM_FORMAT_XRGB8888,
                                    GBM_BO_USE_RENDERING);
    if (!gbmSurface) {
	fprintf(stderr,"gbm_surface_create failed\n");
        exit(1);
    }

#ifdef EGL_MESA_platform_gbm
    eglSurface = eglCreatePlatformWindowSurfaceEXT(eglDisplay,eglConfig,gbmSurface,NULL);
#else
    eglSurface = eglCreateWindowSurface(eglDisplay,eglConfig,(EGLNativeWindowType)gbmSurface,NULL);
#endif

    if (eglSurface == EGL_NO_SURFACE) {
	fprintf(stderr,"eglCreateWindowSurface failed\n");
        exit(1);
    }

    eglContext = eglCreateContext (eglDisplay, eglConfig, EGL_NO_CONTEXT, NULL);
}


void cleanup()
{
    eglDestroySurface(eglDisplay, eglSurface);
    eglDestroyContext(eglDisplay, eglContext);
    eglTerminate(eglDisplay);
    gbm_surface_destroy(gbmSurface);
    gbm_device_destroy(gbm);
    close(fd);
}


int main(void)
{
    check_extensions();

    get_display();
    get_config();
    get_window();

    glClearColor(1.0f,0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(eglDisplay, eglSurface);

    int size = 4 * renderBufferHeight * renderBufferWidth;
    unsigned char *data = malloc(size);
    if(!data) {
	fprintf(stderr,"out of memory\n");
	exit(1);
    }

    glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA, GL_UNSIGNED_BYTE, data);
    save_png("screen.png",renderBufferWidth,renderBufferHeight,32,data);
    free(data);

    cleanup();
    return 0;
}

