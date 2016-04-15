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
                onClicked: {
                    if (!mediadownloader.isReady(currentitem.mediaurl()))
                    {
                        mediadownloader.startDownload(currentitem.mediaurl());
                        mediadownloader.downloadReady.connect(onLoaded);
                        media_busy.running = true;
                    }
                }
                Component.onCompleted: {
                    if (mediadownloader.isReady(currentitem.mediaurl()))
                    {
                        menuitem_download.color = Theme.secondaryColor;
                        menuitem_download.text = menuitem_download.text + " ✓";
                    }
                }
                function onLoaded(url, localurl) {
                    console.log("Finished download");

                    media_busy.running = false;
                    media_slider.visible = true;

                    menuitem_download.color = Theme.secondaryColor;
                    menuitem_download.text = menuitem_download.text + " ✓";

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
            }
        }

        PageHeader {
            id: pageHeader
            title: currentitem.title()
        }

        width: parent.width;
        contentHeight: column.height + pageHeader.height

        anchors.top: parent.top;
        anchors.bottom: media_bar.top

        Column
        {
            id: column;

            width: page.width - Theme.paddingLarge;
            anchors.top: pageHeader.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            height: description.height + feedicon.height + 3*spacing + fulltext_button.height + media_button.height;

            spacing: Theme.paddingLarge;

            Image {
                id: feedicon
                anchors.horizontalCenter: parent.horizontalCenter
                visible: imagedownloader.isReady(currentitem.imageurl()) && !webview.visible
                source: imagedownloader.getLocalPath(currentitem.imageurl())
                fillMode: Image.PreserveAspectFit
                height: (sourceSize.height > page.height / 4) ? page.height / 4 : sourceSize.height
                width: parent.width;
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
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: fulltext_icon.right
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
                    text: currentitem.mediatype() + " " + formatSeconds(currentitem.mediaduration())
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: media_icon.right
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
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: link_icon.right
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

            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter

            anchors.leftMargin: Theme.paddingSmall

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
                console.log("State " + manager.isplaying);

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
                media_label.visible = false;
            }
        }
        Slider {
            id: media_slider
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: media_play.right
            anchors.right: parent.right

            visible: audioplayer.state > 0
            enabled: audioplayer.seekable

            minimumValue: 0
            maximumValue: (audioplayer.duration / 1000)
            value: (audioplayer.position / 1000)
            valueText: formatSeconds(value)
            label: currentplayitem.mediaduration() ? currentplayitem.mediatype() + " " + formatSeconds(currentplayitem.mediaduration()) : currentplayitem.mediatype() + " " + formatSeconds(audioplayer.duration / 1000)

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
}





