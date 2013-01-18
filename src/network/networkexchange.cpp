#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


class NetworkExchangePrivate
{
public:
    typedef enum {
        GetMethod,
        HeadMethod,
        PostMethod,
        PutMethod,
        DeleteMethod
    } Method;

    NetworkExchangePrivate(NetworkExchange *q);
    virtual ~NetworkExchangePrivate();
    
    void start(Method method, QIODevice *data = 0);
    void redirect(const QUrl & url);
    
    void dumpRequestInfo();
    void dumpReplyInfo();
    
public:
    NetworkExchange *q_ptr;
    Q_DECLARE_PUBLIC(NetworkExchange);
    
    QNetworkAccessManager *netAccessManager;
    Method method;
    QNetworkRequest request;
    QNetworkReply *reply;
    
    bool replyReceived;
    QList<QUrl> urlsVisited;
    qint32 maxRedirects;
    
    QString errorString;
};


NetworkExchange::NetworkExchange(const QNetworkRequest & request, QObject *parent /* = 0 */)
    : QObject(parent),
      d_ptr(new NetworkExchangePrivate(this))
{
    d_ptr->request = request;
}

NetworkExchange::~NetworkExchange()
{
    if (d_ptr) {
        delete d_ptr;
        d_ptr = 0;
    }
}

void NetworkExchange::get()
{
    d_ptr->start(NetworkExchangePrivate::GetMethod);
}

void NetworkExchange::head()
{
    d_ptr->start(NetworkExchangePrivate::HeadMethod);
}

void NetworkExchange::post(QIODevice *data)
{
    d_ptr->start(NetworkExchangePrivate::PostMethod, data);
}

void NetworkExchange::put(QIODevice *data)
{
    d_ptr->start(NetworkExchangePrivate::PutMethod, data);
}

void NetworkExchange::deleteResource()
{
    d_ptr->start(NetworkExchangePrivate::DeleteMethod);
}

void NetworkExchange::abort()
{
    if (d_ptr->reply) {
        d_ptr->reply->abort();
    }
}

QUrl NetworkExchange::requestUrl() const
{
    return d_ptr->request.url();
}

QByteArray NetworkExchange::requestRawHeader(const QByteArray & headerName) const
{
    return d_ptr->request.rawHeader(headerName);
}

QVariant NetworkExchange::replyAttribute(QNetworkRequest::Attribute code) const
{
    if (d_ptr->reply) {
        return d_ptr->reply->attribute(code);
    }
    return QVariant();
}

QVariant NetworkExchange::replyHeader(QNetworkRequest::KnownHeaders header) const
{
    if (d_ptr->reply) {
        return d_ptr->reply->header(header);
    }
    return QVariant();
}

QByteArray NetworkExchange::replyRawHeader(const QByteArray & headerName) const
{
    if (d_ptr->reply) {
        return d_ptr->reply->rawHeader(headerName);
    }
    return QByteArray();
}

QByteArray NetworkExchange::readAll()
{
    if (d_ptr->reply) {
        return d_ptr->reply->readAll();
    }
    return QByteArray();
}

void NetworkExchange::setMaxRedirects(qint32 max)
{
    d_ptr->maxRedirects = max;
}

void NetworkExchange::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    d_ptr->netAccessManager = manager;
}

QString NetworkExchange::errorString() const
{
    return d_ptr->errorString;
}

void NetworkExchange::setErrorString(const QString & str)
{
    d_ptr->errorString = str;
}

void NetworkExchange::onMetaDataChanged()
{
    if (!d_ptr->replyReceived) {
        d_ptr->replyReceived = true;
        emit replyReceived();
    }
    
    QUrl url = d_ptr->reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (url.isRelative() && !url.isEmpty()) {
        url = d_ptr->reply->url().resolved(url);
    }
    
    if (!url.isEmpty()) {
        // Headers have been received for this reply, and a redirection has been
        // issued by the server.  As an optimization (and, as experience
        // suggests, a drastic one), the reply will be aborted and the redirect
        // followed.  By doing this, we don't have to wait for the reply to
        // finish, which could potentially take a while if it also has content
        // in the body, leaving other pending requests queued in the networking
        // layer.
        QNetworkReply *redirectReply = d_ptr->reply;
        redirectReply->disconnect(this);
        redirectReply->abort();
        
        if (d_ptr->urlsVisited.size() > d_ptr->maxRedirects) {
            setErrorString("Too many redirections");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        } else if (d_ptr->urlsVisited.contains(url)) {
            setErrorString("Infinite redirection loop detected");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        }
        
        d_ptr->redirect(url);
        
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
    if (!d_ptr->replyReceived) {
        d_ptr->replyReceived = true;
        emit replyReceived();
    }
    
    emit readyRead();
}

void NetworkExchange::onFinished()
{
    if (!d_ptr->replyReceived) {
        d_ptr->replyReceived = true;
        emit replyReceived();
    }
    
    //dumpReplyInfo();
    //QByteArray data = reply->readAll();
    //qDebug() << data.data();
    
    
    // The QNetworkRequest::RedirectionTargetAttribute attribute is set on a
    // reply in which the server is redirecting the request.  For HTTP requests,
    // this means the status code is 3xx.
    QUrl url = d_ptr->reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (url.isRelative() && !url.isEmpty()) {
        url = d_ptr->reply->url().resolved(url);
    }
    
    if (!url.isEmpty()) {
        QNetworkReply *redirectReply = d_ptr->reply;
    
        if (d_ptr->urlsVisited.size() > d_ptr->maxRedirects) {
            setErrorString("Too many redirections");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        } else if (d_ptr->urlsVisited.contains(url)) {
            setErrorString("Infinite redirection loop detected");
            emit error(QNetworkReply::ProtocolUnknownError);
            emit finished();
            return;
        }
        
        d_ptr->redirect(url);
        
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
    
    setErrorString(d_ptr->reply->errorString());
    emit error(code);
}

void NetworkExchange::onSslErrors(const QList<QSslError> & errors)
{
    qDebug() << "!! NetworkExchange::onSslErrors";
    
    // FIXME: Take this out
    d_ptr->reply->ignoreSslErrors();
    
    Q_UNUSED(errors);
}


NetworkExchangePrivate::NetworkExchangePrivate(NetworkExchange *q)
    : q_ptr(q),
      netAccessManager(0),
      method(GetMethod),
      reply(0),
      replyReceived(false),
      maxRedirects(5)
{
}

NetworkExchangePrivate::~NetworkExchangePrivate()
{
}

void NetworkExchangePrivate::start(Method method, QIODevice *data /* = 0 */)
{
    if (!netAccessManager) {
        netAccessManager = new QNetworkAccessManager(q_ptr);
    }
    
    replyReceived = false;
    
    QUrl url = request.url();
    request.setRawHeader("Host", url.host().toUtf8());
    
    urlsVisited.append(url);
    
    switch(method) {
    case GetMethod:
        reply = netAccessManager->get(request);
        break;
    case HeadMethod:
        reply = netAccessManager->head(request);
        break;
    case PostMethod:
        reply = netAccessManager->post(request, data);
        break;
    case PutMethod:
        reply = netAccessManager->put(request, data);
        break;
    case DeleteMethod:
        reply = netAccessManager->deleteResource(request);
        break;
    }
    
    //dumpRequestInfo();
    
    if (reply) {
        reply->setParent(q_ptr);
        
        q_ptr->connect(reply, SIGNAL(metaDataChanged()), SLOT(onMetaDataChanged()));
        q_ptr->connect(reply, SIGNAL(downloadProgress(qint64, qint64)), SLOT(onDownloadProgress(qint64, qint64)));
        q_ptr->connect(reply, SIGNAL(uploadProgress(qint64, qint64)), SLOT(onUploadProgress(qint64, qint64)));
        q_ptr->connect(reply, SIGNAL(readyRead()), SLOT(onReadyRead()));
        q_ptr->connect(reply, SIGNAL(finished()), SLOT(onFinished()));
        q_ptr->connect(reply, SIGNAL(sslErrors(const QList<QSslError> &)), SLOT(onSslErrors(const QList<QSslError> &)));
        q_ptr->connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    }
}

void NetworkExchangePrivate::redirect(const QUrl & url)
{
    emit q_ptr->redirected(url);
    
    // Set the redirect URL on the request.  By reusing the existing request (as
    // opposed to creating a new one), the existing headers are preserved.
    request.setUrl(url);
    q_ptr->get();
}

void NetworkExchangePrivate::dumpRequestInfo()
{
    qDebug() << "NETWORK REQUEST:";
    foreach(QByteArray header, request.rawHeaderList()) {
        qDebug() << header.data() << ":" << request.rawHeader(header).data();
    }
}

void NetworkExchangePrivate::dumpReplyInfo()
{
    int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    qDebug() << "NETWORK REPLY: " << code;
    foreach(QNetworkReply::RawHeaderPair pair, reply->rawHeaderPairs()) {
        qDebug() << pair.first.data() << ":" << pair.second.data();
    }
}


QTX_END_NAMESPACE
