#ifndef FILEUPLOAD_H
#define FILEUPLOAD_H

#include "networkglobal.h"
#include "filetransfer.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class NetworkExchange;

// TODO: Much of this can be factored out into FileTransfer, shared with FileDownload
class FileUpload : public FileTransfer
{
    Q_OBJECT

public:
    //FileUpload(NetworkExchange *connection);
    FileUpload(const QNetworkRequest & request);
    virtual ~FileUpload();
    
    void start();
    void abort();
    
    QString sourcePath() const;
    
    void setUploadPath(const QString & path);
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
private slots:
    void onReplyReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onReadyRead();
    void onRedirected(const QUrl & url);
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onDataTxTimeout();
    
private:
    QNetworkRequest mRequest;
    NetworkExchange *mConnection;
    QUrl mOriginalUrl;
    
    QFile *mFile;
    QString mPath;
    qint64 mBytesSent;
    qint64 mBytesTotal;
    //qint64 mExpectedFileSize;
    
    QTimer mDataTxTimer;
    int mDataTxTimeout;
    bool mRedirecting;
    
    QNetworkAccessManager *mAccessManager;
};


QTX_END_NAMESPACE

#endif // FILEUPLOAD_H
