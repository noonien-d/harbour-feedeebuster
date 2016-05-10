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

#ifndef FBITEM
#define FBITEM

#include <QObject>
#include <QDateTime>
#include "fbfeed.h"

class FBItem : public QObject
{
Q_OBJECT
public:
    FBItem (QDateTime time, QString title, QString description, QString link, FBFeed* feed) : QObject()
    {
        mTime = time;
        mTitle = title;
        mDescription = description;
        mDescriptionHtml = false;
        mLink = link;
        mIsRead = true;
        mFeed = feed;

        mMediaDuration = 0;
        mMediaSize = 0;
    }

    QDateTime mTime;
    QString mTitle;
    QString mDescription;
    bool    mDescriptionHtml;
    QString mContent;
    QString mLink;
    QString mImage;
    uint    mUuid;

    bool    mIsRead;

    QString mMediaUrl;
    uint    mMediaDuration;
    uint    mMediaSize;
    QString mMediaType;

    FBFeed *mFeed;

    static bool TimeCompare(const FBItem* i, const FBItem* j)
    {
        return i->mTime > j->mTime;
    }

public Q_SLOTS:
    QString title() {return mTitle;}
    QString feedtitle() {if(mFeed) return mFeed->mTitle; else return "";}
    QString description() {return mDescription;}
    bool    descriptionhtml() {return mDescriptionHtml;}
    QString content() {return mContent;}
    QString imageurl() {return mImage;}
    QString mediaurl() {return mMediaUrl;}
    uint    mediaduration() {return mMediaDuration;}
    QString mediasize() {if (mMediaSize) return QString("(%1 MiB)").arg(mMediaSize/1024/1024);else return "";}
    QString mediatype() {return mMediaType;}
    QString link() {return mLink;}
};

#endif // FBITEM

