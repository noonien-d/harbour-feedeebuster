#ifndef FBFEED_H
#define FBFEED_H

#include <QObject>
#include <QDateTime>

class QXmlStreamReader;
class FBItem;

class FBFeed : public QObject
{
    Q_OBJECT
public:
    explicit FBFeed(QString source, QObject *parent = 0);

    QList<FBItem *> mItemList;

    QString mSourceUrl;
    QString mSourceFile;

    QString mTitle;
    QString mIcon;

    bool parseFile(QString filepath);

signals:
    void newItem (FBItem* item);

private:
    void processItem(QXmlStreamReader& xml);
    void processEntry(QXmlStreamReader& xml);
    QString processImage(QXmlStreamReader& xml);
};

#endif // FBFEED_H
