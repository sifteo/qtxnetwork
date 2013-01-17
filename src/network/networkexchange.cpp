#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


NetworkExchange::NetworkExchange(const QNetworkRequest & request, QObject *parent /* = 0 */)
    : QObject(parent),
      mAccessManager(0),
      mRequest(request),
      mReply(0),
      mMethod(GetMethod),
      mMaxRedirects(5),
      mReplyReceived(false)
{
}

NetworkExchange::~NetworkExchange()
{
}

void NetworkExchange::get()
{
    start(GetMethod);
}

void NetworkExchange::head()
{
    start(HeadMethod);
}

void NetworkExchange::post(QIODevice *data)
{
    start(PostMethod, data);
}

void NetworkExchange::put(QIODevice *data)
{
    start(PutMethod, data);
}

void NetworkExchange::deleteResource()
{
    start(DeleteMethod);
}

QUrl NetworkExchange::requestUrl() const
{
    return mRequest.url();
}

QByteArray NetworkExchange::requestRawHeader(const QByteArray & headerName) const
{
    return mRequest.rawHeader(headerName);
}

QVariant NetworkExchange::replyAttribute(QNetworkRequest::Attribute code) const
{
    if (mReply) {
        return mReply->attribute(code);
    }
    return QVariant();
}

QVariant NetworkExchange::replyHeader(QNetworkRequest::KnownHeaders header) const
{
    if (mReply) {
        return mReply->header(header);
    }
    return QVariant();
}

QByteArray NetworkExchange::replyRawHeader(const QByteArray & headerName) const
{
    if (mReply) {
        return mReply->rawHeader(headerName);
    }
    return QByteArray();
}

QByteArray NetworkExchange::readAll()
{
    if (mReply) {
        return mReply->readAll();
    }
    return QByteArray();
}

void NetworkExchange::abort()
{
    if (mReply) {
        mReply->abort();
    }
}

QString NetworkExchange::errorString() const
{
    return mErrorString;
}

void NetworkExchange::setErrorString(const QString & str)
{
    mErrorString = str;
}

void NetworkExchange::start(Method method, QIODevice *data /* = 0 */)
{
    if (!mAccessManager) {
        mAccessManager = new QNetworkAccessManager(this);
    }
    
    mReplyReceived = false;
    
    QUrl url = mRequest.url();
    mRequest.setRawHeader("Host", url.host().toUtf8());
    
    mUrlsVisited.append(url);
    
    switch(method) {
    case GetMethod:
        mReply = mAccessManager->get(mRequest);
        break;
    case HeadMethod:
        mReply = mAccessManager->head(mRequest);
        break;
    case PostMethod:
        mReply = mAccessManager->post(mRequest, data);
        break;
    case PutMethod:
        mReply = mAccessManager->put(mRequest, data);
        break;
    case DeleteMethod:
        mReply = mAccessManager->deleteResource(mRequest);
        break;
    }
    
    //dumpRequestInfo();
    
    if (mReply) {
        mReply->setParent(this);
        
        connect(mReply, SIGNAL(metaDataChanged()), SLOT(onMetaDataChanged()));
        connect(mReply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
        connect(mReply, SIGNAL(uploadProgress(qint64, qint64)), SLOT(onUploadProgress(qint64, qint64)));
        connect(mReply, SIGNAL(readyRead()), SLOT(onReadyRead()));
        connect(mReply, SIGNAL(finished()), SLOT(onFinished()));
        connect(mReply, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(onSslErrors(const QList<QSslError> &)));
        connect(mReply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    }
}

void NetworkExchange::redirect(const QUrl & url)
{
    emit redirected(url);
    
    // Set the redirect URL on the request.  By reusing the existing request (as
    // opposed to creating a new one), the existing headers are preserved.
    mRequest.setUrl(url);
    get();
}

void NetworkExchange::setMaxRedirects(qint32 max)
{
    mMaxRedirects = max;
}

void NetworkExchange::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    mAccessManager = manager;
}

void NetworkExchange::onMetaDataChanged()
{
    if (!mReplyReceived) {
        mReplyReceived = true;
        emit replyReceived();
    }
    
    QUrl url = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (url.isRelative() && !url.isEmpty()) {
        url = mReply->url().resolved(url);
    }
    
    if (!url.isEmpty()) {
        // Headers have been received for this reply, and a redirection has been
        // issued by the server.  As an optimization (and, as experience
        // suggests, a drastic one), the reply will be aborted and the redirect
        // followed.  By doing this, we don't have to wait for the reply to
        // finish, which could potentially take a while if it also has content
        // in the body, leaving other pending requests queued in the networking
        // layer.
        QNetworkReply *redirectReply = mReply;
        redirectReply->disconnect(this);
        redirectReply->abort();
        
        if (mUrlsVisited.size() > mMaxRedirects) {
            setErrorString("Too many redirections");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        } else if (mUrlsVisited.contains(url)) {
            setErrorString("Infinite redirection loop detected");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        }
        
        redirect(url);
        
        redirectReply->deleteLater();
        redirectReply = 0;
    }
}

void NetworkExchange::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

void NetworkExchange::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

void NetworkExchange::onReadyRead()
{
    if (!mReplyReceived) {
        mReplyReceived = true;
        emit replyReceived();
    }
    
    emit readyRead();
}

void NetworkExchange::onFinished()
{
    if (!mReplyReceived) {
        mReplyReceived = true;
        emit replyReceived();
    }
    
    //dumpReplyInfo();
    //QByteArray data = mReply->readAll();
    //qDebug() << data.data();
    
    
    // The QNetworkRequest::RedirectionTargetAttribute attribute is set on a
    // reply in which the server is redirecting the request.  For HTTP requests,
    // this means the status code is 3xx.
    QUrl url = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (url.isRelative() && !url.isEmpty()) {
        url = mReply->url().resolved(url);
    }
    
    if (!url.isEmpty()) {
        QNetworkReply *redirectReply = mReply;
    
        if (mUrlsVisited.size() > mMaxRedirects) {
            setErrorString("Too many redirections");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        } else if (mUrlsVisited.contains(url)) {
            setErrorString("Infinite redirection loop detected");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        }
        
        redirect(url);
        
        redirectReply->deleteLater();
        redirectReply = 0;
    } else {
        emit finished();
    }
}

void NetworkExchange::onError(QNetworkReply::NetworkError code)
{
    // NOTE: According to Qt 4.7 documentation, the finished() signal will
    //       "probably" follow.  In all cases encountered, this seems to be the
    //       case, so finalizing the connection is deferred until then.
    //       See: http://doc.trolltech.com/4.7/qnetworkreply.html#error-2
    
    setErrorString(mReply->errorString());
    emit error(code);
}

void NetworkExchange::onSslErrors(const QList<QSslError> & errors)
{
    qDebug() << "!! NetworkExchange::onSslErrors";
    
    // FIXME: Take this out
    mReply->ignoreSslErrors();
    
    Q_UNUSED(errors);
}

void NetworkExchange::dumpRequestInfo()
{
    qDebug() << "NETWORK REQUEST:";
    foreach(QByteArray header, mRequest.rawHeaderList()) {
        qDebug() << header.data() << ":" << mRequest.rawHeader(header).data();
    }
}

void NetworkExchange::dumpReplyInfo()
{
    int code = mReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    qDebug() << "NETWORK REPLY: " << code;
    foreach(QNetworkReply::RawHeaderPair pair, mReply->rawHeaderPairs()) {
        qDebug() << pair.first.data() << ":" << pair.second.data();
    }
}


QTX_END_NAMESPACE
