//
// Rendering with EGL ES2 into pbuffer surface
//
// based on 
// https://stackoverflow.com/questions/28817777/pbuffer-vs-fbo-in-egl-offscreen-rendering
//
// https://www.khronos.org/registry/EGL/sdk/docs/man/html/indexflat.php
// https://www.khronos.org/assets/uploads/books/openglr_es_20_programming_guide_sample.pdf
// https://github.com/adobe/angle/blob/master/samples/gles2_book/Hello_Triangle/Hello_Triangle.c
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <libpng/png.h>
#include <errno.h>

///
// Create a shader object, load the shader source, and
// compile the shader.
//
GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;
 
   // Create the shader object
   shader = glCreateShader ( type );

   if ( shader == 0 )
    return 0;

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );
   
   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled ) 
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char *)malloc (sizeof(char) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         fprintf(stderr, "Error compiling shader: %s\n", infoLog);
         
         free ( infoLog );
      }

      glDeleteShader ( shader );
      return 0;
   }

   return shader;
}


int init(GLuint *programObjectPtr)
{
const char vShaderStr[] =
      "attribute vec4 vPosition;    \n"
      "void main()                  \n"
      "{                            \n"
      "   gl_Position = vPosition;  \n"
      "}                            \n";
 
const char fShaderStr[] =
      "precision mediump float;\n"\
      "void main()                                  \n"
      "{                                            \n"
      "  gl_FragColor = vec4 ( 1.0, 1.0, 1.0, 1.0 );\n"
      "}                                            \n";

   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
   fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

   // Create the program object
   programObject = glCreateProgram ( );
   
   if ( programObject == 0 )
      return 0;

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Bind vPosition to attribute 0   
   glBindAttribLocation ( programObject, 0, "vPosition" );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked ) 
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );
      
      if ( infoLen > 1 )
      {
         char* infoLog = (char *)malloc (sizeof(char) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         fprintf(stderr, "Error linking program: %s\n",infoLog);
         
         free ( infoLog );
      }

      glDeleteProgram ( programObject );
      return 0;
   }

   // Store the program object
   *programObjectPtr = programObject;

   glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );
   return 1;
}


void draw(GLuint programObject)
{
    GLfloat vVertices[] = {  0.0f,  0.5f, 0.0f, 
                       -0.5f, -0.5f, 0.0f,
                        0.5f, -0.5f, 0.0f };

   // Set the viewport
   glViewport ( 0, 0, 1920, 1080 );
 
   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( programObject );

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   glEnableVertexAttribArray ( 0 );

    glDrawArrays ( GL_TRIANGLES, 0, 3 );
}


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



int main(int argc, char *argv[])
{
    EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,
                                                     EGL_NONE };
    // Step 1 - Get the default display.
    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);

    // Step 2 - Initialize EGL.
    eglInitialize(eglDisplay, 0, 0);

    // Step 3 - Make OpenGL ES the current API.
    eglBindAPI(EGL_OPENGL_ES_API);

    // Step 4 - Specify the required configuration attributes.
    EGLint pi32ConfigAttribs[5];
    pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
    pi32ConfigAttribs[1] = EGL_PBUFFER_BIT;
    pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
    pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
    pi32ConfigAttribs[4] = EGL_NONE;

    // Step 5 - Find a config that matches all requirements.
    int iConfigs;
    EGLConfig eglConfig;
    eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1, &iConfigs);
    if (iConfigs != 1) {
        printf("Error: eglChooseConfig(): config not found.\n");
        exit(-1);
    }

    int renderBufferWidth = 1920;
    int renderBufferHeight = 1080;

    // Step 6 - Create a surface to draw to.
    EGLint pbufferAttribs[5];
    pbufferAttribs[0] = EGL_WIDTH;
    pbufferAttribs[1] = renderBufferWidth;
    pbufferAttribs[2] = EGL_HEIGHT;
    pbufferAttribs[3] = renderBufferHeight;
    pbufferAttribs[4] = EGL_NONE;

    EGLSurface eglSurface;
    eglSurface = eglCreatePbufferSurface(eglDisplay, eglConfig, pbufferAttribs);

    // Step 7 - Create a context.
    EGLContext eglContext;
    eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, ai32ContextAttribs);

    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    int size = 4 * renderBufferHeight * renderBufferWidth;
    unsigned char *data2 = malloc(size);
    if(!data2) {
	fprintf(stderr,"out of memory\n");
	exit(1);
    }

    // Step 11 - clear the screen in Red and read it back
    glClearColor(1.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint programObject;
    init(&programObject);
    draw(programObject);

    eglSwapBuffers( eglDisplay, eglSurface);
    glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA, GL_UNSIGNED_BYTE, data2);

    save_png("screen.png",renderBufferWidth,renderBufferHeight,32,data2);

    free(data2);
    fprintf(stderr,"done\n");
    return 0;
}

