#ifndef FILETRANSFER_H
#define FILETRANSFER_H

#include <QtCore>


// TODO: Much of this can be factored out into FileTransfer, shared with FileDownload
class FileTransfer : public QObject
{
    Q_OBJECT

public:
    FileTransfer();
    virtual ~FileTransfer();
    
    virtual void start() = 0;
    virtual void abort() = 0;
    
    virtual void setDeleteWhenFinished(bool autoDelete = true) = 0;
};

#endif // FILETRANSFER_H
