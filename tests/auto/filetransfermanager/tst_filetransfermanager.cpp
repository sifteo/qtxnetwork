#include "tst_filetransfermanager.h"
#include "../../mock/src/mockfiletransfer.h"


void tst_FileTransferManager::initTestCase()
{
}

void tst_FileTransferManager::cleanupTestCase()
{
}

void tst_FileTransferManager::init()
{
    mManager = new FileTransferManager();
}

void tst_FileTransferManager::cleanup()
{
    if (mManager) {
        delete mManager;
        mManager = 0;
    }
}

void tst_FileTransferManager::testQueueing()
{
    MockFileTransfer *transfer1 = new MockFileTransfer();
    MockFileTransfer *transfer2 = new MockFileTransfer();
    MockFileTransfer *transfer3 = new MockFileTransfer();
    MockFileTransfer *transfer4 = new MockFileTransfer();
    
    mManager->add(transfer1);
    mManager->add(transfer2);
    mManager->add(transfer3);
    mManager->add(transfer4);
    
    QVERIFY(transfer1->isStarted());
    QVERIFY(transfer2->isStarted());
    QVERIFY(transfer3->isStarted());
    QVERIFY(!transfer4->isStarted());
    
    transfer2->finish();
    
    QVERIFY(transfer4->isStarted());
}

void tst_FileTransferManager::testQueueingAfterError()
{
    MockFileTransfer *transfer1 = new MockFileTransfer();
    MockFileTransfer *transfer2 = new MockFileTransfer();
    MockFileTransfer *transfer3 = new MockFileTransfer();
    MockFileTransfer *transfer4 = new MockFileTransfer();
    
    mManager->add(transfer1);
    mManager->add(transfer2);
    mManager->add(transfer3);
    mManager->add(transfer4);
    
    QVERIFY(transfer1->isStarted());
    QVERIFY(transfer2->isStarted());
    QVERIFY(transfer3->isStarted());
    QVERIFY(!transfer4->isStarted());
    
    transfer2->fail();
    
    QVERIFY(transfer4->isStarted());
}


QTEST_APPLESS_MAIN(tst_FileTransferManager)
