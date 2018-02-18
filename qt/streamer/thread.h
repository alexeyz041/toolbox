#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QThread>
#include <QPixmap>
#include <QGraphicsScene>

class RenderThread : public QThread
{
    Q_OBJECT

public:
    RenderThread(QObject *parent = 0) : QThread(parent), quit(false)
    {

    }

    virtual ~RenderThread()
    {
    };


public slots:
	void stop();

protected:
    void run();

private:
    bool quit;
};

#endif //THREAD_H
