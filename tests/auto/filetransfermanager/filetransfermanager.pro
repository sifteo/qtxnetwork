TEMPLATE = app
TARGET = tst_filetransfermanager

QT += testlib
macx:CONFIG -= app_bundle

include(../../../mkspecs/test.pri)

HEADERS += tst_filetransfermanager.h \
           ../../mock/src/mockfiletransfer.h
SOURCES += tst_filetransfermanager.cpp \
           ../../mock/src/mockfiletransfer.cpp
