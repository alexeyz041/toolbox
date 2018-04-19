
#include <QGuiApplication>
#include <QskGridBox.h>
#include <QskWindow.h>
#include <SkinnyFont.h>
#include <QskTextLabel.h>




class TextBox: public QskGridBox
{
public:
    TextBox(QQuickItem *parent = nullptr) : QskGridBox(parent)
    {
	addItem(new QskTextLabel("Hello", this ),0,0);
    }
};



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    SkinnyFont::init( &app );

    QskWindow window;
    window.addItem( new TextBox() );
    window.show();

    return app.exec();
}
