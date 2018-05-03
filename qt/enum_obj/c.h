
#include <QObject>


class MyClass : public QObject {
    Q_OBJECT
public:
    MyClass(QObject *parent = nullptr) : QObject(parent)
    {
	setObjectName("MyClass123");
    }

    void print();
};