#ifndef FILEDOWNLOAD_H
#define FILEDOWNLOAD_H

#include <QtCore>
#include "filetransfer.h"


class NetworkExchange;

class FileDownload : public FileTransfer
{
    Q_OBJECT

public:
    typedef enum {
        UserError       = 1023,
        TimeoutError    = UserError + 1,
        FileErrorDomain = 2056
    } Error;

public:
    FileDownload(NetworkExchange *connection);
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
    void setDeleteWhenFinished(bool autoDelete = true);
    
    quint32 error() const;
    QString errorString() const;
    
signals:
    void started();
    void redirected(const QUrl & url);
    void progress(qint64 partial, qint64 total);
    void finished();
    void error(quint32 code);
    
protected:
    void setError(quint32 code, const QString & string);
    
private slots:
    void onReplyReceived();
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onReadyRead();
    void onRedirected(const QUrl & url);
    void onFinished();
    void onError(quint32 code);
    void onDataRxTimeout();
    
private:
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
    
    quint32 mError;
    QString mErrorString;
    
    bool mDeleteWhenFinished;
};

#endif // FILEDOWNLOAD_H
