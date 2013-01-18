#ifndef TST_FILETRANSFERMANAGER_H
#define TST_FILETRANSFERMANAGER_H

#include "filetransfermanager.h"
#include <QtTest/QtTest>

QTX_USE_NAMESPACE


class tst_FileTransferManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testQueueing();
    void testQueueingAfterError();
    
    //void testUpload();
    //void testErrorResponse();
    
private:
    FileTransferManager *mManager;

    //QEventLoop mEventLoop;
    //MockNetworkAccessManager *mNetAccessManager;
    //FileUpload *mUpload;
};

#endif // TST_FILETRANSFERMANAGER_H
