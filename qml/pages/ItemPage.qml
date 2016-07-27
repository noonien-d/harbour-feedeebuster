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
import QtWebKit 3.0

Page {
    id: page

    function formatSeconds(fullseconds)
    {
        if (fullseconds < 1)
            return "";

        var hours = Math.floor(fullseconds / 3600);
        var minutes = Math.floor((fullseconds % 3600) / 60);
        var seconds = Math.floor(fullseconds % 60);

        if (hours)
            return (hours < 10 ? "0" + hours : hours) + ":" + (minutes < 10 ? "0" + minutes : minutes) + ":" + (seconds  < 10 ? "0" + seconds : seconds);
        else
            return (minutes) + ":" + (seconds  < 10 ? "0" + seconds : seconds);
    }

    SilicaFlickable {
        PullDownMenu {
            MenuItem {
                id: menuitem_download;
                text: qsTr("Download") + " " + currentitem.mediasize();
                visible: currentitem.mediaurl() !== "";
                onClicked: {
                    if (!mediadownloader.isReady(currentitem.mediaurl()))
                    {
                        mediadownloader.startDownload(currentitem.mediaurl());
                        mediadownloader.downloadReady.connect(onLoaded);
                        mediadownloader.downloadFailed.connect(onFailed);
                        media_busy.running = true;
                    }
                }
                Component.onCompleted: {
                    if (mediadownloader.isReady(currentitem.mediaurl()))
                    {
                        menuitem_download.text = menuitem_download.text + " ✓";
                        media_text.text = media_text.text + " ✓";
                        enabled = false
                    }
                }
                function onLoaded(url, localurl) {
                    console.log("Finished download");
                    media_busy.running = false;
                    media_slider.visible = true;
                    menuitem_download.text = menuitem_download.text + " ✓";
                    media_text.text = media_text.text + " ✓";
                    enabled = false

                    if (audioplayer.state == 1)
                    {
                        var pos = audioplayer.position;
                        audioplayer.stop();
                        manager.setCurrentMedia(mediadownloader.getLocalPath(currentitem.mediaurl()));

                        audioplayer.play();
                        audioplayer.setPosition(pos);

                        console.log("audioPlayer source changed to " + mediadownloader.getLocalPath(currentitem.mediaurl()) + " at " + pos);
                    }
                }
                function onFailed(url, localurl) {
                    console.log("Download failed");
                    banner.notify(qsTr("Download failed"));
                    media_busy.running = false;
                }
            }
        }

        PageHeader {
            id: pageHeader
            title: currentitem.title()
        }

        width: parent.width;
        contentHeight: column.height + pageHeader.height

        anchors {top: parent.top;bottom: media_bar.top}

        Column
        {
            id: column;

            width: page.width - Theme.paddingLarge;
            anchors {top: pageHeader.bottom;horizontalCenter: parent.horizontalCenter}

            height: description.height + feedicon.height + 3*spacing + fulltext_button.height + media_button.height + link_button.height;

            spacing: Theme.paddingLarge;

            Image {
                id: feedicon
                anchors.horizontalCenter: parent.horizontalCenter
                visible: !webview.visible
                source: imagedownloader.isReady(currentitem.imageurl()) ? imagedownloader.getLocalPath(currentitem.imageurl()) : ""
                fillMode: Image.PreserveAspectFit
                height: (sourceSize.height > page.height / 4) ? page.height / 4 : sourceSize.height
                width: parent.width;
                cache: false;

                Component.onCompleted: {
                    if (!imagedownloader.isReady(currentitem.imageurl()))
                        imagedownloader.downloadReady.connect(onLoaded);
                }
                function onLoaded(url, localurl) {
                    if (url === currentitem.imageurl()) {
                        source = localurl;
                    }
                }
            }

            Label {
                id: description
                width: parent.width
                visible: !webview.visible

                textFormat: currentitem.descriptionhtml() ? Text.RichText : Text.AutoText;

                wrapMode: Text.Wrap
                text: currentitem.description();
            }

            BackgroundItem {
                id: fulltext_button
                visible: currentitem.content() != "" && webview.visible == false

                width: parent.width
                height: fulltext_icon.height

                Image {
                    id: fulltext_icon
                    anchors.verticalCenter: parent.verticalCenter

                    width: sourceSize.width
                    height: sourceSize.height
                    asynchronous: true
                    smooth: true
                    fillMode: Image.PreserveAspectCrop
                    source: "image://theme/icon-m-document"
                }
                Label {
                    text: qsTr("view full content")
                    anchors {verticalCenter: parent.verticalCenter;left: fulltext_icon.right}
                }

                onClicked: {
                    //description.visible = false;
                    //feedicon.visible = false;
                    fulltext_button.visible = false;
                    column.height = 0;

                    webview.visible = true;

                    webview.loadHtml("<html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><body>"+currentitem.content()+"</body></html>");
                }
            }
            BackgroundItem {
                id: media_button
                visible: (currentitem.mediaurl() !== "") && (webview.visible == false)

                width: parent.width
                height: media_icon.height

                Image {
                    id: media_icon
                    anchors.verticalCenter: parent.verticalCenter

                    width: sourceSize.width
                    height: sourceSize.height
                    asynchronous: true
                    smooth: true
                    fillMode: Image.PreserveAspectCrop
                    source: "image://theme/icon-m-music"
                }
                Label {
                    id: media_text
                    text: currentitem.mediatype() + " " + formatSeconds(currentitem.mediaduration())
                    anchors  {verticalCenter: parent.verticalCenter;left: media_icon.right}
                }

                onClicked: {
                    media_bar.visible = true;

                    if (mediadownloader.isReady(currentitem.mediaurl()))
                        manager.setCurrentMedia(mediadownloader.getLocalPath(currentitem.mediaurl()));
                    else
                        manager.setCurrentMedia(currentitem.mediaurl());

                    audioplayer.play();
                }
            }
            BackgroundItem {
                id: link_button
                visible: (currentitem.link() !== "") && (webview.visible == false)

                width: parent.width
                height: link_icon.height

                Image {
                    id: link_icon
                    anchors.verticalCenter: parent.verticalCenter

                    width: sourceSize.width
                    height: sourceSize.height
                    asynchronous: true
                    smooth: true
                    fillMode: Image.PreserveAspectCrop
                    source: "image://theme/icon-m-link"

                }
                Label {
                    text: qsTr("Website")
                    anchors {verticalCenter: parent.verticalCenter;left: link_icon.right}
                }

                onClicked: {
                    Qt.openUrlExternally(currentitem.link());
                }
            }
        }


        ScrollDecorator { }
    }
    WebView {
        id: webview
        visible: false;
        width: parent.width
        height: page.height - media_bar.height - pageHeader.height;
        anchors.bottom: media_bar.top;

        onNavigationRequested: {
            console.log("NavRequest: " + request.url);

            if (request.navigationType == WebView.LinkClickedNavigation) {
                request.action = WebView.IgnoreRequest;
                Qt.openUrlExternally(request.url);
            } else {
                request.action = WebView.AcceptRequest;
            }
        }
    }
    BackgroundItem
    {
        id: media_bar
        visible: (audioplayer.state > 0) || (currentitem.mediaurl() != "")
        anchors.bottom: parent.bottom
        width: parent.width
        height: visible ? audioplayer.state > 0 ? media_play.height + 2 * Theme.paddingLarge : media_play.height + Theme.paddingLarge : 0

        Rectangle {
            anchors.fill: parent

            color: audioplayer.state > 0 ? "#A0000000" : "#40000000"
        }

        BackgroundItem {
            id: media_play

            anchors {left: parent.left;verticalCenter: parent.verticalCenter;leftMargin: Theme.paddingSmall}

            width: mediaIcon.width

            Image {
                id: mediaIcon
                anchors.centerIn: parent;

                width: sourceSize.width
                height: sourceSize.height
                asynchronous: true
                smooth: true
                fillMode: Image.PreserveAspectCrop
                source: (audioplayer.state == 1) ? "image://theme/icon-l-pause" : "image://theme/icon-l-play"
                clip: true

                BusyIndicator {
                    id: media_busy

                    running: parent.status === Image.Loading
                    anchors.centerIn: parent
                    size: BusyIndicatorSize.Medium
                }
            }
            onClicked: {
                if (audioplayer.state == 1)
                    audioplayer.pause();
                else
                {
                    if (audioplayer.state == 0)
                    {
                        if (mediadownloader.isReady(currentitem.mediaurl()))
                            manager.setCurrentMedia(mediadownloader.getLocalPath(currentitem.mediaurl()));
                        else
                            manager.setCurrentMedia(currentitem.mediaurl());
                    }
                    audioplayer.play();
                }
                media_slider.visible = true;
            }
        }
        Slider {
            id: media_slider
            anchors {verticalCenter: parent.verticalCenter;left: media_play.right;right: parent.right}

            visible: audioplayer.state > 0
            enabled: audioplayer.seekable

            minimumValue: 0
            maximumValue: (audioplayer.duration / 1000)
            value: (audioplayer.position / 1000)
            valueText: formatSeconds(value)
            label: currentplayitem !== 'undefined' ? currentplayitem.mediaduration() ? currentplayitem.mediatype() + " " + formatSeconds(currentplayitem.mediaduration()) : currentplayitem.mediatype() + " " + formatSeconds(audioplayer.duration / 1000) : ""

            onDownChanged: {
                console.log("Seek");
                audioplayer.setPosition(media_slider.value * 1000);
                audioplayer.positionChanged.connect(playerposChanged)
            }
            function playerposChanged (newpos)
            {
                if (!media_slider.down)
                    value = (newpos / 1000)
            }
        }
    }
    Banner {
        id: banner
    }
}





