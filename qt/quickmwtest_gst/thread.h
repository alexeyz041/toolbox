#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QScreen>
#include <QBuffer>

#include <string.h>
#include "gst.h"

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QScreen *screen,QObject *parent = 0) :  QThread(parent), quit(false), screen(screen)
    {
	memset(&config,0,sizeof(config));

	config.width = 800;
	config.height = 600;
	config.bpp = 4;
	strcpy(config.ip,"127.0.0.1");
	config.port = 5000;
    }

    virtual ~RenderThread()
    {
    };

    void shot(uchar *buf,int size);

public slots:
    void stop();

protected:
    void run();

private:
    bool quit;
    QScreen *screen;
    GST_CONFIG config;
};

#endif //THREAD_H
