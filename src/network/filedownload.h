#ifndef QTXNETWORK_FILEDOWNLOAD_H
#define QTXNETWORK_FILEDOWNLOAD_H

#include "networkglobal.h"
#include "filetransfer.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class NetworkExchange;

class FileDownload : public FileTransfer
{
    Q_OBJECT

public:
    FileDownload(const QNetworkRequest & request);
    virtual ~FileDownload();
    
    void start();
    void abort();
    
    QUrl currentUrl() const;
    QUrl originalUrl() const;
    QString destinationPath() const;
    qint64 bytesReceived() const;
    qint64 bytesTotal() const;
    
    void setDownloadPath(const QString & path);
    void setExpectedFileSize(qint64 size);
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
private slots:
    void onReplyReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReadyRead();
    void onRedirected(const QUrl & url);
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onDataRxTimeout();
    
private:
    QNetworkRequest mRequest;
    NetworkExchange *mConnection;
    QUrl mOriginalUrl;
    
    QFile *mFile;
    QString mPath;
    qint64 mBytesReceived;
    qint64 mBytesTotal;
    qint64 mExpectedFileSize;
    
    QTimer mDataRxTimer;
    int mDataRxTimeout;
    bool mRedirecting;
    
    QNetworkAccessManager *mAccessManager;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILEDOWNLOAD_H
