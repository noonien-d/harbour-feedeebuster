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

import QtQuick 2.1
import Sailfish.Silica 1.0

MouseArea {
    id: popup
    anchors.top: parent.top
    width: parent.width
    height: message.paintedHeight + (Theme.paddingLarge * 2)
    property alias title: message.text
    property alias timeout: hideTimer.interval
    property alias background: bg.color
    visible: opacity > 0
    opacity: 0.0

    Behavior on opacity {
        FadeAnimation {}
    }

    Rectangle {
        id: bg
        anchors.fill: parent
    }

    Timer {
        id: hideTimer
        triggeredOnStart: false
        repeat: false
        interval: 5000
        onTriggered: popup.hide()
    }

    function hide() {
        if (hideTimer.running)
            hideTimer.stop()
        popup.opacity = 0.0
    }

    function show() {
        popup.opacity = 1.0
        hideTimer.restart()
    }

    function notify(text, color) {
        popup.title = text
        if (color && (typeof(color) != "undefined"))
            bg.color = color
        else
            bg.color = Theme.rgba(Theme.secondaryHighlightColor, 0.9)
        show()
    }

    Label {
        id: message
        anchors.verticalCenter: popup.verticalCenter
        font.pixelSize: 32
        anchors.left: parent.left
        anchors.leftMargin: Theme.paddingLarge
        anchors.right: parent.right
        anchors.rightMargin: Theme.paddingLarge
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
        wrapMode: Text.Wrap
    }

    onClicked: hide()
}
