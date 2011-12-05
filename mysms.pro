# -------------------------------------------------
# Project created by QtCreator 2011-07-30T00:27:08
# -------------------------------------------------
QT += core \
    gui \
    network \
    webkit
TARGET = mysms
TEMPLATE = app
CONFIG += static
SOURCES += networkcookiejar.cpp \
    kdenotification.cpp \
    main.cpp \
    mainwindow.cpp \
    qtsingleapplication.cpp \
    qtlocalpeer.cpp \
    jsinterface.cpp
HEADERS += networkcookiejar.h \
    kdenotification.h \
    mainwindow.h \
    qtsingleapplication.h \
    qtlocalpeer.h \
    jsinterface.h
FORMS += 
OTHER_FILES += 
RESOURCES += mysms.qrc

# Windows resource file
win32:RC_FILE = mysms.rc
win32:QMAKE_LFLAGS += -static-libgcc
macx:ICON = mysms.icns
