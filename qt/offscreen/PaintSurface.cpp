
#include "PaintSurface.h"
#include <QPainter>


PaintSurface::PaintSurface(QScreen *targetScreen,const QSize& size)
    : OpenGlOffscreenSurface(targetScreen, size)
{
}

PaintSurface::~PaintSurface()
{
}


void PaintSurface::initializeGL()
{
}


void PaintSurface::resizeGL(int width, int height)
{
}


void PaintSurface::paintGL()
{
    QPainter painter(getPaintDevice());
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    painter.drawText(20, 40, "Test"); // <-- drawing here

    painter.end();
}
