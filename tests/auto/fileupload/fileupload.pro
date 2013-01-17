TEMPLATE = app
TARGET = tst_fileupload

QT += testlib
macx:CONFIG -= app_bundle

include(../../../mkspecs/test.pri)

HEADERS += tst_fileupload.h
SOURCES += tst_fileupload.cpp
