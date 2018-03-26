
#include "thread.h"
#include "gst.h"

#include <QDebug>


void RenderThread::shot(uchar *buf,int size)
{
    if(quit) return;
    QPixmap orig = screen->grabWindow(0);
    QRect rect(0, 0, 800, 600);
    QPixmap cropped = orig.copy(rect);
    QImage img = cropped.toImage();
    memcpy(buf,img.bits(),size);
}


extern "C" void get_image(void *ctx,void *buf,int size)
{
RenderThread *p = (RenderThread *)ctx;

    p->shot((uchar *)buf,size);
}


void RenderThread::run()
{
    gst_main(this,&config);
}


void RenderThread::stop()
{
    quit = true;
}

