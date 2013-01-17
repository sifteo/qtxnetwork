#include "filedownload.h"
#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


//FileDownload::FileDownload(NetworkExchange *connection)
FileDownload::FileDownload(const QNetworkRequest & request)
    : //mConnection(connection),
      mRequest(request),
      mConnection(0),
      mFile(0),
      mBytesReceived(0),
      mBytesTotal(0),
      mExpectedFileSize(-1),
      mDataRxTimeout(60000),  // 60000 msec = 60 sec = 1 min
      mRedirecting(false),
      mError(0),
      mDeleteWhenFinished(false),
      mAccessManager(0)
{
    //mConnection->setParent(this);
    //mOriginalUrl = connection->requestUrl();
    mOriginalUrl = mRequest.url();
}

FileDownload::~FileDownload()
{
}

void FileDownload::start()
{
    //qDebug() << "FileDownload::start";
    //qDebug() << "  thread: " << QThread::currentThread();
    //qDebug() << "  path: " << mPath;
    
    mConnection = new NetworkExchange(mRequest, this);
    if (mAccessManager) {
        mConnection->setNetworkAccessManager(mAccessManager);
    }
    
    connect(mConnection, SIGNAL(replyReceived()), SLOT(onReplyReceived()));
    connect(mConnection, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
    connect(mConnection, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(mConnection, SIGNAL(redirected(const QUrl &)), SLOT(onRedirected(const QUrl &)));
    connect(mConnection, SIGNAL(finished()), SLOT(onFinished()));
    connect(mConnection, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    connect(&mDataRxTimer, SIGNAL(timeout()), this, SLOT(onDataRxTimeout()));
    
    emit started();
    
    mConnection->get();
    mDataRxTimer.start(mDataRxTimeout);
}

void FileDownload::abort()
{
    mConnection->abort();
}

QUrl FileDownload::currentUrl() const
{
    return mConnection->requestUrl();
}

QUrl FileDownload::originalUrl() const
{
    return mOriginalUrl;
}

QString FileDownload::destinationPath() const
{
    return mPath;
}

qint64 FileDownload::bytesReceived() const
{
    return mBytesReceived;
}

qint64 FileDownload::bytesTotal() const
{
    return mBytesTotal;
}

void FileDownload::setDownloadPath(const QString & path)
{
    mPath = path;
}

void FileDownload::setExpectedFileSize(qint64 size)
{
    mExpectedFileSize = size;
}

void FileDownload::setDeleteWhenFinished(bool autoDelete /* = true */)
{
    mDeleteWhenFinished = autoDelete;
}

quint32 FileDownload::error() const
{
    return mError;
}

QString FileDownload::errorString() const
{
    return mErrorString;
}

void FileDownload::setError(quint32 code, const QString & string)
{
    mError = code;
    mErrorString = string;
}

void FileDownload::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    mAccessManager = manager;
}

void FileDownload::onReplyReceived()
{
    //qDebug() << "FileDownload::onReplyReceived";
    
    QUrl url = mConnection->replyAttribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        mRedirecting = true;
    } else {
        mRedirecting = false;
    }
    
    //QVariant header = mConnection->replyHeader(QNetworkRequest::ContentLengthHeader);
    //qint64 contentLength = header.isValid() ? header.toInt() : -1;
    
    // When a reply from the network is received, open the file and prepare it
    // to receive incoming data.  Due to redirects, multiple replies may be
    // received.  Therefore, the file is truncated each time, to ensure that
    // any data from previous replies is disposed.
    
    mDataRxTimer.start(mDataRxTimeout);
    
    if (mFile) {
        mFile->close();
        mFile->deleteLater();
    }
    
    mFile = new QFile(mPath, this);
    
    if (!mFile->open(QFile::WriteOnly | QFile::Truncate)) {
        QFile::FileError err = mFile->error();
        setError(FileErrorDomain + err, mFile->errorString());
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
}

void FileDownload::onReadyRead()
{
    if (!mFile) {
        qWarning() << "DownloadTask attempted to read data with invalid file";
        return;
    }
    
    mDataRxTimer.start(mDataRxTimeout);
    
    // stream incoming data from the network to the file
    mFile->write(mConnection->readAll());
}

void FileDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    //qDebug() << "FileDownload::onDownloadProgress";
    
    mBytesReceived = bytesReceived;
    mBytesTotal = bytesTotal;
    
    // while requests are being redirected, actual progress is indeterminate
    qint64 total = mRedirecting ? -1 : bytesTotal;
    emit progress(bytesReceived, total);
}

void FileDownload::onRedirected(const QUrl & url)
{
    emit redirected(url);
    
    mDataRxTimer.start(mDataRxTimeout);
}

void FileDownload::onFinished()
{
    if (!mFile) {
        qWarning() << "FileDownload attempted to finish with invalid file";
        return;
    }
    
    mDataRxTimer.stop();
    mFile->close();
    
    if (mDeleteWhenFinished) {
        this->deleteLater();
    }
    
    //if (!error()) {
        emit finished();
    //}
}

void FileDownload::onError(QNetworkReply::NetworkError code)
{
    qDebug() << "FileDownload::onError";
    qDebug() << "  code: " << code;
    qDebug() << "  string: " << mConnection->errorString();
    
    mDataRxTimer.stop();
    
    // preserve custom errors, if set
    if (error() != TimeoutError) {
        setError(code, mConnection->errorString());
    }
    emit error(code);
}

void FileDownload::onDataRxTimeout()
{
    //qDebug() << "FileDownload::onDataRxTimeout";
    
    mDataRxTimer.stop();
    
    // Aborting a connection will result an error signal being emitted, followed
    // by a finished signal.  To keep the lifecycle consistent, handling will be
    // done in the corresponding slots.
    setError(TimeoutError, "Data receive timeout");
    abort();
}


QTX_END_NAMESPACE
