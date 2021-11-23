QT       += sql network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/besconfigeditor.cpp \
    src/clientconnection.cpp \
    src/configeditor.cpp \
    src/databaseconnection.cpp \
    src/logsystem.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/multithreadtcpserver.cpp \
    src/serverstatisticscounter.cpp \
    src/serverworker.cpp \
    src/timecounter.cpp

HEADERS += \
    src/besProtocol.h \
    src/besconfigeditor.h \
    src/clientconnection.h \
    src/configeditor.h \
    src/databaseconnection.h \
    src/logsystem.h \
    src/mainwindow.h \
    src/multithreadtcpserver.h \
    src/serverstatisticscounter.h \
    src/serverworker.h \
    src/timecounter.h \
    src/user.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    src/mainwindow.ui

SUBDIRS += \
    tests/TestClientServerCommunication
