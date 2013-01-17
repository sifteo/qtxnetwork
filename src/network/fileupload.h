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
    typedef enum {
        UserError       = 1023,
        TimeoutError    = UserError + 1,
        FileErrorDomain = 2056
    } Error;

public:
    //FileUpload(NetworkExchange *connection);
    FileUpload(const QNetworkRequest & request);
    virtual ~FileUpload();
    
    void start();
    void abort();
    
    QString sourcePath() const;
    
    void setUploadPath(const QString & path);
    void setDeleteWhenFinished(bool autoDelete = true);
    
    quint32 error() const;
    QString errorString() const;
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
signals:
    void started();
    void progress(qint64 partial, qint64 total);
    void finished();
    void error(QNetworkReply::NetworkError code);
    
protected:
    void setError(quint32 code, const QString & string);
    
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
    
    quint32 mError;
    QString mErrorString;
    
    bool mDeleteWhenFinished;
    
    QNetworkAccessManager *mAccessManager;
};


QTX_END_NAMESPACE

#endif // FILEUPLOAD_H
