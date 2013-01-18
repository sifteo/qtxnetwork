#ifndef QTXNETWORK_FILETRANSFER_P_H
#define QTXNETWORK_FILETRANSFER_P_H

#include "filetransfer.h"

QTX_BEGIN_NAMESPACE


class FileTransferPrivate
{
public:
    FileTransferPrivate(FileTransfer *q);
    virtual ~FileTransferPrivate();
    
public:
    FileTransfer *q_ptr;
    Q_DECLARE_PUBLIC(FileTransfer);
    
    QNetworkReply::NetworkError errorCode;
    QString errorString;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILETRANSFER_P_H
