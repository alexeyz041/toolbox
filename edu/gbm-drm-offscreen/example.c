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
#include <EGL/eglext.h>
#include <GL/gl.h>
#include <gbm.h>

int fd;
struct gbm_device *gbm;
struct gbm_surface *gbmSurface;

EGLDisplay eglDisplay;
EGLConfig eglConfig;
EGLContext eglContext;
EGLSurface eglSurface;


int renderBufferWidth = 1920;
int renderBufferHeight = 1080;

#ifndef EGL_EXT_platform_base
typedef EGLDisplay (EGLAPIENTRYP PFNEGLGETPLATFORMDISPLAYEXTPROC) (EGLenum platform, void *native_display, const EGLint *attrib_list);
typedef EGLSurface (EGLAPIENTRYP PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC) (EGLDisplay dpy, EGLConfig config, void *native_window, const EGLint *attrib_list);
#endif



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


#define	NELEM(X)	(sizeof(X)/sizeof(X[0]))

typedef struct {
    EGLint attrib;
    const char *desc;
} ATTR_DESC;


ATTR_DESC attrs[] = {
    { EGL_ALPHA_SIZE, "EGL_ALPHA_SIZE" },
    { EGL_ALPHA_MASK_SIZE, "EGL_ALPHA_MASK_SIZE" },
    { EGL_BIND_TO_TEXTURE_RGB, "EGL_BIND_TO_TEXTURE_RGB" },
    { EGL_BIND_TO_TEXTURE_RGBA, "EGL_BIND_TO_TEXTURE_RGBA" },
    { EGL_BLUE_SIZE, "EGL_BLUE_SIZE" },
    { EGL_BUFFER_SIZE, "EGL_BUFFER_SIZE" },
    { EGL_COLOR_BUFFER_TYPE, "EGL_COLOR_BUFFER_TYPE" },
    { EGL_CONFIG_CAVEAT, "EGL_CONFIG_CAVEAT" },
    { EGL_CONFIG_ID, "EGL_CONFIG_ID" },
    { EGL_CONFORMANT, "EGL_CONFORMANT" },
    { EGL_DEPTH_SIZE, "EGL_DEPTH_SIZE" },
    { EGL_GREEN_SIZE, "EGL_GREEN_SIZE" },
    { EGL_LEVEL, "EGL_LEVEL" },
    { EGL_LUMINANCE_SIZE, "EGL_LUMINANCE_SIZE" },
    { EGL_MAX_PBUFFER_WIDTH, "EGL_MAX_PBUFFER_WIDTH" },
    { EGL_MAX_PBUFFER_HEIGHT, "EGL_MAX_PBUFFER_HEIGHT" },
    { EGL_MAX_PBUFFER_PIXELS, "EGL_MAX_PBUFFER_PIXELS" },
    { EGL_MAX_SWAP_INTERVAL, "EGL_MAX_SWAP_INTERVAL" },
    { EGL_MIN_SWAP_INTERVAL, "EGL_MIN_SWAP_INTERVAL" },
    { EGL_NATIVE_RENDERABLE, "EGL_NATIVE_RENDERABLE" },
    { EGL_NATIVE_VISUAL_ID, "EGL_NATIVE_VISUAL_ID" },
    { EGL_NATIVE_VISUAL_TYPE, "EGL_NATIVE_VISUAL_TYPE" },
    { EGL_RED_SIZE, "EGL_RED_SIZE" },
    { EGL_RENDERABLE_TYPE, "EGL_RENDERABLE_TYPE" },
    { EGL_SAMPLE_BUFFERS, "EGL_SAMPLE_BUFFERS" },
    { EGL_SAMPLES, "EGL_SAMPLES" },
    { EGL_STENCIL_SIZE, "EGL_STENCIL_SIZE" },
    { EGL_SURFACE_TYPE, "EGL_SURFACE_TYPE" },
    { EGL_TRANSPARENT_TYPE, "EGL_TRANSPARENT_TYPE" },
    { EGL_TRANSPARENT_RED_VALUE, "EGL_TRANSPARENT_RED_VALUE" },
    { EGL_TRANSPARENT_GREEN_VALUE, "EGL_TRANSPARENT_GREEN_VALUE" },
    { EGL_TRANSPARENT_BLUE_VALUE, "EGL_TRANSPARENT_BLUE_VALUE" }
};


void list_config(EGLConfig config)
{
    printf("config %p\n",config);
    for(int i=0; i < NELEM(attrs); i++) {
        EGLint val = 0;
        if (!eglGetConfigAttrib(eglDisplay, config, attrs[i].attrib, &val)) {
	    fprintf(stderr,"eglGetConfigAttrib failed\n");
            exit(1);
        }
	printf("%s %x\n",attrs[i].desc,val);
    }
    printf("---\n");
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


//#define DEV "/dev/dri/card0"
#define DEV "/dev/dri/renderD128"




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

    PFNEGLGETPLATFORMDISPLAYEXTPROC getPlatformDisplay = NULL;
    getPlatformDisplay = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    if (!getPlatformDisplay) {
	fprintf(stderr,"can\'t get display fn\n");
        exit(1);
    }

    eglDisplay = getPlatformDisplay(EGL_PLATFORM_GBM_KHR, gbm, NULL);
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

    fprintf(stderr,"EGL Version \"%s\"\n", eglQueryString(eglDisplay, EGL_VERSION));
    fprintf(stderr,"EGL Vendor \"%s\"\n", eglQueryString(eglDisplay, EGL_VENDOR));
    fprintf(stderr,"EGL Extensions \"%s\"\n", eglQueryString(eglDisplay, EGL_EXTENSIONS));

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
//    if (!eglBindAPI(EGL_OPENGL_API)) {
	fprintf(stderr,"failed to bind api EGL_OPENGL_ES_API\n");
	exit(1);
    }
}





void get_config()
{
    EGLint egl_config_attribs[] = {
        EGL_BUFFER_SIZE,        24,
        EGL_DEPTH_SIZE,         EGL_DONT_CARE,
        EGL_STENCIL_SIZE,       EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE,    EGL_OPENGL_ES3_BIT, //EGL_OPENGL_BIT, //EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,       EGL_WINDOW_BIT,	//EGL_PBUFFER_BIT
        EGL_NONE
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

//    for (int i = 0; i < num_configs; ++i) {
//	list_config(configs[i]);
//    }

    // Find a config whose native visual ID is the desired GBM format.
    for (int i = 0; i < num_configs; ++i) {
        EGLint gbm_format;
        if (!eglGetConfigAttrib(eglDisplay, configs[i], EGL_NATIVE_VISUAL_ID, &gbm_format)) {
	    fprintf(stderr,"eglGetConfigAttrib failed\n");
            exit(1);
        }
        if (gbm_format == GBM_FORMAT_XRGB8888) {
            eglConfig = configs[i];
	    list_config(configs[i]);
            free(configs);
            return;
        }
    }

    fprintf(stderr,"Failed to find a config with matching GBM format.\n");
    exit(1);
}


void get_window()
{
    EGLint attributes[] = {
	EGL_RED_SIZE, 8,
	EGL_GREEN_SIZE, 8,
	EGL_BLUE_SIZE, 8,
	EGL_NONE
    };

    EGLint num_config=0;
    eglChooseConfig (eglDisplay, attributes, &eglConfig, 1, &num_config);

    eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, NULL);

    gbmSurface = gbm_surface_create(gbm, renderBufferWidth, renderBufferHeight, 
                                    GBM_FORMAT_XRGB8888,
                                    GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT);
    if (!gbmSurface) {
	fprintf(stderr,"gbm_surface_create failed\n");
        exit(1);
    }

    PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC createPlatformWindowSurface = NULL;
    createPlatformWindowSurface = (PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC)eglGetProcAddress("eglCreatePlatformWindowSurfaceEXT");
    if (!createPlatformWindowSurface) {
	fprintf(stderr,"can\'t get display fn\n");
        exit(1);
    }
    
    eglSurface = createPlatformWindowSurface(eglDisplay,eglConfig,gbmSurface,NULL);
    if (eglSurface == EGL_NO_SURFACE) {
	fprintf(stderr,"eglCreateWindowSurface failed\n");
        exit(1);
    }

//    EGLint attrs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
//    eglContext = eglCreateContext (eglDisplay, eglConfig,  EGL_NO_CONTEXT, attrs);
    if(eglContext == EGL_NO_CONTEXT) {
	fprintf(stderr,"eglCreateContext failed\n");
	exit(1);
    }

    if(eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) == EGL_FALSE) {
	fprintf(stderr,"eglMakeCurrent failed %x\n",glGetError());
	exit(1);
    }
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
//    get_config();
    get_window();

    glClearColor(1.0, 0.0, 0.0, 1.0);
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

