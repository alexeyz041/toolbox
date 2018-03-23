
#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QTimer>


int main(int argc, char **argv)
{
//    qputenv("QT_LOGGING_RULES", "qt.qpa.*=true");
//    qputenv("QSG_INFO", "1");

    QGuiApplication app(argc, argv);

    QList<QScreen *> screens = app.screens();
    qDebug("Application sees %d screens", screens.count());

    for(int i=0; i <  screens.count(); i++) {
	qDebug() << "name " << screens[i]->name();

	qDebug() << "geom " << screens[i]->geometry();
	qDebug() << "size " << screens[i]->size();

	qDebug() << "virt geom " << screens[i]->virtualGeometry();
	qDebug() << "virt size " << screens[i]->virtualSize();

//	qDebug() << "aval geom " << screens[i]->availableGeometry();
//	qDebug() << "aval size " << screens[i]->availableSize();
//	qDebug() << "aval virt geom " << screens[i]->availableVirtualGeometry();
//	qDebug() << "aval virt geom " << screens[i]->availableVirtualSize();

	qDebug() << "---";
    }

    QTimer::singleShot(100, &app, SLOT(quit()));
    int r = app.exec();

    return r;
}

