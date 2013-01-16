include(features/qtx_testcase.prf)


QT += network

INCLUDEPATH += \
    ../../../src/network \
    ../../../../qtxmocknetwork/include

LIBS += \
    -L../../../lib \
    -L../../../../qtxmocknetwork/lib

LIBS += -lQtxNetwork -lQtxMockNetwork
