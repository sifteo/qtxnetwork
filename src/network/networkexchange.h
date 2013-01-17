#ifndef QTXNETWORK_NETWORKEXCHANGE_H
#define QTXNETWORK_NETWORKEXCHANGE_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE

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
    
public:
    NetworkExchange(const QNetworkRequest & request, QObject *parent = 0);
    virtual ~NetworkExchange();
    
    void get();
    void head();
    void post(QIODevice *data);
    void put(QIODevice *data);
    void deleteResource();
    
    QUrl requestUrl() const;
    QByteArray requestRawHeader(const QByteArray & headerName) const;
    QVariant replyAttribute(QNetworkRequest::Attribute code) const;
    QVariant replyHeader(QNetworkRequest::KnownHeaders header) const;
    QByteArray replyRawHeader(const QByteArray & headerName) const;
    QByteArray readAll();
    void abort();
    
    QString errorString() const;
    
    void setMaxRedirects(qint32 max);
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
signals:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void replyReceived();
    void readyRead();
    void redirected(const QUrl & url);
    void finished();
    void error(QNetworkReply::NetworkError code);
    
protected:
    void setErrorString(const QString & str);
    
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


QTX_END_NAMESPACE

#endif // QTXNETWORK_NETWORKEXCHANGE_H
