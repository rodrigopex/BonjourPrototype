#-------------------------------------------------
#
# Project created by QtCreator 2014-02-06T14:43:01
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = QBonjour
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

!mac:LIBS += -ldns_sd

SOURCES += main.cpp \
    bonjourserviceregister.cpp \
    bonjourserviceresolver.cpp \
    bonjourservicebrowser.cpp \
    Serializable.cpp \
    server.cpp

#HEADERS +=

HEADERS += \
    bonjourserviceregister.h \
    bonjourserviceresolver.h \
    bonjourservicebrowser.h \
    bonjourrecord.h \
    Serializable.h \
    server.h
