#ifndef QTXNETWORK_FILETRANSFER_P_H
#define QTXNETWORK_FILETRANSFER_P_H

#include "filetransfer.h"
#include "networkexchange.h"

QTX_BEGIN_NAMESPACE


class NetworkExchange;

class FileTransferPrivate
{
public:
    FileTransferPrivate(FileTransfer *q);
    virtual ~FileTransferPrivate();
    
public:
    FileTransfer *q_ptr;
    Q_DECLARE_PUBLIC(FileTransfer);
    
    QNetworkAccessManager *netAccessManager;
    QNetworkRequest request;
    NetworkExchange *exchange;
    
    QFile *file;
    QString path;
    
    bool autoDelete;
    
    QNetworkReply::NetworkError errorCode;
    QString errorString;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILETRANSFER_P_H
