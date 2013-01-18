#ifndef QTXNETWORK_FILEUPLOAD_P_H
#define QTXNETWORK_FILEUPLOAD_P_H

#include "fileupload.h"
#include "filetransfer_p.h"

QTX_BEGIN_NAMESPACE


class FileUploadPrivate : public FileTransferPrivate
{
public:
    FileUploadPrivate(FileTransfer *q)
        : FileTransferPrivate(q),
          bytesSent(0),
          bytesTotal(0),
          txTimeout(60000),  // 60000 msec = 60 sec = 1 min
          isRedirecting(false)
        { }
        
    qint64 bytesSent;
    qint64 bytesTotal;
    QTimer txTimer;
    int txTimeout;
    bool isRedirecting;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILEUPLOAD_P_H
