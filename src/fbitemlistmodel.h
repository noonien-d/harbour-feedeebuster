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
