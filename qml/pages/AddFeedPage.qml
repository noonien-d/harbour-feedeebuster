import QtQuick 2.2
import Sailfish.Silica 1.0

Dialog {
    Column {
        width: parent.width

        DialogHeader { }

        TextField {
            id: urlField
            width: parent.width
            placeholderText: "http://domain.tld/feed.xml"
            label: "URL"
            inputMethodHints: Qt.ImhUrlCharactersOnly;
        }
    }

    onDone: {
        if (result == DialogResult.Accepted) {
            console.log(urlField.text)
            manager.addFeed(urlField.text)
        }
    }
}
