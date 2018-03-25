// https://stackoverflow.com/questions/28491665/how-to-egl-offscreen-render-to-an-image-on-linux
//
// https://www.khronos.org/registry/EGL/sdk/docs/man/html/indexflat.php
//

#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <GLES2/gl2.h>
#include <EGL/egl.h>


int main(int argc, char *argv[])
{
    #define CONTEXT_ES20

    #ifdef CONTEXT_ES20
        EGLint ai32ContextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2,
                                                     EGL_NONE };
    #endif

    // Step 1 - Get the default display.
    EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)0);

    // Step 2 - Initialize EGL.
    eglInitialize(eglDisplay, 0, 0);

    #ifdef CONTEXT_ES20
    // Step 3 - Make OpenGL ES the current API.
    eglBindAPI(EGL_OPENGL_ES_API);

    // Step 4 - Specify the required configuration attributes.
    EGLint pi32ConfigAttribs[5];
    pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
    pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
    pi32ConfigAttribs[2] = EGL_RENDERABLE_TYPE;
    pi32ConfigAttribs[3] = EGL_OPENGL_ES2_BIT;
    pi32ConfigAttribs[4] = EGL_NONE;
    #else
    EGLint pi32ConfigAttribs[3];
    pi32ConfigAttribs[0] = EGL_SURFACE_TYPE;
    pi32ConfigAttribs[1] = EGL_WINDOW_BIT;
    pi32ConfigAttribs[2] = EGL_NONE;
    #endif

    // Step 5 - Find a config that matches all requirements.
    int iConfigs;
    EGLConfig eglConfig;
    eglChooseConfig(eglDisplay, pi32ConfigAttribs, &eglConfig, 1,
                                                    &iConfigs);

    if (iConfigs != 1) {
        printf("Error: eglChooseConfig(): config not found.\n");
        exit(-1);
    }

    // Step 6 - Create a surface to draw to.
    EGLSurface eglSurface;
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig,
                                  (EGLNativeWindowType)NULL, NULL);

    // Step 7 - Create a context.
    EGLContext eglContext;
    #ifdef CONTEXT_ES20
        eglContext = eglCreateContext(eglDisplay, eglConfig, NULL,
                                               ai32ContextAttribs);
    #else
        eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, NULL);
    #endif

    // Step 8 - Bind the context to the current thread
    eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);

    GLuint fboId = 0;
    GLuint renderBufferWidth = 1280;
    GLuint renderBufferHeight = 720;

    // create a framebuffer object
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, fboId);

    // create a texture object
    /*  GLuint textureId;
     glGenTextures(1, &textureId);
     glBindTexture(GL_TEXTURE_2D, textureId);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                             
     //GL_LINEAR_MIPMAP_LINEAR
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
     glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
     glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_HINT, GL_TRUE); // automatic mipmap
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderBufferWidth, renderBufferHeight, 0,
                  GL_RGB, GL_UNSIGNED_BYTE, 0);
     glBindTexture(GL_TEXTURE_2D, 0);
     // attach the texture to FBO color attachment point
     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, textureId, 0);
     */
     qDebug() << glGetError();
     GLuint renderBuffer;
     glGenRenderbuffers(1, &renderBuffer);
     glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
     qDebug() << glGetError();
     glRenderbufferStorage(GL_RENDERBUFFER,
                           GL_RGB565,
                           renderBufferWidth,
                           renderBufferHeight);
     qDebug() << glGetError();
     glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0,
                               GL_RENDERBUFFER,
                               renderBuffer);

      qDebug() << glGetError();
      GLuint depthRenderbuffer;
      glGenRenderbuffers(1, &depthRenderbuffer);
      glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,     renderBufferWidth, renderBufferHeight);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

      // check FBO status
      GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
      if(status != GL_FRAMEBUFFER_COMPLETE) {
          printf("Problem with OpenGL framebuffer after specifying color render buffer: \n%x\n", status);
      } else {
          printf("FBO creation succedded\n");
  }

  glClearColor(1.0, 0.5, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  qDebug() << eglSwapBuffers(   eglDisplay, eglSurface);
  int size = 4 * renderBufferHeight * renderBufferWidth;
  printf("print size");
  printf("size %d", size);
  qDebug() << size;

  unsigned char *data2 = new unsigned char[size];

  glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA,GL_UNSIGNED_BYTE, data2);

  QImage image(data2, renderBufferWidth,  renderBufferHeight, renderBufferWidth*4, QImage::Format_RGB32);

  image.save("result.png");


  qDebug() << "done";


  QCoreApplication a(argc, argv);

  return a.exec();
}
