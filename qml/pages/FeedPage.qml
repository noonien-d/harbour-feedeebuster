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
        delegate: ListItem {
            id: delegate
            Image {
                id: feedicon
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
                anchors.left: feedicon.right
                anchors.bottom: feedicon.verticalCenter
                color: Theme.primaryColor
                width: parent.width - feedicon.width
            }
            Label {
                x: Theme.paddingLarge

                text: feedurl
                font.pixelSize: Theme.fontSizeExtraSmall
                anchors.left: feedicon.right
                anchors.top: feedicon.verticalCenter
                color: Theme.primaryColor
                width: parent.width - feedicon.width
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


