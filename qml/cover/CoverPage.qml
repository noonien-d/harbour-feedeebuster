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


