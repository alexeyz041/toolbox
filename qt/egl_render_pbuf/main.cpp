// https://stackoverflow.com/questions/28817777/pbuffer-vs-fbo-in-egl-offscreen-rendering
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
    eglSwapBuffers( eglDisplay, eglSurface);
    glReadPixels(0,0,renderBufferWidth,renderBufferHeight,GL_RGBA, GL_UNSIGNED_BYTE, data2);

    QImage image(data2, renderBufferWidth,  renderBufferHeight, renderBufferWidth*4, QImage::Format_RGB32);
    image.save("result.png");
    qDebug() << "done";
    QCoreApplication a(argc, argv);
    return a.exec();
}
