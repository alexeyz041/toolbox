
#include "main.h"
#include "thread.h"

#include <time.h>

// call this function to start a nanosecond-resolution timer
struct timespec timer_start()
{
    struct timespec start_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time)
{
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

void GraphicsScene::update1(uint8_t *buf,int len)
{
	struct timespec start = timer_start();

	QPixmap img;
   	if(img.loadFromData(buf,len)) {
       	//printf("%d x %d\n",img.width(),img.height());
   	}
   	((QGraphicsPixmapItem *)items()[0])->setPixmap(img);

   	long delta = timer_end(start)/100000;
   	printf("png %ld.%d\n",delta/10,delta%10);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GraphicsScene scene;
    //QGraphicsScene scene;
    QGraphicsView view(&scene);
    QGraphicsPixmapItem item(QPixmap("test.png"));
    scene.addItem(&item);

    RenderThread *rt = new RenderThread();
    rt->start();

    QObject::connect(rt, SIGNAL(received(uint8_t *, int)), &scene, SLOT(update1(uint8_t *, int)));
    QObject::connect(&app, SIGNAL(aboutToQuit()), rt, SLOT(stop()));

    view.show();
    return app.exec();
}
