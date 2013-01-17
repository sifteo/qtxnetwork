#ifndef TST_FILEDOWNLOAD_H
#define TST_FILEDOWNLOAD_H

#include "filedownload.h"
#include <QtxMockNetwork>
#include <QtTest/QtTest>

QTX_USE_NAMESPACE


class tst_FileDownload : public QObject,
                         public IMockNetworkAccessManagerDelegate
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testDownload();
    
private:
    QEventLoop mEventLoop;
    MockNetworkAccessManager *mNetAccessManager;
    FileDownload *mDownload;
    
public:
    QIODevice *createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData = 0);
    
public slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onTimeout();
};

#endif // TST_FILEDOWNLOAD_H
