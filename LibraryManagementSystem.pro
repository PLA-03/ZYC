QT += core gui widgets sql printsupport

TARGET = LibraryManagementSystem
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    databasemanager.cpp \
    bookmanager.cpp \
    readermanager.cpp \
    borrowmanager.cpp

HEADERS += \
    mainwindow.h \
    databasemanager.h \
    bookmanager.h \
    readermanager.h \
    borrowmanager.h

FORMS += \
    mainwindow.ui

OTHER_FILES += \
    style.qss

CONFIG += c++11

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
