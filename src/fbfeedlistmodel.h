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
