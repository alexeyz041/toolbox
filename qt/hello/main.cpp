#include <QtWidgets>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;

    QLabel *label = new QLabel("Hello World!");

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(label);
    window.setLayout(layout);

    window.resize(320, 240);
    window.show();

    window.setWindowTitle("Hello");
    return app.exec();
}
