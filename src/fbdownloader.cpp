/* feedeebuster - a feed and podcast application for Sailfish OS.
 * Copyright (C) 2016  Ferdinand Stehle
 *
 * This program is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation;
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program;
 * if not, see <http://www.gnu.org/licenses/>. */

#include "fbdownloader.h"

#include <QCryptographicHash>
#include <QFile>
#include <QImage>
#include <QDir>
#include <QNetworkRequest>

FBDownload::FBDownload (QUrl url, QString localurl, FBDownloader *downloader) : QObject(downloader)
{
    mUrl = url;
    mFilepath = localurl;
    mDownloader = downloader;
    mFile = NULL;
}

FBDownload::~FBDownload()
{
    qDebug() << "destroy " << mUrl;
    if (mFile)
        delete mFile;

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
    const QVariant redirectionTarget = mReply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    if (!redirectionTarget.isNull()) {
        const QUrl newurl = mUrl.resolved(redirectionTarget.toUrl());

        // overwrite possibly written relocation data
        if (mFile)
            mFile->seek(0);

        qDebug() << "got relocation to " << newurl;
        QNetworkRequest request(newurl);

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
        return;
    }
    else if ((mReply->error() == 0) && (mReply->size() > 0))
    {
        if (mFile == NULL)
        {
            mFile = new QFile(mFilepath + ".part");
            if (!mFile->open(QFile::WriteOnly))
            {
                qDebug() << "Error " << mFile->error() << ": " << mFile->errorString();
                emit downloadFailed(this);
                return;
            }
        }

        mFile->write(mReply->readAll());
        mFile->close();

        QFile oldfile(mFilepath);
        if (oldfile.exists())
            oldfile.remove();

        if (!mFile->rename(mFilepath))
        {
            qDebug() << "Error " << mFile->error() << ": " << mFile->errorString();
            emit downloadFailed(this);
        }
        else
        {
            emit downloadReady(this);
        }
    }
    else
    {
        qDebug() << "Error " << mReply->error() << ": " << mReply->errorString();
        emit downloadFailed(this);
    }
}

void FBDownload::onReadyRead()
{
    if (mFile == NULL)
    {
        mFile = new QFile(mFilepath + ".part");
        if (!mFile->open(QFile::WriteOnly))
        {
            qDebug() << "Failed to open intermediate file for " << mFilepath;

            delete mFile;
            mFile = NULL;

            return;
        }
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
    foreach (QUrl url, mPendingUrl)
    {
        qDebug() << "Pending download: " << url.toString();
    }
}

void FBDownloader::startDownload(QString urlstring)
{
    if (urlstring.isEmpty())
        return;

    QString localurl = getLocalPath(urlstring);
    QUrl url(urlstring);

    QFile file(localurl);
    if (file.exists())
    {
//        qDebug() << "file load file from cache";
        emit downloadReady(url.toString(), localurl);
    }
    else
    {
        if (mPendingUrl.indexOf(url) != -1)
        {
//            qDebug() << "avoid double download";
            return;
        }
        if (url.scheme().contains("http"))
        {
            FBDownload *dl = new FBDownload(url, localurl, this);

            connect(dl, &FBDownload::downloadReady, this, &FBDownloader::onDownloadReady);
            connect(dl, &FBDownload::downloadFailed, this, &FBDownloader::onDownloadFailed);

            mPendingUrl.append(url);
            dl->startDownload();
        }
        else
        {
//            qDebug() << "is local file";
            emit downloadReady(url.toString(), localurl);
        }
    }
}

void FBDownloader::onDownloadReady (FBDownload* download)
{
    qDebug() << download->mUrl.toString() << " ready";

    mPendingUrl.removeOne(download->mUrl.toString());

    emit downloadReady(download->mUrl.toString(), download->mFilepath);
    delete download;
}

void FBDownloader::onDownloadFailed (FBDownload* download)
{
    qDebug() << download->mUrl.toString() << " failed";

    mPendingUrl.removeOne(download->mUrl.toString());

    emit downloadFailed(download->mUrl.toString(), download->mFilepath);
    delete download;
}

void FBDownloader::startDownloadUpdate(QString urlstring, QString localurl)
{
    QFile file(localurl);
    if (file.exists())
    {
        qDebug() << "start download if updated";

        QUrl url(urlstring);
        FBDownload *dl = new FBDownload(url, localurl, this);
        mPendingUrl.append(url);

        QFileInfo finfo(localurl);
        dl->mRequestTimestamp = finfo.lastModified().toUTC();
        dl->startDownload();
    }
    else
    {
        qDebug() << "file to update doesn't exist, should not happen, download anyway :P";
        startDownload(urlstring);
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
