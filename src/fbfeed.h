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
