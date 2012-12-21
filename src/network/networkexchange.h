#ifndef NETWORKEXCHANGE_H
#define NETWORKEXCHANGE_H

#include <QtCore>
#include <QtNetwork>

// high level wrapper for QNetworkRequest / QNetworkReply pair, providing facilities for following redirects

class NetworkExchange : public QObject
{
    Q_OBJECT
    
public:
    typedef enum {
        GetMethod,
        HeadMethod,
        PostMethod,
        PutMethod,
        DeleteMethod
    } Method;
    
    typedef enum {
        UserError             = 511,
        TooManyRedirectsError = UserError + 1,
        RedirectLoopError     = UserError + 2,
    } Error;
    
public:
    NetworkExchange(const QNetworkRequest & request, QObject *parent = 0);
    virtual ~NetworkExchange();
    
    void get();
    void head();
    void post(QIODevice *data);
    void put(QIODevice *data);
    void deleteResource();
    
    QUrl requestUrl() const;
    QVariant replyAttribute(QNetworkRequest::Attribute code) const;
    QVariant replyHeader(QNetworkRequest::KnownHeaders header) const;
    QByteArray readAll();
    void abort();
    
    QString errorString() const;
    
    QNetworkAccessManager * networkAccessManager() const;
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void replyReceived();
    void readyRead();
    void redirected(const QUrl & url);
    void finished();
    void error(quint32 code);
    
private:
    void start(Method method, QIODevice *data = 0);
    void redirect(const QUrl & url);
    
    void dumpRequestInfo();
    void dumpReplyInfo();
    
private slots:
    void onMetaDataChanged();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onReadyRead();
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onSslErrors(const QList<QSslError> & errors);

private:
    QNetworkAccessManager *mAccessManager;
    QNetworkRequest mRequest;
    QNetworkReply *mReply;
    Method mMethod;
    
    QList<QUrl> mUrlsVisited;
    qint32 mMaxRedirects;
    
    bool mReplyReceived;
    
    QString mErrorString;
};

#endif // NETWORKEXCHANGE_H
