# -------------------------------------------------
# Project created by QtCreator 2011-07-30T00:27:08
# -------------------------------------------------
QT += core \
    gui \
    network \
    webkit webkitwidgets \
    widgets

TARGET = mysms
TEMPLATE = app
CONFIG += static
SOURCES += networkcookiejar.cpp \
    kdenotification.cpp \
    main.cpp \
    mainwindow.cpp \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    jsinterface.cpp \
    logger.cpp \
    qtlockedfile.cpp \
    qnetworkaccessmanagercustom.cpp \
    notificationpopup.cpp \
    notificationpopupmanager.cpp \
    tabdialog.cpp \
    usersettings.cpp \
    notificationsummarywidget.cpp \
    updatedialog.cpp \
    soundselector.cpp \
    filedownloader.cpp \
    webview.cpp
HEADERS += networkcookiejar.h \
    kdenotification.h \
    mainwindow.h \
    qtsingleapplication.h \
    qtlocalpeer.h \
    jsinterface.h \
    logger.h \
    qtlockedfile.h \
    qnetworkaccessmanagercustom.h \
    notificationpopup.h \
    notificationpopupmanager.h \
    tabdialog.h \
    usersettings.h \
    notificationsummarywidget.h \
    updatedialog.h \
    soundselector.h \
    filedownloader.h \
    globalSettings.h \
    webview.h
FORMS +=
OTHER_FILES += 
RESOURCES += mysms.qrc
TRANSLATIONS = mysms_de.ts

# Windows resource file
win32:RC_FILE = mysms.rc
win32:QMAKE_LFLAGS += -static-libgcc
macx:ICON = mysms.icns
