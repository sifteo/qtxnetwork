#include "fileupload.h"
#include "networkexchange.h"


FileUpload::FileUpload(NetworkExchange *connection)
    : mConnection(connection),
      mBytesSent(0),
      mBytesTotal(0),
      mDataTxTimeout(60000),  // 60000 msec = 60 sec = 1 min
      mRedirecting(false),
      mError(0),
      mDeleteWhenFinished(false)
{
    mConnection->setParent(this);
}

FileUpload::~FileUpload()
{
}

void FileUpload::start()
{
    //qDebug() << "FileUpload::start";
    //qDebug() << "  path: " << mPath;
    //qDebug() << "  thread: " << QThread::currentThread();
    
    mFile = new QFile(mPath, this);
    if (!mFile->open(QFile::ReadOnly)) {
        QFile::FileError err = mFile->error();
        setError(FileErrorDomain + err, mFile->errorString());
        emit error(error());
        return;
    }
    
    //qDebug() << "  fileSize: " << mFile->size();
    
    connect(mConnection, SIGNAL(replyReceived()), SLOT(onReplyReceived()));
    connect(mConnection, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
    connect(mConnection, SIGNAL(uploadProgress(qint64, qint64)), SLOT(onUploadProgress(qint64, qint64)));
    connect(mConnection, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(mConnection, SIGNAL(redirected(const QUrl &)), SLOT(onRedirected(const QUrl &)));
    connect(mConnection, SIGNAL(finished()), SLOT(onFinished()));
    connect(mConnection, SIGNAL(error(quint32)), SLOT(onError(quint32)));
    connect(&mDataTxTimer, SIGNAL(timeout()), this, SLOT(onDataTxTimeout()));
    
    emit started();
    
    mConnection->post(mFile);
    mDataTxTimer.start(mDataTxTimeout);
}

void FileUpload::abort()
{
    mConnection->abort();
}

QString FileUpload::sourcePath() const
{
    return mPath;
}

void FileUpload::setUploadPath(const QString & path)
{
    mPath = path;
}

void FileUpload::setDeleteWhenFinished(bool autoDelete /* = true */)
{
    mDeleteWhenFinished = autoDelete;
}

quint32 FileUpload::error() const
{
    return mError;
}

QString FileUpload::errorString() const
{
    return mErrorString;
}

void FileUpload::setError(quint32 code, const QString & string)
{
    mError = code;
    mErrorString = string;
}

void FileUpload::onReplyReceived()
{
    //qDebug() << "FileUpload::onReplyReceived";
    
    QUrl url = mConnection->replyAttribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        mRedirecting = true;
    } else {
        mRedirecting = false;
    }
}

void FileUpload::onReadyRead()
{
    //qDebug() << "FileUpload::onReadyRead";
}

void FileUpload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived)
    Q_UNUSED(bytesTotal)
    
    //qDebug() << "FileUpload::onDownloadProgress";
}

void FileUpload::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    //qDebug() << "FileUpload::onUploadProgress" << bytesSent << bytesTotal;
    
    mBytesSent = bytesSent;
    mBytesTotal = bytesTotal;
    
    mDataTxTimer.start(mDataTxTimeout);
    
    // while requests are being redirected, actual progress is indeterminate
    qint64 total = mRedirecting ? -1 : bytesTotal;
    emit progress(bytesSent, total);
}

void FileUpload::onRedirected(const QUrl & url)
{
    Q_UNUSED(url)
    
    //mDataTxTimer.start(mDataTxTimeout);
}

void FileUpload::onFinished()
{
    //qDebug() << "FileUpload::onFinished";
    
    if (!mFile) {
        qWarning() << "FileUpload attempted to finish with invalid file";
        return;
    }
    
    mDataTxTimer.stop();
    mFile->close();
    
    if (mDeleteWhenFinished) {
        this->deleteLater();
    }
    
    if (!error()) {
        emit finished();
    }
}

void FileUpload::onError(quint32 code)
{
    qDebug() << "FileUpload::onError";
    qDebug() << "  code: " << code;
    qDebug() << "  string: " << mConnection->errorString();
    
    mDataTxTimer.stop();
    
    // preserve custom errors, if set
    if (error() != TimeoutError) {
        setError(code, mConnection->errorString());
    }
    emit error(error());
}

void FileUpload::onDataTxTimeout()
{
    qDebug() << "FileUpload::onDataTxTimeout";
    
    mDataTxTimer.stop();
    
    // Aborting a connection will result an error signal being emitted, followed
    // by a finished signal.  To keep the lifecycle consistent, handling will be
    // done in the corresponding slots.
    setError(TimeoutError, "Data receive timeout");
    abort();
}
