
#include <QGuiApplication>
#include <QskGridBox.h>
#include <QskWindow.h>
#include <SkinnyFont.h>
#include <QskTextLabel.h>

#include <QskSkinManager.h>
#include <QskSetup.h>
#include "SkinFactory.h"

#include <QQmlExtensionPlugin>
Q_IMPORT_PLUGIN(QtQuick2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQmlModelsPlugin)


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
    auto skinFactory = new SkinFactory();
    qskSkinManager->setPluginPaths( QStringList() ); // no plugins
    qskSkinManager->registerFactory( QStringLiteral( "SampleSkinFactory" ), skinFactory );
    
    QGuiApplication app(argc, argv);
    SkinnyFont::init( &app );

    QskWindow window;
    window.addItem( new TextBox() );
    window.show();

    return app.exec();
}
