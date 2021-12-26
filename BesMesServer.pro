QT       += sql network core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    src/besconfigreader.cpp \
    src/beslogsystem.cpp \
    src/clientconnection.cpp \
    src/databaseconnection.cpp \
    src/emailsender.cpp \
    src/logsystem.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/multithreadtcpserver.cpp \
    src/projectstructuredefender.cpp \
    src/serverstatisticscounter.cpp \
    src/serverworker.cpp \
    src/timecounter.cpp \
    src/user.cpp

HEADERS += \
    libs/include/toml.hpp \
    src/besconfigreader.h \
    src/beslogsystem.h \
    src/besprotocol.h \
    src/clientconnection.h \
    src/databaseconnection.h \
    src/emailsender.h \
    src/logsystem.h \
    src/mainwindow.h \
    src/multithreadtcpserver.h \
    src/projectstructuredefender.h \
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

RESOURCES += \
    res/res.qrc

DISTFILES +=

