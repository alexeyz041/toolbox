TEMPLATE     = app

QT          += qml quick

HEADERS     += screenstreamer.h
SOURCES     += main.cpp screenstreamer.cpp
RESOURCES   += clocks.qrc

target.path  = $$[QT_INSTALL_EXAMPLES]/quick/demos/clocks
INSTALLS    += target

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0 Qt5GStreamer-1.0 Qt5GStreamerUtils-1.0

OTHER_FILES  += clocks.qml \
		clocks2.qml \
                content/Clock.qml \
                content/*.png
