#ifndef MOCK_MOCKFILETRANSFER_H
#define MOCK_MOCKFILETRANSFER_H

#include "filetransfer.h"
#include <QtCore>

QTX_USE_NAMESPACE


class MockFileTransfer : public FileTransfer
{
    Q_OBJECT

public:
    MockFileTransfer(QObject *parent = 0);
    ~MockFileTransfer();
    
    void start();
    void abort();
    void finish();
    void fail();
    
    bool isStarted() const;
    
private:
    bool mIsStarted;
};

#endif // MOCK_MOCKFILETRANSFER_H