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

                return;
            }
        }

        qDebug() << "No relocation in header";
        mDownloader->onDownloadFailed(this);
    }
    else if (mReply->error() == 0)
    {
        if (mFile == NULL)
        {
            mFile = new QFile(mFilepath + ".part");
            if (!mFile->open(QFile::WriteOnly))
            {
                qDebug() << "Error " << mFile->error() << ": " << mFile->errorString();
                mDownloader->onDownloadFailed(this);
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
            mDownloader->onDownloadFailed(this);
        }
        else
        {
            emit downloadReady(mUrl, mFilepath);
            mDownloader->onDownloadReady(this);
        }
    }
    else
    {
        qDebug() << "Error " << mReply->error() << ": " << mReply->errorString();
        mDownloader->onDownloadFailed(this);
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
    qDebug() << download->mUrl << " ready";

    mPendingUrl.removeOne(download->mUrl);

    emit downloadReady(download->mUrl, download->mFilepath);
}

void FBDownloader::onDownloadFailed (FBDownload* download)
{
    qDebug() << download->mUrl << " failed";

    mPendingUrl.removeOne(download->mUrl);

    emit downloadFailed(download->mUrl, download->mFilepath);
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
