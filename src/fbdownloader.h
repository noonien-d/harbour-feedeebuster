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

#ifndef FBDOWNLOADER_H
#define FBDOWNLOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkAccessManager>


class QFile;
class FBDownloader;

class FBDownload : public QObject
{
Q_OBJECT
public:
    explicit FBDownload(QString url, QString localurl, FBDownloader *downloader);
    QString mUrl;
    QString mFilepath;
    QDateTime mRequestTimestamp;

private:
    QFile *mFile;
    QNetworkReply   *mReply;
    FBDownloader    *mDownloader;

signals:
    void downloadReady (QString url, QString localurl);

public Q_SLOTS:
    void onReadyRead();
    void onDownloaded ();

public:
    void startDownload ();
};

class FBDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FBDownloader(QString basepath, bool preservenames = false, QObject *parent = 0);
    virtual ~FBDownloader();

    QNetworkAccessManager mAccessManager;

    void onDownloadReady (FBDownload* download);
    void onDownloadFailed (FBDownload* download);

    bool allDownloadsFinished();

signals:
    void downloadReady (QString url, QString localurl);
    void downloadFailed (QString url, QString localurl);

public Q_SLOTS:
    void startDownload (QString uri);
    void startDownloadUpdate (QString uri, QString localurl);
    bool isReady (QString uri);
    QString getLocalPath(QString url);

private:
    QString mBasepath;
    QList<QString> mPendingUrl;

    bool mPreserveNames;
};

#endif // FBDOWNLOADER_H
