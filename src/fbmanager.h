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
    Q_PROPERTY(bool backgroundrefresh READ getBackgroundRefresh WRITE setBackgroundRefresh)
    Q_PROPERTY(bool backgroundrefreshavailable READ getBackgroundRefreshAvailable)

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
    bool getBackgroundRefresh();
    void setBackgroundRefresh(bool backgroundrefresh);
    bool getBackgroundRefreshAvailable();
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
