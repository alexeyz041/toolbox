#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QScreen>
#include <QBuffer>

#include <string.h>

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QScreen *screen,QObject *parent = 0) :  QThread(parent), quit(false), screen(screen)
    {
    }

    virtual ~RenderThread()
    {
    };

    void shot();

public slots:
    void stop();

protected:
    void run();

private:
    bool quit;
    QScreen *screen;
};

#endif //THREAD_H
