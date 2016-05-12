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
import "../"

Page {
    Column {
        width: parent.width

        PageHeader {
            id: pageHeader
            title: qsTr("Configuration")
        }

        TextSwitch {
            id: iconsSwitch
            text: qsTr("Show Icons")
            checked: manager.showicons;
            onCheckedChanged: {
                manager.showicons = checked
            }
        }
        TextSwitch {
            id: notificationSwitch
            text: qsTr("Show Notifications")
            checked: manager.shownotifications;
            onCheckedChanged: {
                manager.shownotifications = checked
            }
        }
        TextSwitch {
            id: serviceSwitch
            text: qsTr("Background refresh")
            checked: manager.backgroundrefresh;
            enabled: manager.backgroundrefreshavailable;
            onCheckedChanged: {
                manager.backgroundrefresh = checked
            }
        }
    }
}
