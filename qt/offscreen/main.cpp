//
// https://stackoverflow.com/questions/31323749/easiest-way-for-offscreen-rendering-with-qopenglwidget
// http://doc.qt.io/qt-5/qoffscreensurface.html
//


#include <QApplication>
#include "PaintSurface.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    PaintSurface paintSurface;
    paintSurface.resize(300, 200);
    paintSurface.render();
    QImage image = paintSurface.grabFramebuffer();
    image.save(QString("image.png"));

    return a.exec();
}
