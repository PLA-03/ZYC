QT += core gui widgets sql printsupport

TARGET = LibraryManagementSystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    databasemanager.cpp \
    bookmanager.cpp \
    readermanager.cpp \
    borrowmanager.cpp \
    loginwindow.cpp \
    registerdialog.cpp

HEADERS += \
    mainwindow.h \
    databasemanager.h \
    bookmanager.h \
    readermanager.h \
    borrowmanager.h \
    loginwindow.h \
    registerdialog.h

FORMS += \
    mainwindow.ui \
    loginwindow.ui \
    registerdialog.ui

OTHER_FILES += \
    style.qss

CONFIG += c++11
CONFIG += utf8_source

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
