#include "filedownload.h"
#include "filedownload_p.h"
#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


FileDownload::FileDownload(const QNetworkRequest & request, QObject *parent /* = 0 */)
    : FileTransfer(*new FileDownloadPrivate(this), parent)
{
    d_ptr->request = request;
}

FileDownload::~FileDownload()
{
}

void FileDownload::start()
{
    //qDebug() << "FileDownload::start";
    //qDebug() << "  thread: " << QThread::currentThread();
    //qDebug() << "  path: " << d_ptr->path;
    
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    
    d->exchange = new NetworkExchange(d->request, this);
    if (d->netAccessManager) {
        d->exchange->setNetworkAccessManager(d->netAccessManager);
    }
    
    connect(d->exchange, SIGNAL(replyReceived()), SLOT(onReplyReceived()));
    connect(d->exchange, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
    connect(d->exchange, SIGNAL(readyRead()), SLOT(onReadyRead()));
    connect(d->exchange, SIGNAL(redirected(const QUrl &)), SLOT(onRedirected(const QUrl &)));
    connect(d->exchange, SIGNAL(finished()), SLOT(onFinished()));
    connect(d->exchange, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    connect(&d->rxTimer, SIGNAL(timeout()), this, SLOT(onDataRxTimeout()));
    
    emit started();
    
    d->exchange->get();
    d->rxTimer.start(d->rxTimeout);
}

QUrl FileDownload::currentUrl() const
{
    return d_ptr->exchange->requestUrl();
}

QUrl FileDownload::originalUrl() const
{
    return d_ptr->request.url();
}

QString FileDownload::destinationPath() const
{
    return this->path();
}

qint64 FileDownload::bytesReceived() const
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    return d->bytesReceived;
}

qint64 FileDownload::bytesTotal() const
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    return d->bytesTotal;
}

void FileDownload::setDestinationPath(const QString & path)
{
    this->setPath(path);
}

void FileDownload::setExpectedFileSize(qint64 size)
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    d->expectedFileSize = size;
}

void FileDownload::onReplyReceived()
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    
    QUrl url = d->exchange->replyAttribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (!url.isEmpty()) {
        d->isRedirecting = true;
    } else {
        d->isRedirecting = false;
    }
    
    //QVariant header = d->exchange->replyHeader(QNetworkRequest::ContentLengthHeader);
    //qint64 contentLength = header.isValid() ? header.toInt() : -1;
    
    // When a reply from the network is received, open the file and prepare it
    // to receive incoming data.  Due to redirects, multiple replies may be
    // received.  Therefore, the file is truncated each time, to ensure that
    // any data from previous replies is disposed.
    
    d->rxTimer.start(d->rxTimeout);
    
    if (d->file) {
        d->file->close();
        d->file->deleteLater();
    }
    
    d->file = new QFile(d->path, this);
    
    if (!d->file->open(QFile::WriteOnly | QFile::Truncate)) {
        setError(QNetworkReply::UnknownContentError, d->file->errorString());
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
}

void FileDownload::onReadyRead()
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);

    if (!d->file) {
        setError(QNetworkReply::UnknownContentError, "Illegal attempt to write data to invalid file");
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
    
    d->rxTimer.start(d->rxTimeout);
    
    // stream incoming data from the network to the file
    d->file->write(d->exchange->readAll());
}

void FileDownload::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);
    
    d->bytesReceived = bytesReceived;
    d->bytesTotal = bytesTotal;
    
    // while requests are being redirected, actual progress is indeterminate
    qint64 total = d->isRedirecting ? -1 : bytesTotal;
    emit progress(bytesReceived, total);
}

void FileDownload::onRedirected(const QUrl & url)
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);

    emit redirected(url);
    
    d->rxTimer.start(d->rxTimeout);
}

void FileDownload::onFinished()
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);

    if (!d->file) {
        setError(QNetworkReply::UnknownContentError, "Illegal attempt to close invalid file");
        emit error(QNetworkReply::UnknownContentError);
        return;
    }
    
    d->rxTimer.stop();
    d->file->close();
    
    if (this->autoDelete()) {
        this->deleteLater();
    }
    
    emit finished();
}

void FileDownload::onError(QNetworkReply::NetworkError code)
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);

    d->rxTimer.stop();
    
    // preserve custom errors, if set
    QNetworkReply::NetworkError e = this->error();
    if (e != QNetworkReply::TimeoutError) {
        setError(code, d_ptr->exchange->errorString());
    }
    emit error(code);
}

void FileDownload::onDataRxTimeout()
{
    FileDownloadPrivate *d = static_cast<FileDownloadPrivate *>(d_ptr);

    d->rxTimer.stop();
    
    // Aborting a connection will result an error signal being emitted, followed
    // by a finished signal.  To keep the lifecycle consistent, handling will be
    // done in the corresponding slots.
    setError(QNetworkReply::TimeoutError, "Data receive timeout");
    abort();
}


QTX_END_NAMESPACE
