#include "fbmanager.h"

#include <QDebug>
#include <QQmlContext>
#include <QQuickView>
#include <QDir>
#include <QStandardPaths>
#include <QFileSystemWatcher>
#include <notification.h>

#include "fbdownloader.h"
#include "fbitem.h"

FBManager::FBManager(QObject *parent) : QObject(parent)
{
    mFeedDirectory = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    mFeedDownloader = new FBDownloader(mFeedDirectory);
    connect (mFeedDownloader, &FBDownloader::downloadReady, this, &FBManager::onFeedDownloaded);

    mImageDownloader = new FBDownloader(QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation));

    mMediaDownloader = new FBDownloader(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation), true);


    mWatcher = new QFileSystemWatcher(this);
    connect(mWatcher, &QFileSystemWatcher::fileChanged, this, &FBManager::onFileChanged);

    mAllItemListModel = new FBItemListModel();
    mCurrentItemListModel = mAllItemListModel;
    mFeedListModel = new FBFeedListModel();

    connect (mImageDownloader, &FBDownloader::downloadReady, mAllItemListModel, &FBItemListModel::onImageLoaded);

    // read feed list from file
    QDir dir;
    dir.setPath(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/harbour-feedeebuster/");

    if (!dir.exists())
        dir.mkpath(".");

    mConfigFeedsFile.setFileName(dir.absolutePath()+"/feeds.cnf");

    qDebug() << mConfigFeedsFile.fileName();

    if (mConfigFeedsFile.open(QFile::ReadOnly))
    {
        while (!mConfigFeedsFile.atEnd())
        {
            QString feedurl = QString::fromUtf8(mConfigFeedsFile.readLine()).trimmed();
            QString feedfile = mFeedDownloader->getLocalPath(feedurl);

            if ((feedurl.at(0) == '#') || (feedurl.length() < 3))
                continue;

            mWatcher->addPath(feedfile);

            //create and load feed
            FBFeed *feed = new FBFeed (feedfile);
            feed->mSourceUrl = feedurl;
            mFeedListModel->mFeedList.append(feed);
            connect (feed, &FBFeed::newItem, mAllItemListModel, &FBItemListModel::onNewItem);
        }
        mConfigFeedsFile.close();
    }

    mAllItemListModel->mItemList = getCombinedList();

    // check current notifications
    QList<QObject*> nlist = Notification::notifications("harbour-feedeebuster");
    foreach (QObject* o, nlist)
    {
        Notification* n = (Notification*)o;

        foreach (FBFeed *feed, mFeedListModel->mFeedList)
        {
            foreach (FBItem *item, feed->mItemList)
            {
                if (item->mTitle == n->summary())
                {
                    mNotifications.insert(item, n);
                    item->mIsRead = false;
                }
            }
        }
    }

    // read configuration file
    mConfigShowIcons = true;
    mConfigShowNotifications = true;
    mConfigFile.setFileName(dir.absolutePath()+"/config.cnf");
    if (mConfigFile.open(QFile::ReadOnly))
    {
        while(!mConfigFile.atEnd())
        {
            QString line = QString::fromUtf8(mConfigFile.readLine());

            qDebug() << line;

            if (line.contains("showicons"))
                mConfigShowIcons = line.contains("true");
            if (line.contains("shownotifications"))
                mConfigShowNotifications = line.contains("true");
        }
        mConfigFile.close();
    }
}

FBManager::~FBManager()
{
    if (mConfigChanged)
    {
        qDebug("Write configuration file");
        QString str;

        if (mConfigShowIcons)
            str += "showicons = true\n";
        else
            str += "showicons = false\n";

        if (mConfigShowNotifications)
            str += "shownotifications = true\n";
        else
            str += "shownotifications = false\n";

        mConfigFile.open(QFile::WriteOnly);
        mConfigFile.write(str.toUtf8());
        mConfigFile.close();
    }

}

void FBManager::setCurrentItem (int index)
{
    qDebug() << index;
    mQmlRoot->setContextProperty("currentitem", mCurrentItemListModel->mItemList.at(index));

    if (!mCurrentItemListModel->mItemList.at(index)->mIsRead)
    {
        FBItem *item = mCurrentItemListModel->mItemList.at(index);

        item->mIsRead = true;
        mCurrentItemListModel->onItemChanged(item);
        emit unreadcountChanged(getUnreadCount());

        if (mNotifications.contains(item))
        {
            mNotifications[item]->close();
            mNotifications.remove(item);
        }
    }
    emit setitemview();
}

void FBManager::setCurrentMedia(QString media)
{
    qDebug() << mAudioPlayer.currentMedia().canonicalUrl().toString();

    if (mAudioPlayer.currentMedia().canonicalUrl().toString().contains(media))
        return;

    if (media.startsWith("/"))
        media.prepend("file://");

    mAudioPlayer.setMedia(QMediaContent(QUrl(media)));

    mQmlRoot->setContextProperty("currentplayitem", mQmlRoot->contextProperty("currentitem"));
}

QList<FBItem*> FBManager::getCombinedList ()
{
    QList<FBItem*> list;

    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        foreach (FBItem *item, feed->mItemList)
            list.append(item);
    }

    qSort (list.begin(), list.end(), FBItem::TimeCompare);

    if (list.count() > 50)
        return list.mid(0, 50);
    else
        return list;
}

void FBManager::onFeedDownloaded(QString url, QString localurl)
{
    qDebug () << "downloaded feed " << localurl;

    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        if (feed->mSourceFile == localurl)
        {
            qDebug () << "parse feed " << feed->mTitle;

            feed->parseFile(feed->mSourceFile);

            foreach (FBItem *item, feed->mItemList)
            {
                if ((!item->mIsRead) && (mConfigShowNotifications))
                {
                    //check current notifications, there may be a notification by background reload
                    QList<QObject*> nlist = Notification::notifications("harbour-feedeebuster");
                    foreach (QObject* o, nlist)
                    {
                        Notification* n = (Notification*)o;
                        if (item->mTitle == n->summary())
                            mNotifications.insert(item, n);
                    }

                    if (!mNotifications.contains(item))
                    {
                        if (!mImageDownloader->isReady(item->mImage))
                            mImageDownloader->startDownload(item->mImage);

                        Notification *n = new Notification();
                        n->setAppName("harbour-feedeebuster");
                        n->setSummary(item->mTitle);
                        n->setBody(item->mDescription);
                        n->setTimestamp(item->mTime);

                        QVariant na1 = Notification::remoteAction("default", "start feedeebuster", "com.harbour.feedeebuster", "/", "com.harbour.feedeebuster", "show", QVariantList() << QVariant(item->mUuid));
                        QVariant na2 = Notification::remoteAction("app", "start feedeebuster", "com.harbour.feedeebuster", "/", "com.harbour.feedeebuster", "start");

                        n->setRemoteActions(QVariantList() << na1 << na2);

                        n->publish();

                        mNotifications.insert(item, n);
                    }
                }
            }
            //Found related feed, nothing else to do
            emit unreadcountChanged(getUnreadCount());
            return;
        }
    }

    //Its a new feed so add it to the config list...
    mConfigFeedsFile.open(QFile::WriteOnly | QFile::Append);
    mConfigFeedsFile.write(url.toUtf8());
    mConfigFeedsFile.write(QString("\n").toUtf8());
    mConfigFeedsFile.close();

    //... load the feed content...
    FBFeed *feed = new FBFeed (localurl);
    feed->mSourceUrl = url;
    qDebug () << "add new feed: " << feed->mTitle;

    //... add the feed to the feed list
    mFeedListModel->mFeedList.append(feed);

    //... and add its content to the item view
    mAllItemListModel->connect (feed, &FBFeed::newItem, mAllItemListModel, &FBItemListModel::onNewItem);
    foreach (FBItem *item, feed->mItemList)
    {
        if (!mImageDownloader->isReady(item->mImage))
            mImageDownloader->startDownload(item->mImage);

        mAllItemListModel->mItemList.append(item);
    }
    mAllItemListModel->sort();
}

void FBManager::onFileChanged(QString path)
{
    qDebug () << "feed changed " << path;

    // reload all files because there is no garanty that every filechange emits an event
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        onFeedDownloaded(feed->mSourceUrl, feed->mSourceFile);
    }
}

int FBManager::getUnreadCount()
{
    int count = 0;
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        foreach (FBItem *item, feed->mItemList)
        {
            if (!item->mIsRead)
                count ++;
        }
    }
    return count;
}

void FBManager::addFeed(QString url)
{
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        if (feed->mSourceUrl == url)
        {
            qWarning() << "Feed url is already known as " << feed->mTitle;
            return;
        }
    }

    mFeedDownloader->startDownload(url);
}

void FBManager::removeFeed(QString url)
{
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        if (feed->mSourceUrl == url)
        {
            qDebug () << "remove feed " << feed->mTitle;
            mFeedListModel->removeRow(mFeedListModel->mFeedList.indexOf(feed));
            break;
        }
    }

    mConfigFeedsFile.open(QFile::WriteOnly);
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        mConfigFeedsFile.write(feed->mSourceUrl.toUtf8());
        mConfigFeedsFile.write(QString("\n").toUtf8());
    }
    mConfigFeedsFile.close();

    //reset list as it may contain items of the removed feed
    mAllItemListModel->setItemList(getCombinedList());
}

void FBManager::setShowIcons(bool showicons)
{
    mConfigShowIcons = showicons;
    mConfigChanged = true;
    emit showiconsChanged(mConfigShowIcons);
}

bool FBManager::getShowIcons()
{
    return mConfigShowIcons;
}

void FBManager::setShowNotifications(bool shownotifications)
{
    mConfigShowNotifications = shownotifications;
    mConfigChanged = true;
}

bool FBManager::getShowNotifications()
{
    return mConfigShowNotifications;
}

void FBManager::reload()
{
    foreach (FBFeed *feed, mFeedListModel->mFeedList)
    {
        mFeedDownloader->startDownloadUpdate(feed->mSourceUrl, feed->mSourceFile);
    }
}

void FBManager::show(uint uuid)
{
    qDebug() << "Invoked " << uuid;

    foreach (FBItem *item, mCurrentItemListModel->mItemList)
    {
        if (item->mUuid == uuid)
            setCurrentItem(mCurrentItemListModel->mItemList.indexOf(item));
    }

    mQuickView->raise();
}

void FBManager::setQmlContext(QQmlContext *qmlroot)
{
    mQmlRoot = qmlroot;

    mQmlRoot->setContextProperty("mediadownloader", mMediaDownloader);
    mQmlRoot->setContextProperty("imagedownloader", mImageDownloader);

    mQmlRoot->setContextProperty("allitemlist", mAllItemListModel);
    mQmlRoot->setContextProperty("allfeedlist", mFeedListModel);

    mQmlRoot->setContextProperty("audioplayer", &mAudioPlayer);

    mQmlRoot->setContextProperty("manager", this);
}
