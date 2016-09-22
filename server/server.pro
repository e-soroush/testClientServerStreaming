TARGET = server
TEMPLATE = app
SOURCES = main.cpp \
    x264encoder.cpp

HEADERS += \
    x264encoder.h
LIBS += -lx264 -lswscale -lswresample -lzmq `pkg-config --libs opencv`
