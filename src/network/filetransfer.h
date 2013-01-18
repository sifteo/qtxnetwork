#ifndef QTXNETWORK_FILETRANSFER_H
#define QTXNETWORK_FILETRANSFER_H

#include "networkglobal.h"
#include <QtNetwork>
#include <QtCore>

QTX_BEGIN_NAMESPACE


class FileTransferPrivate;

class FileTransfer : public QObject
{
    Q_OBJECT

public:
    FileTransfer(QObject *parent = 0);
    virtual ~FileTransfer();
    
    virtual void start() = 0;
    virtual void abort();
    
    QString path() const;
    
    void setPath(const QString & path);
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    void setAutoDelete(bool autoDelete = true);
    
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    
signals:
    void started();
    void redirected(const QUrl & url);
    void progress(qint64 partial, qint64 total);
    void finished();
    void error(QNetworkReply::NetworkError code);
    
protected:
    bool autoDelete() const;
    void setError(QNetworkReply::NetworkError errorCode, const QString & errorString);
    
protected:
    FileTransfer(FileTransferPrivate &d, QObject *parent = 0);
    FileTransferPrivate *d_ptr;
private:
    Q_DECLARE_PRIVATE(FileTransfer);
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILETRANSFER_H
