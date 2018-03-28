#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QScreen>
#include <QBuffer>

#include <QCoreApplication>
#include <QGlib/Error>
#include <QGlib/Connect>
#include <QGst/Init>
#include <QGst/Bus>
#include <QGst/Pipeline>
#include <QGst/Parse>
#include <QGst/Message>
#include <QGst/Buffer>
#include <QGst/Utils/ApplicationSink>
#include <QGst/Utils/ApplicationSource>


class MySource : public QGst::Utils::ApplicationSource
{
public:
    MySource(QScreen *screen,bool *quit,int x,int y,int width,int height,int bpp) 
	: QGst::Utils::ApplicationSource(), m_quit(quit), m_screen(screen), m_numSamples(0), m_x(x),m_y(y),m_width(width),m_height(height),m_bpp(bpp)
    {
    }

protected:
    virtual void needData(uint length);

private:
    bool *m_quit;
    QScreen *m_screen;
    uint64_t m_numSamples;
    int m_x;
    int m_y;
    int m_width;
    int m_height;
    int m_bpp;
};



class ScreenStreamer : public QObject
{
    Q_OBJECT

public:
    ScreenStreamer(QScreen *screen,int argc,char **argv,const char *destIP,int destPort,int width,int height,QObject *parent = 0) 
	:  QObject(parent), m_quit(false), m_destIP(destIP),m_destPort(destPort),m_width(width),m_height(height),m_x(0),m_y(0),
	    m_source(screen,&m_quit,m_x,m_y,width,height,4)
    {
	QGst::init(&argc, &argv);
	init();
    }

    virtual ~ScreenStreamer()
    {
	m_pipeline->setState(QGst::StateNull);
    };

public slots:
    void stop();

private:
    void init();
    void onBusMessage(const QGst::MessagePtr & message);

private:
    bool m_quit;
    QString m_destIP;
    int m_destPort;
    int m_width;
    int m_height;
    int m_x;
    int m_y;
    MySource m_source;
    QGst::PipelinePtr m_pipeline;
};

#endif //THREAD_H
