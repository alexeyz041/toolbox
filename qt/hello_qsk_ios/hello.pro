QT += quick 3dextras

CONFIG += c++11

SOURCES += \
    main.cpp \
    SkinFactory.cpp


#RESOURCES += scenegraph.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    SkinFactory.h


QSK_DIRS = \
    /Users/alexeyz/Documents/git/qskinny_ios/src/common \
    /Users/alexeyz/Documents/git/qskinny_ios/src/nodes \
    /Users/alexeyz/Documents/git/qskinny_ios/src/graphic \
    /Users/alexeyz/Documents/git/qskinny_ios/src/controls \
    /Users/alexeyz/Documents/git/qskinny_ios/src/layouts \
    /Users/alexeyz/Documents/git/qskinny_ios/src/dialogs \
    /Users/alexeyz/Documents/git/qskinny_ios/src/inputpanel \
    /Users/alexeyz/Documents/git/qskinny_ios/support \
    /Users/alexeyz/Documents/git/qskinny_ios/skins/material \
    /Users/alexeyz/Documents/git/qskinny_ios/skins/squiek


INCLUDEPATH *= $${QSK_DIRS}

LIBS += -L/Users/alexeyz/Documents/git/qskinny_ios/lib -lqskinny -lqsktestsupport -L/Users/alexeyz/Documents/git/qskinny_ios/plugins/skins -lmaterialskin -lsquiekskin

LIBS += -L/Users/alexeyz/Documents/git/qt5/qtbase/qml/QtQml/Models.2 -lmodelsplugin_debug \
    -L/Users/alexeyz/Documents/git/qt5/qtbase/qml/QtQuick/Window.2 -lwindowplugin_debug \
    -L/Users/alexeyz/Documents/git/qt5/qtbase/qml/QtQuick.2 -lqtquick2plugin_debug
