#ifndef QTXNETWORK_FILEUPLOAD_H
#define QTXNETWORK_FILEUPLOAD_H

#include "networkglobal.h"
#include "filetransfer.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class FileUpload : public FileTransfer
{
    Q_OBJECT

public:
    FileUpload(const QNetworkRequest & request, QObject *parent = 0);
    virtual ~FileUpload();
    
    void start();
    
    QString sourcePath() const;
    
    void setSourcePath(const QString & path);
    
private slots:
    void onReplyReceived();
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onReadyRead();
    void onRedirected(const QUrl & url);
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onDataTxTimeout();
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILEUPLOAD_H
