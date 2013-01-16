#include "tst_networkexchange.h"


void tst_NetworkExchange::initTestCase()
{
    mNetAccessManager = new MockNetworkAccessManager();
    mNetAccessManager->setDelegate(this);
}

void tst_NetworkExchange::cleanupTestCase()
{
    if (mNetAccessManager) {
        delete mNetAccessManager;
        mNetAccessManager = 0;
    }
}

void tst_NetworkExchange::init()
{
    mExchange = 0;
    mBuffer = 0;

    QTimer::singleShot(10000, this, SLOT(onTimeout()));
}

void tst_NetworkExchange::cleanup()
{
    if (mBuffer) {
        delete mBuffer;
        mBuffer = 0;
    }

    if (mExchange) {
        delete mExchange;
        mExchange = 0;
    }
}

void tst_NetworkExchange::testGet()
{
    QNetworkRequest request(QUrl("http://www.example.com/hello-world.txt"));
    mExchange = new NetworkExchange(request);
    mExchange->setNetworkAccessManager(mNetAccessManager);
    connect(mExchange, SIGNAL(finished()), SLOT(onFinished()));
    connect(mExchange, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));

    QSignalSpy replyReceivedSpy(mExchange, SIGNAL(replyReceived()));
    QSignalSpy redirectedSpy(mExchange, SIGNAL(redirected(const QUrl &)));
    QSignalSpy finishedSpy(mExchange, SIGNAL(finished()));
    QSignalSpy errorSpy(mExchange, SIGNAL(error(QNetworkReply::NetworkError)));
    QList<QVariant> arguments;

    mExchange->get();
    mEventLoop.exec();

    QVERIFY(replyReceivedSpy.count() == 1);
    QVERIFY(redirectedSpy.count() == 0);
    QVERIFY(finishedSpy.count() == 1);
    QVERIFY(errorSpy.count() == 0);
    
    QCOMPARE(mExchange->requestUrl(), QUrl("http://www.example.com/hello-world.txt"));
    QCOMPARE(QString::fromUtf8(mExchange->requestRawHeader("Host")), QString("www.example.com"));
    QCOMPARE(mExchange->replyAttribute(QNetworkRequest::HttpReasonPhraseAttribute).toString(), QString("OK"));
    QCOMPARE(QString::fromUtf8(mExchange->replyRawHeader("Content-Type")), QString("text/plain"));
    QCOMPARE(QString::fromUtf8(mExchange->readAll()), QString("Hello World!"));
}

void tst_NetworkExchange::testPost()
{
    QNetworkRequest request(QUrl("http://www.example.com/hello-world-post.txt"));
    mData = QString("FOO").toUtf8();
    mBuffer = new QBuffer(&mData);
    
    mExchange = new NetworkExchange(request);
    mExchange->setNetworkAccessManager(mNetAccessManager);
    connect(mExchange, SIGNAL(finished()), SLOT(onFinished()));
    connect(mExchange, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));

    QSignalSpy replyReceivedSpy(mExchange, SIGNAL(replyReceived()));
    QSignalSpy redirectedSpy(mExchange, SIGNAL(redirected(const QUrl &)));
    QSignalSpy finishedSpy(mExchange, SIGNAL(finished()));
    QSignalSpy errorSpy(mExchange, SIGNAL(error(QNetworkReply::NetworkError)));
    QList<QVariant> arguments;

    mExchange->post(mBuffer);
    mEventLoop.exec();

    QVERIFY(replyReceivedSpy.count() == 1);
    QVERIFY(redirectedSpy.count() == 0);
    QVERIFY(finishedSpy.count() == 1);
    QVERIFY(errorSpy.count() == 0);
    
    QCOMPARE(mExchange->requestUrl(), QUrl("http://www.example.com/hello-world-post.txt"));
    QCOMPARE(QString::fromUtf8(mExchange->requestRawHeader("Host")), QString("www.example.com"));
    QCOMPARE(mExchange->replyAttribute(QNetworkRequest::HttpReasonPhraseAttribute).toString(), QString("OK"));
    QCOMPARE(QString::fromUtf8(mExchange->replyRawHeader("Content-Type")), QString("text/plain"));
}

QIODevice *tst_NetworkExchange::createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData /* = 0 */)
{
    QString path = req.url().path();
    QFile *file = 0;
    
    if ("/hello-world.txt" == path) {
        file = new QFile("data/hello-world.http");
    } else if ("/hello-world-post.txt" == path) {
        QByteArray data = outgoingData->readAll();
        if ("FOO" != QString::fromUtf8(data)) {
            return 0;
        }
    
        file = new QFile("data/hello-world.http");
    }
    
    if (file) {
        file->open(QIODevice::ReadOnly);
        return file;
    }
    return 0;
}

void tst_NetworkExchange::onFinished()
{
    mEventLoop.quit();
}

void tst_NetworkExchange::onError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)

    //qDebug() << "tst_NetworkExchange::onError";
    //qDebug() << "  error: " << code;
    //qDebug() << "  string: " << mChecker->errorString();
    
    mEventLoop.quit();
}

void tst_NetworkExchange::onTimeout()
{
    mEventLoop.quit();
}


QTEST_MAIN(tst_NetworkExchange)
