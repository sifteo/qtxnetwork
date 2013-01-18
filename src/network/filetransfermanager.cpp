#include "filetransfermanager.h"
#include "filetransfer.h"

QTX_BEGIN_NAMESPACE


class FileTransferManagerPrivate
{
public:
    FileTransferManagerPrivate(FileTransferManager *q);
    virtual ~FileTransferManagerPrivate();
    
    void startNext();
    
public:
    FileTransferManager *q_ptr;
    Q_DECLARE_PUBLIC(FileTransferManager);
    
    QList<FileTransfer *> active;
    QList<FileTransfer *> queue;
    int maxConcurrent;
};


FileTransferManager *FileTransferManager::_singleton = 0;

FileTransferManager *FileTransferManager::instance()
{
    if (!_singleton) {
        _singleton = new FileTransferManager();
    }
    return _singleton;
}


FileTransferManager::FileTransferManager(QObject *parent /* = 0 */)
    : QObject(parent),
      d_ptr(new FileTransferManagerPrivate(this))
{
}

FileTransferManager::~FileTransferManager()
{
    reset();
    
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

void FileTransferManager::add(FileTransfer *transfer)
{
    transfer->setParent(this);
    d_ptr->queue.append(transfer);
    
    if (d_ptr->active.size() < d_ptr->maxConcurrent) {
        d_ptr->startNext();
    }
}

void FileTransferManager::reset()
{
    foreach (FileTransfer *transfer, d_ptr->queue) {
        transfer->deleteLater();
    }
    d_ptr->queue.clear();
    
    foreach (FileTransfer *transfer, d_ptr->active) {
        transfer->disconnect(this);
        transfer->setAutoDelete();
        transfer->abort();
    }
    d_ptr->active.clear();
}

int FileTransferManager::maxConcurrent() const
{
    return d_ptr->maxConcurrent;
}

void FileTransferManager::setMaxConcurrent(int max)
{
    d_ptr->maxConcurrent = max;
}

void FileTransferManager::onTransferFinished()
{
    FileTransfer* transfer = qobject_cast<FileTransfer *>(sender());
    if (!transfer) { return; }
    
    d_ptr->active.removeOne(transfer);
    transfer->disconnect(this);
    transfer->deleteLater();
    
    if (d_ptr->active.size() < d_ptr->maxConcurrent) {
        d_ptr->startNext();
    }
}


FileTransferManagerPrivate::FileTransferManagerPrivate(FileTransferManager *q)
    : q_ptr(q),
      maxConcurrent(3)
{
}

FileTransferManagerPrivate::~FileTransferManagerPrivate()
{
}

void FileTransferManagerPrivate::startNext()
{
    if (queue.isEmpty()) {
        return;
    }
    
    FileTransfer *transfer = queue.takeFirst();
    // `FileTransfer` always emits `finished`.  Connecting to `error` is not
    // necessary because `error` is followed by `finish`.
    q_ptr->connect(transfer, SIGNAL(finished()), SLOT(onTransferFinished()));
    
    active.append(transfer);
    transfer->start();
}


QTX_END_NAMESPACE
