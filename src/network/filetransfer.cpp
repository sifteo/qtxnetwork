#include "filetransfer.h"
#include "filetransfer_p.h"

QTX_BEGIN_NAMESPACE


FileTransfer::FileTransfer(QObject *parent /* = 0 */)
    : QObject(parent),
      d_ptr(new FileTransferPrivate(this))
{
}

FileTransfer::FileTransfer(FileTransferPrivate &d, QObject *parent /* = 0 */)
    : QObject(parent),
      d_ptr(&d)
{
}

FileTransfer::~FileTransfer()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

void FileTransfer::abort()
{
    d_ptr->exchange->abort();
}

QString FileTransfer::path() const
{
    return d_ptr->path;
}

bool FileTransfer::autoDelete() const
{
    return d_ptr->autoDelete;
}

void FileTransfer::setPath(const QString & path)
{
    d_ptr->path = path;
}

void FileTransfer::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    d_ptr->netAccessManager = manager;
}

void FileTransfer::setAutoDelete(bool autoDelete /* = true */)
{
    d_ptr->autoDelete = autoDelete;
}

QNetworkReply::NetworkError FileTransfer::error() const
{
    return d_ptr->errorCode;
}

QString FileTransfer::errorString() const
{
    return d_ptr->errorString;
}

void FileTransfer::setError(QNetworkReply::NetworkError errorCode, const QString & errorString)
{
    d_ptr->errorCode = errorCode;
    d_ptr->errorString = errorString;
}


FileTransferPrivate::FileTransferPrivate(FileTransfer *q)
    : q_ptr(q),
      netAccessManager(0),
      exchange(0),
      file(0),
      autoDelete(false),
      errorCode(QNetworkReply::NoError)
{
}

FileTransferPrivate::~FileTransferPrivate()
{
}


QTX_END_NAMESPACE
