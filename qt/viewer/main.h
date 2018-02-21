
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
    void update1(uint8_t *buf,int len);

private:

};

