INCLUDEPATH+=$$PWD $$PWD/../zlib-1.2.8
DEPENDPATH+=$$PWD/quazip
include($$PWD/quazip/quazip.pri)
DEFINES+=QUAZIP_STATIC
