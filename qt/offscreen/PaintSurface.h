
#ifndef PAINTSURFACE_H
#define PAINTSURFACE_H

#include "OpenGLOffscreenSurface.h"

class PaintSurface : public OpenGlOffscreenSurface
{
public:
    explicit PaintSurface(QScreen* targetScreen = nullptr, const QSize& size = QSize (1, 1));
    virtual ~PaintSurface() override;

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int width,int height) override;
    virtual void paintGL() override;
};


#endif  // PAINTSURFACE_H
