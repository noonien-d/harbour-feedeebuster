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
                anchors.verticalCenter: parent.verticalCenter
                visible: true
                source: imageurl && imagedownloader.isReady(imageurl) ? imagedownloader.getLocalPath(imageurl) : mediaurl ? "image://theme/icon-m-speaker" : "image://theme/icon-m-note"
                asynchronous: true;
                fillMode: Image.PreserveAspectFit
                width: manager.showicons ? Theme.iconSizeMedium + Theme.paddingSmall : 0
                height: Theme.iconSizeMedium
                sourceSize {height: height;width: width}

                Component.onCompleted: {
                    if (!imagedownloader.isReady(imageurl))
                        imagedownloader.downloadReady.connect(onLoaded);
                }
                function onLoaded(url, localurl) {
                    if (url === imageurl)
                    {
                        visible = true;
                        source = localurl;
                    }
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


