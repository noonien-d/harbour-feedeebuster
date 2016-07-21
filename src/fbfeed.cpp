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

#include "fbfeed.h"

#include <QXmlStreamReader>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QTextDocument>
#include <QCryptographicHash>

#include "fbitem.h"

FBFeed::FBFeed(QString source, QObject *parent) : QObject(parent)
{
    mSourceFile = source;

    parseFile(mSourceFile);
}

bool FBFeed::parseFile(QString filepath)
{
    qWarning() << filepath;

    QXmlStreamReader xml;

    QFile xmlFile(filepath);
    if (!xmlFile.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file " << filepath << " : " << xmlFile.errorString();
        return false;
    }
    xml.setDevice(&xmlFile);

    QString imageurl;
    QString feedtitle;
    QString selfurl;

    while (!xml.atEnd())
    {
        if (xml.readNext())
        {
            //qDebug() << "XML: " << xml.name() << " : " << xml.attributes().value("href").toString();
            if (xml.name() == "title")
                feedtitle = xml.readElementText();
            else if (xml.name() == "item")
                processItem(xml);
            else if (xml.name() == "entry")
                processEntry(xml);
            else if (xml.name() == "image")
            {
                if (xml.attributes().hasAttribute("href"))
                    imageurl = xml.attributes().value("href").toString();
                else if (xml.isStartElement())
                    imageurl = processImage(xml);
            }
            else if ((xml.name() == "link") && xml.attributes().hasAttribute("rel") && (xml.attributes().value("rel").toString() == "self"))
                selfurl = xml.attributes().value("href").toString();
        }
    }
    if (xml.hasError())
        qWarning() << "Feed has error";

    if (!imageurl.isEmpty())
    {
        mIcon = imageurl;

        foreach (FBItem* item, mItemList)
        {
            if (item->mImage.isEmpty())
                item->mImage = mIcon;
        }
    }
    if (!feedtitle.isEmpty())
    {
        mTitle = feedtitle;
        qDebug() << "Got feed title: " << mTitle;
    }
    if (!selfurl.isEmpty())
    {
        mSourceUrl = selfurl;
        qDebug() << "Got feed url: " << mSourceUrl;
    }
    xmlFile.close();

    return true;
}

void FBFeed::processItem(QXmlStreamReader& xml) {

    if (!xml.isStartElement() || xml.name() != "item")
        return;

    QString title;
    QString link;
    QString timetext;
    QString description;
    QString summary;
    QString body;
    QString content;
    QString mediaurl;
    QString mediaduration;
    uint    mediasize = 0;
    QString mediatype;
    QString imageurl;
    QString uuid;

    while (!xml.atEnd())
    {
        QXmlStreamReader::TokenType t = xml.readNext();
        if (t >= QXmlStreamReader::StartElement)
        {
            //qDebug() << xml.name();

            if (xml.name() == "title")
                title = xml.readElementText();
            else if (xml.name() == "link")
                link = xml.readElementText();
            else if (xml.name() == "pubDate")
                timetext = xml.readElementText();
            else if (xml.name() == "description")
                description = xml.readElementText();
            else if (xml.name() == "summary")
                summary = xml.readElementText();
            else if ((xml.name() == "enclosure") && xml.attributes().hasAttribute("url"))
            {
                mediaurl = xml.attributes().value("url").toString();

                if (xml.attributes().hasAttribute("length"))
                    mediasize = xml.attributes().value("length").toUInt();

                if (xml.attributes().hasAttribute("type"))
                    mediatype = xml.attributes().value("type").toString();
            }
            else if ((xml.name() == "image") && xml.attributes().hasAttribute("href"))
                imageurl = xml.attributes().value("href").toString();
            else if (xml.name() == "duration")
                mediaduration = xml.readElementText();
            else if (xml.name() == "guid")
                uuid = xml.readElementText();
            else if (xml.name() == "encoded")
                content = xml.readElementText();
            else if (xml.name() == "body")
                body = xml.readElementText();
        }

        if ((t == QXmlStreamReader::EndElement) && (xml.name() == "item"))
            break;
    }
    //fix for some feeds with incorrect month abbreviations
    timetext.replace("July", "Jul");
    QDateTime time = QDateTime::fromString(timetext, Qt::RFC2822Date);

    QTextDocument htmldescription;
    htmldescription.setHtml(description);

    FBItem *item = new FBItem(time, title, htmldescription.toPlainText(), link, this);
//    FBItem *item = new FBItem(time, title, encodeEntities(description), link, this);
//    FBItem *item = new FBItem(time, title, htmldescription.toHtml(), link, this);

    item->mContent = content;

    if (item->mDescription == "")
        item->mDescription = summary;

    if (item->mDescription == "")
    {
        item->mDescriptionHtml = true;
        item->mDescription = content;
    }

    if (item->mContent == "")
        item->mContent = body;

    if (imageurl.isEmpty())
        item->mImage = mIcon;
    else
        item->mImage = imageurl;

    if (mediatype.contains("audio"))
    {
        item->mMediaUrl = mediaurl;
        item->mMediaSize = mediasize;
        item->mMediaType = mediatype;

        if (!mediaduration.isEmpty())
            item->mMediaDuration = QTime::fromString(mediaduration, "hh:mm:ss").msecsSinceStartOfDay()/1000;
    }

    //backup uuid generation
    if (uuid.isEmpty())
        uuid = link;
    if (uuid.isEmpty())
        uuid = title + description;

    //generate numerical uuid
    QByteArray uuidhash = QCryptographicHash::hash(uuid.toUtf8(), QCryptographicHash::Md5);
    item->mUuid = 0;
    for (int i=0; i<uuidhash.count(); i++)
        item->mUuid += uuidhash.at(i);

    foreach (FBItem *olditem, mItemList)
    {
        //Break if item already exists
        if (olditem->mUuid == item->mUuid)
        {
            delete item;
            return;   
        }
    }

    mItemList.append(item);

    emit newItem(item);
}

void FBFeed::processEntry(QXmlStreamReader& xml) {

    if (!xml.isStartElement() || xml.name() != "entry")
        return;

    QString title;
    QString link;
    QString timetext;
    QString summary;
    bool    summaryhtml = false;
    QString content;
    QString mediaurl;
    QString mediaduration;
    uint    mediasize = 0;
    QString mediatype;
    QString imageurl;
    QString uuid;

    while (!xml.atEnd())
    {
        QXmlStreamReader::TokenType t = xml.readNext();
        if (t >= QXmlStreamReader::StartElement)
        {
            qDebug() << xml.name();

            if (xml.name() == "title")
                title = xml.readElementText();
            else if (xml.name() == "updated")
                timetext = xml.readElementText();
            else if (xml.name() == "summary")
            {
                summary = xml.readElementText();

                if (xml.attributes().hasAttribute("type"))
                    summaryhtml = (xml.attributes().value("type") == "html");
            }
            else if (xml.name() == "duration")
                mediaduration = xml.readElementText();
            else if (xml.name() == "id")
                uuid = xml.readElementText();
            else if (xml.name() == "content:encoded")
                content = xml.readElementText();
            else if (xml.name() == "icon")
                imageurl = xml.readElementText();            
            else if ((xml.name() == "link") && xml.attributes().hasAttribute("rel"))
            {
                if (xml.attributes().value("rel") == "enclosure")
                {
                    mediaurl = xml.attributes().value("href").toString();

                    if (xml.attributes().hasAttribute("length"))
                        mediasize = xml.attributes().value("length").toUInt();

                    if (xml.attributes().hasAttribute("type"))
                        mediatype = xml.attributes().value("type").toString();
                }
                if (xml.attributes().value("rel") == "alternate")
                {
                    link = xml.attributes().value("href").toString();
                }
            }
        }

        if ((t == QXmlStreamReader::EndElement) && (xml.name() == "entry"))
            break;
    }
    QDateTime time = QDateTime::fromString(timetext, Qt::ISODate);

    FBItem *item = new FBItem(time, title, summary, link, this);

    item->mContent = content;

    //summaryhtml = true;
    item->mDescriptionHtml = summaryhtml;

    qDebug() << summaryhtml;

    if (imageurl.isEmpty())
        item->mImage = mIcon;
    else
        item->mImage = imageurl;

    if (mediatype.contains("audio"))
    {
        item->mMediaUrl = mediaurl;
        item->mMediaSize = mediasize;
        item->mMediaType = mediatype;

        if (!mediaduration.isEmpty())
            item->mMediaDuration = QTime::fromString(mediaduration, "hh:mm:ss").msecsSinceStartOfDay()/1000;
    }

    //backup uuid generation
    if (uuid.isEmpty())
        uuid = link;
    if (uuid.isEmpty())
        uuid = title + summary;

    //generate numerical uuid
    QByteArray uuidhash = QCryptographicHash::hash(uuid.toUtf8(), QCryptographicHash::Md5);
    item->mUuid = 0;
    for (int i=0; i<uuidhash.count(); i++)
        item->mUuid += uuidhash.at(i);

    foreach (FBItem *olditem, mItemList)
    {
        //Break if item already exists
        if (olditem->mUuid == item->mUuid)
        {
            delete item;
            return;
        }
    }

    mItemList.append(item);

    emit newItem(item);
}

QString FBFeed::processImage(QXmlStreamReader& xml)
{
    while (!xml.atEnd())
    {
        QXmlStreamReader::TokenType t = xml.readNext();
        if (t >= QXmlStreamReader::StartElement)
        {
            qDebug() << xml.name();

            if (xml.name() == "url")
                return xml.readElementText();
        }
        if ((t == QXmlStreamReader::EndElement) && (xml.name() == "image"))
            break;
    }
    return "";
}
