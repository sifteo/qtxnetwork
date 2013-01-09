#ifndef FILETRANSFERMANAGER_H
#define FILETRANSFERMANAGER_H

#include "networkglobal.h"
#include <QtCore>

QTX_BEGIN_NAMESPACE

class FileTransfer;


class FileTransferManager : public QObject
{
    Q_OBJECT
    
public:
    static FileTransferManager* instance();
private:
    static FileTransferManager* _singleton;
    
public:
    FileTransferManager(QObject *parent = 0);
    virtual ~FileTransferManager();
    
    void add(FileTransfer *transfer);
    void reset();
    
    int maxConcurrent() const;
    void setMaxConcurrent(int max);
    
private:
    void startNext();
    
private slots:
    void onTransferFinished();
    void onTransferError(quint32 code);
    
private:
    QList<FileTransfer *> mActiveTransfers;
    QList<FileTransfer *> mQueuedTransfers;
    
    int mMaxConcurrent;
};


QTX_END_NAMESPACE
    
#endif // FILETRANSFERMANAGER_H
