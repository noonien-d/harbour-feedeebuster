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

import QtQuick 2.2
import Sailfish.Silica 1.0

Page {
    id: page

    SilicaListView {

        id: listView
        model: allfeedlist
        anchors.fill: parent
        header: PageHeader {
            title: qsTr("Subscriptions")
        }
        PullDownMenu {
            MenuItem {
                text: qsTr("Add feed")
                onClicked: pageStack.push(Qt.resolvedUrl("AddFeedPage.qml"));
            }
        }
        delegate: ListItem {
            id: delegate
            Image {
                id: feediconimage
                x: Theme.paddingSmall
                visible: true
                source: feedicon ? imagedownloader.getLocalPath(feedicon) : "image://theme/icon-m-note"
                asynchronous: true;
                fillMode: Image.PreserveAspectFit
                width: imagedownloader.isReady(feedicon) ? Theme.iconSizeMedium + Theme.paddingSmall : Theme.paddingSmall
                height: Theme.iconSizeMedium
            }
            Label {
                id: aliaslabel
                text: feedtitle
                anchors {left: feediconimage.right;bottom: feediconimage.verticalCenter}
                color: Theme.primaryColor
                width: parent.width - feediconimage.width
            }
            Label {
                x: Theme.paddingLarge

                text: feedurl
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors {left: feediconimage.right;top: feediconimage.verticalCenter}
                color: Theme.primaryColor
                width: parent.width - feediconimage.width
            }
            onClicked: {
                console.log("Clicked " + index)
            }

            menu: ContextMenu {
                MenuItem {
                    text: qsTr("Remove")
                    onClicked: remorse.execute("Clearing", function() { manager.removeFeed(feedurl) } )
                }
                MenuItem {
                    text: qsTr("Copy Url")
                    onClicked: {copyHelper.text = feedurl; copyHelper.selectAll(); copyHelper.copy()}
                }
            }
        }
        RemorsePopup { id: remorse }
        VerticalScrollDecorator {}
    }
    TextEdit
    {
        id: copyHelper;
        visible: false;
    }
}


