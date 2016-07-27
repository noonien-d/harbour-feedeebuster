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

#include "fbfeedlistmodel.h"

FBFeedListModel::FBFeedListModel()
{

}

QVariant FBFeedListModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

int FBFeedListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mFeedList.count();
}

QVariant FBFeedListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mFeedList.size())
        return QVariant();

    if (role == TimeRole)
        return QDateTime();
    else if (role == TitleRole)
        return mFeedList.at(index.row())->mTitle;
    else if (role == UrlRole)
        return mFeedList.at(index.row())->mSourceUrl;
    else if (role == UrlRole)
        return mFeedList.at(index.row())->mIcon;
    else
        return QVariant();
}

bool FBFeedListModel::removeRows(int row, int count, const QModelIndex & parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), row, row+count-1);

    for (int i=0; i<count; i++)
        mFeedList.removeAt(row);

    endRemoveRows();
    return true;
}

void FBFeedListModel::addFeed(FBFeed *feed)
{
    if (mFeedList.contains(feed))
        return;

    beginInsertRows(QModelIndex(), mFeedList.size(), mFeedList.size());
    mFeedList.append(feed);
    endInsertRows();
}
