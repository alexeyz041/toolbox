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

#include <QCoreApplication>
#include <QDebug>
#include <QImage>

#include <GLES2/gl2.h>
#include <EGL/egl.h>


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
         qDebug() << "Error compiling shader: " << infoLog ;
         
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
         qDebug() << "Error linking program: " << infoLog;
         
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
    unsigned char *data2 = new unsigned char[size];

    // Step 11 - clear the screen in Red and read it back
    glClearColor(1.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint programObject;
    init(&programObject);
    draw(programObject);

    eglSwapBuffers( eglDisplay, eglSurface);
    glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA, GL_UNSIGNED_BYTE, data2);

    QImage image(data2, renderBufferWidth,  renderBufferHeight, renderBufferWidth*4, QImage::Format_RGB32);
    image.mirrored().rgbSwapped().save("result.png");
    qDebug() << "done";
//    QCoreApplication a(argc, argv);
    return 0; //a.exec();
}

