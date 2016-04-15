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

import QtQuick 2.0
import Sailfish.Silica 1.0


Page {
    id: page

    function setItemView() {
        pageStack.push(Qt.resolvedUrl("ItemPage.qml"));
    }

    Component.onCompleted: {
        manager.setitemview.connect(setItemView);
    }

    SilicaListView {
        PullDownMenu {
            MenuItem {
                text: qsTr("Configuration")
                onClicked: pageStack.push(Qt.resolvedUrl("ConfigPage.qml"));
            }
            MenuItem {
                text: qsTr("Manage subscriptions")
                onClicked: pageStack.push(Qt.resolvedUrl("FeedPage.qml"));
            }
            MenuItem {
                text: qsTr("Add feed")
                onClicked: pageStack.push(Qt.resolvedUrl("AddFeedPage.qml"));
            }
            MenuItem {
                text: qsTr("Reload all")
                onClicked: manager.reload();
            }
        }

        id: listView
        model: allitemlist
        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Items")
        }
        delegate: BackgroundItem {
            id: delegate
            Image {
                id: feedicon
                x: Theme.paddingSmall
                visible: imageurl ? imagedownloader.isReady(imageurl) && manager.showicons : true
                source: imageurl ? imagedownloader.getLocalPath(imageurl) : mediaurl ? "image://theme/icon-m-speaker" : "image://theme/icon-m-note"
                asynchronous: true;
                fillMode: Image.PreserveAspectFit
                width: manager.showicons ? Theme.iconSizeMedium + Theme.paddingSmall : Theme.paddingSmall
                height: Theme.iconSizeMedium

                Component.onCompleted: {
                    if ((!visible) && (manager.showicons))
                    {
                        if (imagedownloader.isReady(imageurl))
                            visible = true;
                        else
                            imagedownloader.downloadReady.connect(onLoaded);
                    }
                }
                function onLoaded(url, localurl) {
                    console.log("QML:Loaded " + url);
                    if (localurl == source)
                        visible = true;
                }
            }
            Label {
                id: aliaslabel
                text: itemtitle
                anchors.left: feedicon.right
                anchors.bottom: feedicon.verticalCenter
                color: itemisread ? Theme.primaryColor : Theme.highlightColor
                width: parent.width - feedicon.width
            }
            Label {
                x: Theme.paddingLarge

                text: itemtime.toLocaleString(Qt.locale(), Locale.ShortFormat) + "   " + feedtitle
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: feedicon.right
                anchors.top: feedicon.verticalCenter
                color: itemisread ? Theme.primaryColor : Theme.highlightColor
                width: parent.width - feedicon.width
            }
            onClicked: {
                manager.setCurrentItem(index);
                console.log("Clicked " + index)}
        }
        VerticalScrollDecorator {}
    }
}


