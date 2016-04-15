import QtQuick 2.0

import QtQuick 2.0
import Sailfish.Silica 1.0
import "../"
import QtWebKit 3.0

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
    }
}
