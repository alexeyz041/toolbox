//
// Test Qt multiscreen configuration + save screen to file
// based on https://github.com/alpqr/quickmwtest
//


#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>
#include <QScreen>
#include <QDebug>

#include "thread.h"

static QQuickView *addView(QScreen *screen, int screenIdx,int screenCount)
{
    qDebug("Creating new QQuickView for screen %d", screenIdx);
    QQuickView *v = new QQuickView;

    v->setScreen(screen);
    v->setResizeMode(QQuickView::SizeRootObjectToView);

    v->rootContext()->setContextProperty("screenIdx", screenIdx);
    v->rootContext()->setContextProperty("screenCount", screenCount);
    v->rootContext()->setContextProperty("screenGeom", screen->geometry());
    v->rootContext()->setContextProperty("screenAvailGeom", screen->availableGeometry());
    v->rootContext()->setContextProperty("screenVirtGeom", screen->virtualGeometry());
    v->rootContext()->setContextProperty("screenAvailVirtGeom", screen->availableVirtualGeometry());
    v->rootContext()->setContextProperty("screenPhysSizeMm", screen->physicalSize());
    v->rootContext()->setContextProperty("screenRefresh", screen->refreshRate());

    v->setSource(QUrl("qrc:/screen.qml"));

    QObject::connect(v->engine(), &QQmlEngine::quit, qGuiApp, &QCoreApplication::quit);

    return v;
}



int main(int argc, char **argv)
{
//    qputenv("QT_LOGGING_RULES", "qt.qpa.*=true");
//    qputenv("QSG_INFO", "1");

    QGuiApplication app(argc, argv);

    QList<QScreen *> screens = app.screens();
    qDebug("Application sees %d screens", screens.count());
    qDebug() << screens;

    QVector<QQuickView *> views;
    for (int i = 0; i < screens.count(); ++i) {
        QQuickView *v = addView(screens[i], i, screens.count());
        views.append(v);
        v->showFullScreen();
    }

    RenderThread *rt = new RenderThread(screens[screens.count()-1]);
    rt->start();
    QObject::connect(&app, SIGNAL(aboutToQuit()), rt, SLOT(stop()));

    int r = app.exec();

    qDeleteAll(views);
    return r;
}
