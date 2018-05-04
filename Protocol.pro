TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    messagebase.cpp \
    messageserver.cpp \
    messageclient.cpp \
    sdcp_base.cpp \
    sdcp_client.cpp \
    protobufbase.cpp

HEADERS += \
    messagebase.h \
    messageserver.h \
    messageclient.h \
    sdcp_base.h \
    sdcp_client.h \
    protobufbase.h
