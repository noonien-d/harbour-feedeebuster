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
