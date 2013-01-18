#ifndef QTXNETWORK_FILEDOWNLOAD_H
#define QTXNETWORK_FILEDOWNLOAD_H

#include "networkglobal.h"
#include "filetransfer.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class FileDownload : public FileTransfer
{
    Q_OBJECT

public:
    FileDownload(const QNetworkRequest & request, QObject *parent = 0);
    virtual ~FileDownload();
    
    void start();
    
    QUrl currentUrl() const;
    QUrl originalUrl() const;
    QString destinationPath() const;
    qint64 bytesReceived() const;
    qint64 bytesTotal() const;
    
    void setDestinationPath(const QString & path);
    void setExpectedFileSize(qint64 size);
    
private slots:
    void onReplyReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReadyRead();
    void onRedirected(const QUrl & url);
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onDataRxTimeout();
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILEDOWNLOAD_H
