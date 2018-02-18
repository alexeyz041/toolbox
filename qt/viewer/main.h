
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>


class GraphicsScene : public QGraphicsScene {
    Q_OBJECT

public:
    GraphicsScene()
    {
    }

    virtual ~GraphicsScene()
    {
    }

public slots:
    void update1(uint8_t *buf,int len)
    {
    	QPixmap img;
    	if(img.loadFromData(buf,len)) {
        	printf("%d x %d\n",img.width(),img.height());
    	}

    	((QGraphicsPixmapItem *)items()[0])->setPixmap(img);
    }

private:

};

