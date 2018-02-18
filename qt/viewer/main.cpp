
#include "main.h"
#include "thread.h"


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
