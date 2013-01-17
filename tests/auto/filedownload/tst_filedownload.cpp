#include "tst_filedownload.h"
#include "networkexchange.h"


void tst_FileDownload::initTestCase()
{
    QDir dir;
    dir.mkpath("tmp");

    mNetAccessManager = new MockNetworkAccessManager();
    mNetAccessManager->setDelegate(this);
}

void tst_FileDownload::cleanupTestCase()
{
    if (mNetAccessManager) {
        delete mNetAccessManager;
        mNetAccessManager = 0;
    }
    
    QDir dir;
    dir.rmpath("tmp");
}

void tst_FileDownload::init()
{
    mDownload = 0;

    QTimer::singleShot(10000, this, SLOT(onTimeout()));
}

void tst_FileDownload::cleanup()
{
    if (mDownload) {
        delete mDownload;
        mDownload = 0;
    }
    
    QFile::remove("tmp/download.dat");
}

void tst_FileDownload::testDownload()
{
    QNetworkRequest request(QUrl("http://www.example.com/files/hello-world.txt"));
    
    mDownload = new FileDownload(request);
    mDownload->setDownloadPath("tmp/download.dat");
    mDownload->setNetworkAccessManager(mNetAccessManager);
    connect(mDownload, SIGNAL(finished()), SLOT(onFinished()));
    connect(mDownload, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onError(QNetworkReply::NetworkError)));
    
    QSignalSpy finishedSpy(mDownload, SIGNAL(finished()));
    QSignalSpy errorSpy(mDownload, SIGNAL(error(QNetworkReply::NetworkError)));
    QList<QVariant> arguments;
    
    mDownload->start();
    mEventLoop.exec();
    
    QVERIFY(finishedSpy.count() == 1);
    QVERIFY(errorSpy.count() == 0);
    
    QFile file("tmp/download.dat");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    QCOMPARE(QString::fromUtf8(data), QString("Hello World!"));
}

QIODevice *tst_FileDownload::createIncomingData(const QNetworkRequest & req, QIODevice * outgoingData /* = 0 */)
{
    Q_UNUSED(outgoingData)

    QString path = req.url().path();
    QFile *file = 0;
    
    if ("/files/hello-world.txt" == path) {
        file = new QFile("data/hello-world.http");
    }
    
    if (file) {
        file->open(QIODevice::ReadOnly);
        return file;
    }
    return 0;
}

void tst_FileDownload::onFinished()
{
    mEventLoop.quit();
}

void tst_FileDownload::onError(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code)

    //qDebug() << "tst_FileDownload::onError";
    //qDebug() << "  error: " << code;
    //qDebug() << "  string: " << mChecker->errorString();
    
    mEventLoop.quit();
}

void tst_FileDownload::onTimeout()
{
    mEventLoop.quit();
}


QTEST_MAIN(tst_FileDownload)
