# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-feedeebuster

CONFIG += sailfishapp

QT += multimedia
QT += dbus

DBUS_ADAPTORS += src/feedeebuster.xml

SOURCES += src/Feedeebuster.cpp \
    src/fbfeed.cpp \
    src/fbitemlistmodel.cpp \
    src/fbdownloader.cpp \
    src/fbmanager.cpp \
    src/fbfeedlistmodel.cpp

PKGCONFIG += Qt5DBus
PKGCONFIG += nemonotifications-qt5

OTHER_FILES += \
    qml/cover/CoverPage.qml \
    rpm/Feedeebuster.changes.in \
    rpm/Feedeebuster.spec \
    rpm/Feedeebuster.yaml \
    translations/*.ts

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-feedeebuster-de.ts

HEADERS += \
    src/fbfeed.h \
    src/fbitemlistmodel.h \
    src/fbdownloader.h \
    src/fbmanager.h \
    src/fbitem.h \
    src/fbfeedlistmodel.h

DISTFILES += \
    qml/pages/ItemPage.qml \
    harbour-feedeebuster.desktop \
    qml/harbour-feedeebuster.qml \
    qml/pages/AddFeedPage.qml \
    qml/cover/overlay.png \
    src/feedeebuster.xml \
    rpm/harbour-feedeebuster.yaml \
    qml/pages/ConfigPage.qml \
    qml/pages/ListPage.qml \
    qml/pages/FeedPage.qml \
    qml/pages/Banner.qml
