TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CFLAGS += -std=gnu99

SOURCES += main.c \
    zlib_compression.c \
    util.c

HEADERS += \
    zlib_compression.h \
    util.h

unix:!macx: LIBS += -lz
unix:!macx: LIBS += -lrt
