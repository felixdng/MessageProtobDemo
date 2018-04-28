TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    messagebase.cpp \
    messageserver.cpp \
    messageclient.cpp

HEADERS += \
    messagebase.h \
    messageserver.h \
    messageclient.h
