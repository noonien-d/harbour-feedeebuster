/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

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


    FBManager* manager = new FBManager(app);
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

