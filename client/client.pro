TARGET=client
TEMPLATE=app
CONFIG += c++11
SOURCES= main.cpp \
    x264decoder.cpp

HEADERS += \
    x264decoder.h
LIBS += -lx264 -lswscale -lswresample -lavutil -lavcodec -lzmq `pkg-config --libs opencv`
