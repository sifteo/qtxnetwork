#ifndef QTXNETWORK_FILEDOWNLOAD_P_H
#define QTXNETWORK_FILEDOWNLOAD_P_H

#include "filedownload.h"
#include "filetransfer_p.h"

QTX_BEGIN_NAMESPACE


class FileDownloadPrivate : public FileTransferPrivate
{
public:
    FileDownloadPrivate(FileTransfer *q)
        : FileTransferPrivate(q),
          bytesReceived(0),
          bytesTotal(0),
          expectedFileSize(-1),
          rxTimeout(60000),  // 60000 msec = 60 sec = 1 min
          isRedirecting(false)
        { }
    
    qint64 bytesReceived;
    qint64 bytesTotal;
    qint64 expectedFileSize;
    QTimer rxTimer;
    int rxTimeout;
    bool isRedirecting;
};


QTX_END_NAMESPACE

#endif // QTXNETWORK_FILEDOWNLOAD_P_H
