TEMPLATE = app

QT += quick

SOURCES = main.cpp thread.cpp gst.c
HEADERS = thread.h

RESOURCES = quickmwtest.qrc
OTHER_FILES = screen.qml

CONFIG += link_pkgconfig
PKGCONFIG += gstreamer-1.0
