TEMPLATE = app
TARGET = tst_filedownload

QT += testlib
macx:CONFIG -= app_bundle

include(../../../mkspecs/test.pri)

HEADERS += tst_filedownload.h
SOURCES += tst_filedownload.cpp
