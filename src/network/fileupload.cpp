#include "fileupload.h"
#include "fileupload_p.h"
#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


FileUpload::FileUpload(const QNetworkRequest & request, QObject *parent /* = 0 */)
    : FileTransfer(*new FileUploadPrivate(this), parent)
{
    d_ptr->request = request;
}

FileUpload::~FileUpload()
{
}

void FileUpload::start()
{
    //qDebug() << "FileUpload::start";
    //qDebug() << "  path: " << d_ptr->path;
    //qDebug() << "  thread: " << QThread::currentThread();
    
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);
    
    d->exchange = new NetworkExchange(d->request, this);
    if (d->netAccessManager) {
        d->exchange->setNetworkAccessManager(d->netAccessManager);
    }
    
    d->file = new QFile(d->path, this);
    if (!d->file->open(QFile::ReadOnly)) {
        setError(QNetworkReply::UnknownContentError, d->file->errorString());
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
    
    connect(d->exchange, SIGNAL(replyReceived()), SLOT(onReplyReceived()));
    connect(d->exchange, SIGNAL(uploadProgress(qint64, qint64)), SLOT(onUploadProgress(qint64, qint64)));
    connect(d->exchange, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(d->exchange, SIGNAL(redirected(const QUrl &)), SLOT(onRedirected(const QUrl &)));
    connect(d->exchange, SIGNAL(finished()), SLOT(onFinished()));
    connect(d->exchange, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    connect(&d->txTimer, SIGNAL(timeout()), this, SLOT(onDataTxTimeout()));
    
    emit started();
    
    d->exchange->post(d->file);
    d->txTimer.start(d->txTimeout);
}

QString FileUpload::sourcePath() const
{
    return this->path();
}

void FileUpload::setSourcePath(const QString & path)
{
    this->setPath(path);
}

void FileUpload::onReplyReceived()
{
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);

    QUrl url = d->exchange->replyAttribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        d->isRedirecting = true;
    } else {
        d->isRedirecting = false;
    }
}

void FileUpload::onReadyRead()
{
}

void FileUpload::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);

    d->bytesSent = bytesSent;
    d->bytesTotal = bytesTotal;
    
    d->txTimer.start(d->txTimeout);
    
    // while requests are being redirected, actual progress is indeterminate
    qint64 total = d->isRedirecting ? -1 : bytesTotal;
    emit progress(bytesSent, total);
}

void FileUpload::onRedirected(const QUrl & url)
{
    Q_UNUSED(url)
}

void FileUpload::onFinished()
{
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);

    if (!d->file) {
        setError(QNetworkReply::UnknownContentError, "Illegal attempt to close invalid file");
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
    
    d->txTimer.stop();
    d->file->close();
    
    if (this->autoDelete()) {
        this->deleteLater();
    }
    
    emit finished();
}

void FileUpload::onError(QNetworkReply::NetworkError code)
{
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);

    d->txTimer.stop();
    
    // preserve custom errors, if set
    QNetworkReply::NetworkError e = this->error();
    if (e != QNetworkReply::TimeoutError) {
        setError(code, d->exchange->errorString());
    }
    emit error(code);
}

void FileUpload::onDataTxTimeout()
{
    FileUploadPrivate *d = static_cast<FileUploadPrivate *>(d_ptr);
    
    d->txTimer.stop();
    
    // Aborting a connection will result an error signal being emitted, followed
    // by a finished signal.  To keep the lifecycle consistent, handling will be
    // done in the corresponding slots.
    setError(QNetworkReply::TimeoutError, "Data transmit timeout");
    abort();
}


QTX_END_NAMESPACE
