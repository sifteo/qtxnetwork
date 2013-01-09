#include "filetransfermanager.h"
#include "filetransfer.h"

QTX_BEGIN_NAMESPACE


FileTransferManager* FileTransferManager::_singleton = 0;

FileTransferManager* FileTransferManager::instance()
{
    if (!_singleton) {
        _singleton = new FileTransferManager();
    }
    return _singleton;
}


FileTransferManager::FileTransferManager(QObject *parent /* = 0 */)
    : QObject(parent),
      mMaxConcurrent(3)
{
}

FileTransferManager::~FileTransferManager()
{
    reset();
}

void FileTransferManager::add(FileTransfer *transfer)
{
    mQueuedTransfers.append(transfer);
    
    if (mActiveTransfers.size() < mMaxConcurrent) {
        startNext();
    }
}

void FileTransferManager::reset()
{
    foreach (FileTransfer *transfer, mQueuedTransfers) {
        transfer->deleteLater();
    }
    mQueuedTransfers.clear();
    
    foreach (FileTransfer *transfer, mActiveTransfers) {
        transfer->disconnect(this);
        transfer->setDeleteWhenFinished();
        transfer->abort();
    }
    mActiveTransfers.clear();
}

int FileTransferManager::maxConcurrent() const
{
    return mMaxConcurrent;
}

void FileTransferManager::setMaxConcurrent(int max)
{
    mMaxConcurrent = max;
}

void FileTransferManager::startNext()
{
    if (mQueuedTransfers.isEmpty()) {
        return;
    }
    
    FileTransfer *transfer = mQueuedTransfers.takeFirst();
    connect(transfer, SIGNAL(finished()), this, SLOT(onTransferFinished()));
    connect(transfer, SIGNAL(error(quint32)), this, SLOT(onTransferError(quint32)));
    
    mActiveTransfers.append(transfer);
    transfer->start();
}

void FileTransferManager::onTransferFinished()
{
    //qDebug() << "FileTransferManager::onTransferDone";
    
    FileTransfer* transfer = qobject_cast<FileTransfer *>(sender());
    if (!transfer) { return; }
    
    mActiveTransfers.removeOne(transfer);
    transfer->disconnect(this);
    transfer->deleteLater();
    
    if (mActiveTransfers.size() < mMaxConcurrent) {
        startNext();
    }
}

void FileTransferManager::onTransferError(quint32 code)
{
    Q_UNUSED(code)
    
    //qDebug() << "FileTransferManager::onTransferError";
    
    FileTransfer* transfer = qobject_cast<FileTransfer *>(sender());
    if (!transfer) { return; }
    
    mActiveTransfers.removeOne(transfer);
    transfer->disconnect(this);
    transfer->deleteLater();
    
    if (mActiveTransfers.size() < mMaxConcurrent) {
        startNext();
    }
}


QTX_END_NAMESPACE
