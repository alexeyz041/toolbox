
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>

#include "thread.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    RenderThread *rt = new RenderThread();
    rt->start();

    return app.exec();
}

