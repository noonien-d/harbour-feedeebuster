#include "fbdownloader.h"

#include <QCryptographicHash>
#include <QFile>
#include <QImage>
#include <QDir>
#include <QNetworkRequest>

FBDownload::FBDownload (QString url, QString localurl, FBDownloader *downloader) : QObject(NULL)
{
    mUrl = url;
    mFilepath = localurl;
    mDownloader = downloader;
    mFile = NULL;
}

void FBDownload::startDownload ()
{
    qDebug() << "request url " << mUrl;

    QNetworkRequest request(mUrl);

    if (mRequestTimestamp.isValid())
    {
        QLocale locale_us(QLocale::C, QLocale::AnyCountry);
        request.setRawHeader(QString("If-Modified-Since").toUtf8(), (locale_us.toString(mRequestTimestamp, "ddd, dd MMM yyyy HH:mm:ss") + " GMT").toUtf8());
    }

    mReply = mDownloader->mAccessManager.get(request);

    connect(mReply, &QNetworkReply::readyRead,
            this, &FBDownload::onReadyRead);
    connect(mReply, &QNetworkReply::finished,
            this, &FBDownload::onDownloaded);
}

void FBDownload::onDownloaded ()
{
    qDebug() << "error: " << mReply->error();

    if (mReply->size() == 0)
    {
        qDebug() << "got zero size, check header..." << mUrl;

        foreach (QNetworkReply::RawHeaderPair pair, mReply->rawHeaderPairs())
        {
            qDebug() << QString::fromUtf8(pair.first) + " = " << QString::fromUtf8(pair.second);

            if (QString::fromUtf8(pair.first).contains("Location"))
            {
                QString newurl = pair.second;

                if (newurl.at(0) == '/')
                    newurl = mReply->url().toString(QUrl::RemovePath) + newurl;

                qDebug() << "got relocation to " << newurl;
                QNetworkRequest request(newurl);

                if (mRequestTimestamp.isValid())
                {
                    QLocale locale_us(QLocale::C, QLocale::AnyCountry);
                    request.setRawHeader(QString("If-Modified-Since").toUtf8(), (locale_us.toString(mRequestTimestamp, "ddd, dd MMM yyyy HH:mm:ss") + " GMT").toUtf8());
                }

                // overwrite possibly written relocation data
                if (mFile)
                    mFile->seek(0);

                mReply = mDownloader->mAccessManager.get(request);

                connect(mReply, &QNetworkReply::readyRead,
                        this, &FBDownload::onReadyRead);
                connect(mReply, &QNetworkReply::finished,
                        this, &FBDownload::onDownloaded);
            }
        }
    }
    else if (mReply->error() == 0)
    {
        if (mFile == NULL)
        {
            mFile = new QFile(mFilepath + ".part");
            mFile->open(QFile::WriteOnly);
        }

        mFile->write(mReply->readAll());
        mFile->close();

        QFile oldfile(mFilepath);
        if (oldfile.exists())
            oldfile.remove();

        if (!mFile->rename(mFilepath))
            qWarning() << "Renaming failed";

        qDebug() << "download ready";

        emit downloadReady(mUrl, mFilepath);

        mDownloader->onDownloadReady(this);
    }
}

void FBDownload::onReadyRead()
{
    if (mFile == NULL)
    {
        mFile = new QFile(mFilepath + ".part");
        mFile->open(QFile::WriteOnly);
    }

    mFile->write(mReply->read(1024));
}


FBDownloader::FBDownloader(QString basepath, bool preservenames, QObject *parent) : QObject(parent)
{
    mPreserveNames = preservenames;

    mBasepath = basepath + QDir::separator();

    qDebug() << "Download Path: " << mBasepath;

    QDir dir(mBasepath);

    if (!dir.exists())
        dir.mkpath(".");
}
FBDownloader::~FBDownloader()
{
    qDebug() << "Destroy downloader";
    foreach (QString url, mPendingUrl)
    {
        qDebug() << "Pending download: " << url;
    }
}

void FBDownloader::startDownload(QString url)
{
    if (url.isEmpty())
        return;

    QString localurl = getLocalPath(url);

    QFile file(localurl);
    if (file.exists())
    {
//        qDebug() << "file load file from cache";
        emit downloadReady(url, localurl);
    }
    else
    {
        if (mPendingUrl.indexOf(url) != -1)
        {
//            qDebug() << "avoid double download";
            return;
        }
        if ((url.contains("http://")) || (url.contains("https://")))
        {
            FBDownload *dl = new FBDownload(url, localurl, this);

            mPendingUrl.append(url);
            dl->startDownload();
        }
        else
        {
//            qDebug() << "is local file";
            emit downloadReady(url, localurl);
        }
    }
}

void FBDownloader::onDownloadReady (FBDownload* download)
{
    qDebug ("download ready");

    mPendingUrl.removeOne(download->mUrl);

    emit downloadReady(download->mUrl, download->mFilepath);
}

void FBDownloader::startDownloadUpdate(QString url, QString localurl)
{
    QFile file(localurl);
    if (file.exists())
    {
        qDebug() << "start download if updated";

        QFileInfo finfo(localurl);

        FBDownload *dl = new FBDownload(url, localurl, this);
        mPendingUrl.append(url);

        dl->mRequestTimestamp = finfo.lastModified().toUTC();
        dl->startDownload();
    }
    else
    {
        qDebug() << "file to update doesn't exist, should not happen, download anyway :P";
        startDownload(url);
    }
}

bool FBDownloader::isReady (QString url)
{
    if (url.isEmpty())
        return false;

    QFile file(getLocalPath(url));
    return file.exists();
}

QString FBDownloader::getLocalPath (QString url)
{
    if (url.isEmpty())
        return "";

    if (mPreserveNames)
        return mBasepath + QFileInfo(url).fileName();
    else
        return mBasepath + QString::fromUtf8(QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex());
}

bool FBDownloader::allDownloadsFinished()
{
    return mPendingUrl.empty();
}
