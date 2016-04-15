#ifndef FBMANAGER_H
#define FBMANAGER_H

#include <QObject>
#include <QHash>
#include <QFile>
#include <QMediaPlayer>
#include "fbitemlistmodel.h"
#include "fbfeedlistmodel.h"

class QQmlContext;
class QQuickView;
class FBFeed;
class FBDownloader;
class Notification;
class QFileSystemWatcher;

class FBManager : public QObject
{
    Q_OBJECT
public:
    explicit FBManager(QObject *parent = 0);
    virtual ~FBManager();

    QList<FBItem*> getCombinedList ();
    FBItemListModel *mCurrentItemListModel;
    FBItemListModel *mAllItemListModel;
    FBFeedListModel *mFeedListModel;


    QQuickView *mQuickView;

    QMediaPlayer mAudioPlayer;

    FBDownloader *mImageDownloader;
    FBDownloader *mFeedDownloader;
    FBDownloader *mMediaDownloader;

    Q_PROPERTY(int unreadcount READ getUnreadCount NOTIFY unreadcountChanged)
    Q_PROPERTY(bool showicons READ getShowIcons WRITE setShowIcons NOTIFY showiconsChanged)
    Q_PROPERTY(bool shownotifications READ getShowNotifications WRITE setShowNotifications)

    void setQmlContext (QQmlContext *mQmlRoot);

signals:
    void unreadcountChanged(int count);
    void showiconsChanged(bool status);
    void setitemview();

public slots:
    void setCurrentItem (int index);
    void setCurrentMedia (QString url);
    void onFeedDownloaded(QString url, QString localurl);
    int  getUnreadCount();
    bool getShowIcons();
    void setShowIcons(bool showicons);
    bool getShowNotifications();
    void setShowNotifications(bool shownotfications);
    void addFeed (QString url);
    void removeFeed(QString url);
    void reload();
    void show(uint uuid);

private:
    QString mFeedDirectory;
    QHash<FBItem*, Notification*> mNotifications;

    QFileSystemWatcher *mWatcher;
    QFile mConfigFeedsFile;
    QFile mConfigFile;

    bool mConfigShowIcons;
    bool mConfigShowNotifications;
    bool mConfigChanged;

    QQmlContext *mQmlRoot;
private slots:
    void onFileChanged(QString path);
};

#endif // FBMANAGER_H
