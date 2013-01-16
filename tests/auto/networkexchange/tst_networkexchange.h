#ifndef TST_NETWORKEXCHANGE_H
#define TST_NETWORKEXCHANGE_H

#include "networkexchange.h"
#include <QtxMockNetwork>
#include <QtTest/QtTest>

QTX_USE_NAMESPACE


class tst_NetworkExchange : public QObject,
                            public IMockNetworkAccessManagerDelegate
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();
    
    void testGet();
    void testPost();
    
private:
    QEventLoop mEventLoop;
    MockNetworkAccessManager *mNetAccessManager;
    NetworkExchange *mExchange;
    QBuffer *mBuffer;
    QByteArray mData;
    
public:
    QIODevice *createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData = 0);
    
public slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError code);
    void onTimeout();
};

#endif // TST_NETWORKEXCHANGE_H
