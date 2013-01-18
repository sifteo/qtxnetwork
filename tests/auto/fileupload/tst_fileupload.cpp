#include "tst_fileupload.h"
#include "networkexchange.h"


void tst_FileUpload::initTestCase()
{
    mNetAccessManager = new MockNetworkAccessManager();
    mNetAccessManager->setDelegate(this);
}

void tst_FileUpload::cleanupTestCase()
{
    if (mNetAccessManager) {
        delete mNetAccessManager;
        mNetAccessManager = 0;
    }
}

void tst_FileUpload::init()
{
    mUpload = 0;

    QTimer::singleShot(10000, this, SLOT(onTimeout()));
}

void tst_FileUpload::cleanup()
{
    if (mUpload) {
        delete mUpload;
        mUpload = 0;
    }
}

void tst_FileUpload::testUpload()
{
    QNetworkRequest request(QUrl("http://www.example.com/upload"));
    
    mUpload = new FileUpload(request);
    mUpload->setSourcePath("data/hello-world.txt");
    mUpload->setNetworkAccessManager(mNetAccessManager);
    connect(mUpload, SIGNAL(finished()), SLOT(onFinished()));
    connect(mUpload, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    
    QSignalSpy startedSpy(mUpload, SIGNAL(started()));
    QSignalSpy finishedSpy(mUpload, SIGNAL(finished()));
    QSignalSpy errorSpy(mUpload, SIGNAL(error(QNetworkReply::NetworkError)));
    QList<QVariant> arguments;
    
    mUpload->start();
    mEventLoop.exec();
    
    QVERIFY(startedSpy.count() == 1);
    QVERIFY(finishedSpy.count() == 1);
    QVERIFY(errorSpy.count() == 0);
    
    QCOMPARE(mUpload->sourcePath(), QString("data/hello-world.txt"));
}

void tst_FileUpload::testErrorResponse()
{
    QNetworkRequest request(QUrl("http://www.example.com/error"));
    
    mUpload = new FileUpload(request);
    mUpload->setSourcePath("data/hello-world.txt");
    mUpload->setNetworkAccessManager(mNetAccessManager);
    connect(mUpload, SIGNAL(finished()), SLOT(onFinished()));
    connect(mUpload, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    
    QSignalSpy startedSpy(mUpload, SIGNAL(started()));
    QSignalSpy finishedSpy(mUpload, SIGNAL(finished()));
    QSignalSpy errorSpy(mUpload, SIGNAL(error(QNetworkReply::NetworkError)));
    QList<QVariant> arguments;
    
    mUpload->start();
    mEventLoop.exec();
    
    QVERIFY(startedSpy.count() == 1);
    QVERIFY(finishedSpy.count() == 1);
    QVERIFY(errorSpy.count() == 1);
    
    QCOMPARE(mUpload->sourcePath(), QString("data/hello-world.txt"));
}

QIODevice *tst_FileUpload::createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData /* = 0 */)
{
    QString path = req.url().path();
    QFile *file = 0;
    
    if ("/upload" == path) {
        QByteArray data = outgoingData->readAll();
        if ("Hello World!" != QString::fromUtf8(data)) {
            return 0;
        }
    
        file = new QFile("data/201-created.http");
    } else if ("/error" == path) {
        file = new QFile("data/500-internal-server-error.http");
    }
    
    if (file) {
        file->open(QIODevice::ReadOnly);
        return file;
    }
    return 0;
}

void tst_FileUpload::onFinished()
{
    mEventLoop.quit();
}

void tst_FileUpload::onError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)

    //qDebug() << "tst_FileDownload::onError";
    //qDebug() << "  error: " << code;
    //qDebug() << "  string: " << mChecker->errorString();
    
    mEventLoop.quit();
}

void tst_FileUpload::onTimeout()
{
    mEventLoop.quit();
}


QTEST_MAIN(tst_FileUpload)
