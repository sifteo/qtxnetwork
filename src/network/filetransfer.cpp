#include "filetransfer.h"
#include "filetransfer_p.h"

QTX_BEGIN_NAMESPACE


FileTransfer::FileTransfer(QObject *parent /* = 0 */)
    : QObject(parent),
      d_ptr(new FileTransferPrivate(this))
{
}

FileTransfer::~FileTransfer()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
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
      errorCode(QNetworkReply::NoError)
{
}

FileTransferPrivate::~FileTransferPrivate()
{
}


QTX_END_NAMESPACE
