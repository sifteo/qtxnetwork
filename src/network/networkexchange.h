#ifndef QTXNETWORK_NETWORKEXCHANGE_H
#define QTXNETWORK_NETWORKEXCHANGE_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE

// high level wrapper for QNetworkRequest / QNetworkReply pair, providing facilities for following redirects

class NetworkExchangePrivate;

class NetworkExchange : public QObject
{
    Q_OBJECT
    
public:
    NetworkExchange(const QNetworkRequest & request, QObject *parent = 0);
    virtual ~NetworkExchange();
    
    void get();
    void head();
    void post(QIODevice *data);
    void put(QIODevice *data);
    void deleteResource();
    void abort();
    
    QUrl requestUrl() const;
    QByteArray requestRawHeader(const QByteArray & headerName) const;
    QVariant replyAttribute(QNetworkRequest::Attribute code) const;
    QVariant replyHeader(QNetworkRequest::KnownHeaders header) const;
    QByteArray replyRawHeader(const QByteArray & headerName) const;
    QByteArray readAll();
    
    void setMaxRedirects(qint32 max);
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    
signals:
    void replyReceived();
    void redirected(const QUrl & url);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void readyRead();
    void finished();
    void error(QNetworkReply::NetworkError code);
    
protected:
    void setError(QNetworkReply::NetworkError code);
    void setErrorString(const QString & str);
    
private slots:
    void onMetaDataChanged();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onReadyRead();
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onSslErrors(const QList<QSslError> & errors);

protected:
    NetworkExchangePrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(NetworkExchange);
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_NETWORKEXCHANGE_H
