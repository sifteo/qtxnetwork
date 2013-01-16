TEMPLATE = app
TARGET = tst_networkexchange

QT += testlib
macx:CONFIG -= app_bundle

include(../../../mkspecs/test.pri)

HEADERS += tst_networkexchange.h
SOURCES += tst_networkexchange.cpp
