#ifndef TST_FILEUPLOAD_H
#define TST_FILEUPLOAD_H

#include "fileupload.h"
#include <QtxMockNetwork>
#include <QtTest/QtTest>

QTX_USE_NAMESPACE


class tst_FileUpload : public QObject,
                       public IMockNetworkAccessManagerDelegate
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testUpload();
    void testErrorResponse();
    
private:
    QEventLoop mEventLoop;
    MockNetworkAccessManager *mNetAccessManager;
    FileUpload *mUpload;
    
public:
    QIODevice *createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData = 0);
    
public slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onTimeout();
};

#endif // TST_FILEUPLOAD_H
