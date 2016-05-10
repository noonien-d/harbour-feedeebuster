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

#include "fbitemlistmodel.h"

#include <QDebug>

#include "fbitem.h"

FBItemListModel::FBItemListModel()
{

}

QVariant FBItemListModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}

int FBItemListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return mItemList.count();
}

QVariant FBItemListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mItemList.size())
        return QVariant();

    if (role == TimeRole)
        return mItemList.at(index.row())->mTime;
    else if (role == TitleRole)
        return mItemList.at(index.row())->mTitle;
    else if (role == FeedTitleRole)
        return mItemList.at(index.row())->feedtitle();
    else if (role == BodyRole)
        return mItemList.at(index.row())->mDescription;
    else if (role == LinkRole)
        return mItemList.at(index.row())->mLink;
    else if (role == ImageurlRole)
        return mItemList.at(index.row())->mImage;
    else if (role == MediaurlRole)
        return mItemList.at(index.row())->mMediaUrl;
    else if (role == IsReadRole)
        return mItemList.at(index.row())->mIsRead;
    else
        return QVariant();
}

void FBItemListModel::onImageLoaded (QString url, QString localurl)
{
    Q_UNUSED(localurl);
    foreach (FBItem* m, mItemList)
    {
        if (m->mImage == url)
            emit dataChanged (index(mItemList.indexOf(m)), index(mItemList.indexOf(m)));
    }
}

void FBItemListModel::onNewItem(FBItem *item)
{
    qDebug();

    if (mItemList.contains(item))
        return;

    //for new items without timestamp
    if (!item->mTime.isValid())
        item->mTime = QDateTime::currentDateTime();

    item->mIsRead = false;

    beginInsertRows(QModelIndex(), mItemList.size(), mItemList.size());
    mItemList.append(item);
    endInsertRows();

    sort();
}

void FBItemListModel::onItemChanged(FBItem *item)
{
    emit dataChanged (index(mItemList.indexOf(item)), index(mItemList.indexOf(item)));
}

void FBItemListModel::sort()
{
    qSort (mItemList.begin(), mItemList.end(), FBItem::TimeCompare);

    emit dataChanged (index(0), index(mItemList.count()-1));
}

void FBItemListModel::setItemList (QList<FBItem*> newlist)
{
    if (newlist.count() < mItemList.count())
    {
        beginRemoveRows(QModelIndex(), newlist.count(), mItemList.count()-1);
        mItemList = newlist;
        sort();
        endRemoveRows();
    }
    else
    {
        mItemList = newlist;
        sort();
    }
}
