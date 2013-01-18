#include "mockfiletransfer.h"


MockFileTransfer::MockFileTransfer(QObject *parent /* = 0 */)
    : FileTransfer(parent),
      mIsStarted(false)
{
}

MockFileTransfer::~MockFileTransfer()
{
}

void MockFileTransfer::start()
{
    mIsStarted = true;
}

void MockFileTransfer::abort()
{
    setError(QNetworkReply::OperationCanceledError, "mock file transfer aborted");
    emit error(QNetworkReply::OperationCanceledError);
    emit finished();
}

void MockFileTransfer::finish()
{
    emit finished();
}

void MockFileTransfer::fail()
{
    setError(QNetworkReply::UnknownNetworkError, "mock file transfer error");
    emit error(QNetworkReply::UnknownNetworkError);
    emit finished();
}

bool MockFileTransfer::isStarted() const
{
    return mIsStarted;
}
