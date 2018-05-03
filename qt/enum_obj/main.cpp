
#include <QGuiApplication>
#include "c.h"


int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    MyClass c(&app);
    c.print();

    return app.exec();
}
