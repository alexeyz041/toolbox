TEMPLATE     = app

QT          += qml quick

SOURCES     += main.cpp thread.cpp gst.c
RESOURCES   += clocks.qrc

target.path  = $$[QT_INSTALL_EXAMPLES]/quick/demos/clocks
INSTALLS    += target

HEADERS = thread.h


CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0


OTHER_FILES  += \
                clocks.qml \
                content/Clock.qml \
                content/*.png
