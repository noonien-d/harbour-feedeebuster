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

CoverBackground {
    function formatSeconds(fullseconds)
    {
        var hours = Math.floor(fullseconds / 3600);
        var minutes = Math.floor((fullseconds % 3600) / 60);
        var seconds = Math.floor(fullseconds % 60);

        if (hours)
            return (hours < 10 ? "0" + hours : hours) + ":" + (minutes < 10 ? "0" + minutes : minutes) + ":" + (seconds  < 10 ? "0" + seconds : seconds);
        else
            return (minutes) + ":" + (seconds  < 10 ? "0" + seconds : seconds);
    }

    property bool isplayitem: typeof currentplayitem !== 'undefined'

    Label {
        id: label
        visible: !isplayitem

        text: "Feedeebuster"

        anchors.centerIn: parent;
    }
    Label {
        id: labelUnread
        visible: (manager.unreadcount > 0)

        text: manager.unreadcount + " " +qsTr("unread")
        color: Theme.highlightColor
        anchors.top: label.bottom;
        anchors.horizontalCenter: parent.horizontalCenter
    }
    Image {
        visible: !isplayitem
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        source: Qt.resolvedUrl("overlay.png")
        opacity: 0.1
    }

    Image {
        id: backgroundImageFeed
        anchors.fill: parent
        fillMode: Image.PreserveAspectCrop
        smooth: true
        visible: isplayitem ? imagedownloader.isReady(currentplayitem.imageurl()) : false
        source: isplayitem ? imagedownloader.getLocalPath(currentplayitem.imageurl()) : ""

        Rectangle {
            anchors.fill: parent
            gradient: Gradient {
                GradientStop { position: 0; color: "#80000000" }
                GradientStop { position: 0.6; color: "#80000000" }
                GradientStop { position: 1; color: "transparent" }
            }
        }
    }

    Item
    {
        anchors.fill: parent;

        anchors.margins: Theme.paddingSmall
        visible: isplayitem

        Label {
            id: labelTitle
            width: parent.width;
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            text: isplayitem ? currentplayitem.title() : ""
        }

        Label {
            id: labelTime
            visible: (audioplayer.state != 0)
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: labelTitle.bottom;
            anchors.topMargin: Theme.paddingLarge
            text: isplayitem ? formatSeconds(audioplayer.position / 1000) + " / " + formatSeconds(audioplayer.duration/1000) : ""
        }
    }
    CoverActionList {
        id: coverActionPlay
        enabled: (isplayitem) && (audioplayer.state != 1)

        CoverAction {
            iconSource: "image://theme/icon-cover-play"

            onTriggered: {
                audioplayer.play();
            }
        }
    }
    CoverActionList {
        id: coverActionPause
        enabled: (audioplayer.state == 1)

        CoverAction {
            iconSource: "image://theme/icon-cover-pause"

            onTriggered: {
                audioplayer.pause();
            }
        }
    }
}


