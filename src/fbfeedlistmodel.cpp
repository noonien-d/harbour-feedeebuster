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

    beginRemoveRows(QModelIndex(), row, row);
    mFeedList.removeAt(row);
    endRemoveRows();

    return true;
}
