
#include "screenstreamer.h"


void MySource::needData(uint length)
{
    Q_UNUSED(length);
    if(m_quit && *m_quit) return;

    QPixmap orig = m_screen->grabWindow(0);
    QRect rect(m_x, m_y, m_x+m_width, m_y+m_height);
    QPixmap cropped = orig.copy(rect);
    QImage img = cropped.toImage();

    int size = m_width * m_height * m_bpp;
    QGst::BufferPtr buffer = QGst::Buffer::create(size);

    buffer->presentationTimeStamp() = QGst::ClockTime::fromMSecs(100 * m_numSamples++); // 10 fps
    buffer->duration() = QGst::ClockTime::fromMSecs(100);

    QGst::MapInfo mapInfo;
    buffer->map(mapInfo, QGst::MapWrite);
    memcpy(mapInfo.data(),img.bits(),size);
    buffer->unmap(mapInfo);

    pushBuffer(buffer);
}


void ScreenStreamer::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos:
        qDebug() << "EOS";
        break;
    case QGst::MessageError:
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        break;
    default:
        break;
    }
}


void ScreenStreamer::init()
{
    QString pipeDescr = QString("appsrc name=\"mysrc\" ! video/x-raw,format=BGRA,width=%1,height=%2,framerate=10/1 ! "
				"videoconvert ! video/x-raw,format=I420,width=%1,height=%2,framerate=10/1 ! jpegenc ! rtpjpegpay ! "
				"udpsink host=%3 port=%4")
			.arg(m_width)
			.arg(m_height)
			.arg(m_destIP)
			.arg(m_destPort);

    m_pipeline = QGst::Parse::launch(pipeDescr).dynamicCast<QGst::Pipeline>();
    m_source.setElement(m_pipeline->getElementByName("mysrc"));
    QGlib::connect(m_pipeline->bus(), "message", this, &ScreenStreamer::onBusMessage);
    m_pipeline->bus()->addSignalWatch();
    m_pipeline->setState(QGst::StatePlaying);
}


void ScreenStreamer::stop()
{
    m_quit = true;
}

