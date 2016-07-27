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

#ifndef FBFEEDLISTMODEL_H
#define FBFEEDLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include "fbfeed.h"

class FBFeedListModel : public QAbstractListModel
{
public:
    FBFeedListModel();

    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    virtual bool removeRows(int row, int count,
                            const QModelIndex & parent = QModelIndex());

    void addFeed(FBFeed *feed);

    enum
    {
        TitleRole,
        UrlRole,
        TimeRole,
        IconRole
    };

    virtual QHash<int, QByteArray> roleNames() const {
        QHash<int, QByteArray> roles;
        roles[TitleRole] = "feedtitle";
        roles[UrlRole] = "feedurl";
        roles[TimeRole] = "feedtime";
        roles[IconRole] = "feedicon";
        return roles;
    }

    QList<FBFeed*> mFeedList;
};

#endif // FBFEEDLISTMODEL_H
