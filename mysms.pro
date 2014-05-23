# -------------------------------------------------
# Project created by QtCreator 2011-07-30T00:27:08
# -------------------------------------------------
QT += core \
    gui \
    network \
    webkit

isEqual(QT_MAJOR_VERSION, 5) {
    QT += webkitwidgets widgets multimedia
}

TARGET = mysms
TEMPLATE = app
CONFIG += static
SOURCES += networkcookiejar.cpp \
    main.cpp \
    mainwindow.cpp \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    jsinterface.cpp \
    qnetworkaccessmanagercustom.cpp \
    notificationpopup.cpp \
    notificationpopupmanager.cpp \
    usersettings.cpp \
    notificationsummarywidget.cpp \
    updatedialog.cpp \
    soundselector.cpp \
    webview.cpp \
    usersettingsdialog.cpp \
    networkreplyfilter.cpp
HEADERS += networkcookiejar.h \
    mainwindow.h \
    qtsingleapplication.h \
    qtlocalpeer.h \
    jsinterface.h \
    qnetworkaccessmanagercustom.h \
    notificationpopup.h \
    notificationpopupmanager.h \
    usersettings.h \
    notificationsummarywidget.h \
    updatedialog.h \
    soundselector.h \
    webview.h \
    globalsettings.h \
    usersettingsdialog.h \
    networkreplyfilter.h
FORMS +=
OTHER_FILES += 
RESOURCES += mysms.qrc
TRANSLATIONS = translation/mysms_de.ts

# Windows resource file
win32:RC_FILE = mysms.rc
win32:QMAKE_LFLAGS += -static-libgcc
macx:ICON = mysms.icns

CONFIG(debug, debug|release) {
} else {
   DEFINES += QT_NO_DEBUG
   DEFINES += QT_NO_DEBUG_OUTPUT
}

win32 {
    isEqual(QT_MAJOR_VERSION, 5) {
        QT += winextras
    }
}

unix {

    isEqual(QT_MAJOR_VERSION, 4) {
        QT += phonon
    }

    PREFIX = /usr
    BINDIR = $$PREFIX/bin
    DATADIR =$$PREFIX/share

    DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"

    INSTALLS += target desktop sounds icon32 icon64 icon128

    target.path =$$BINDIR

    desktop.path = $$DATADIR/applications
    desktop.files += $${TARGET}.desktop

    sounds.path = $$DATADIR/sounds/$$TARGET/
    sounds.files += sounds/*.wav

    icon32.path = $$DATADIR/icons/hicolor/32x32/apps
    icon32.files += resource/32x32/$${TARGET}.png

    icon64.path = $$DATADIR/icons/hicolor/64x64/apps
    icon64.files += resource/64x64/$${TARGET}.png

    icon128.path = $$DATADIR/icons/hicolor/128x128/apps
    icon128.files += resource/128x128/$${TARGET}.png
}
