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

#ifndef FBITEMLISTMODEL_H
#define FBITEMLISTMODEL_H

#include <QDateTime>
#include <QAbstractListModel>

#include "fbitem.h"

class FBItem;

class FBItemListModel : public QAbstractListModel
{
public:
    FBItemListModel();

    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;

    void sort ();
    void setItemList (QList<FBItem*> newlist);

    enum
    {
        TimeRole,
        TitleRole,
        FeedTitleRole,
        BodyRole,
        LinkRole,
        ImageurlRole,
        MediaurlRole,
        IsReadRole
    };

    virtual QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[TimeRole]         = "itemtime";
        roles[TitleRole]        = "itemtitle";
        roles[FeedTitleRole]    = "feedtitle";
        roles[BodyRole]         = "itembody";
        roles[LinkRole]         = "itemlink";
        roles[ImageurlRole]     = "imageurl";
        roles[MediaurlRole]     = "mediaurl";
        roles[IsReadRole]       = "itemisread";
        return roles;
    }

    QList<FBItem *> mItemList;

public Q_SLOTS:
    void onImageLoaded (QString url, QString localurl);
    void onNewItem (FBItem *item);
    void onItemChanged(FBItem *item);
};

#endif // FBITEMLISTMODEL_H
