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

#include <QtQuick>

#include <QStandardPaths>
#include <QtDBus/QDBusConnection>

#include <sailfishapp.h>

#include "fbfeed.h"
#include "fbitem.h"
#include "fbitemlistmodel.h"
#include "fbdownloader.h"
#include "fbmanager.h"

#include "feedeebuster_adaptor.h"

#include <QDebug>
#include <QVariant>

int main(int argc, char *argv[])
{
    QGuiApplication *app = SailfishApp::application(argc, argv);

    bool startgui = true;
    bool reload = false;

    if (argc > 1)
    {
        for (int i=0; i<argc; i++)
        {
            if (QString::fromUtf8(argv[i]) == "--reload")
                reload = true;
            if (QString::fromUtf8(argv[i]) == "--nogui")
                startgui = false;
            if (QString::fromUtf8(argv[i]) == "--reloadwifi")
            {
                QNetworkAccessManager nam;

                if (nam.activeConfiguration().bearerType() == QNetworkConfiguration::BearerWLAN)
                    reload = true;
                else
                    qDebug() << "No wifi active!";
            }
        }
    }

    if ((!reload) && (!startgui))
        return 0;

    FBManager* manager = new FBManager((QObject*)app);
    if (reload)
        manager->reload();

    if (startgui)
    {
        QQuickView *view = SailfishApp::createView();
        QQmlContext *root = view->rootContext();
        manager->setQmlContext(root);
        manager->mQuickView = view;

        manager->mAllItemListModel->sort();

        foreach (FBItem* item, manager->mAllItemListModel->mItemList)
            manager->mImageDownloader->startDownload(item->mImage);

        view->setSource(SailfishApp::pathTo("qml/harbour-feedeebuster.qml"));
        view->showFullScreen();

        new FeedeebusterAdaptor(manager);
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.registerObject("/", manager);
        connection.registerService("com.harbour.feedeebuster");

        int ret = app->exec();

        delete app;

        return ret;
    }
    else
    {
        int timeout = 15;
        while ((!manager->mFeedDownloader->allDownloadsFinished()) && (timeout))
        {
            timeout --;

            app->processEvents(QEventLoop::AllEvents, 100);

            QObject().thread()->sleep(1);
        }
        delete manager->mFeedDownloader;
    }
}

