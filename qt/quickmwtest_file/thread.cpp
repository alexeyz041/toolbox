#include "thread.h"

#include <QDebug>


void RenderThread::shot()
{
    fprintf(stderr,"saving screenshot...\n");
    QPixmap orig = screen->grabWindow(0);
    QRect rect(0, 0, 800, 600);
    QPixmap cropped = orig.copy(rect);
    QImage img = cropped.toImage();
    img.save("screen.png", "PNG");
    fprintf(stderr,"done\n");
}


void RenderThread::run()
{
    usleep(1000*1000);
    shot();
}


void RenderThread::stop()
{
    quit = true;
}




