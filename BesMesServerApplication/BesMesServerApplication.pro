QT += sql network core

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        $$files(src/*.cpp)

HEADERS += \
        $$files(src/*.h*) \
        $$files(libs/include/*.h*)

RESOURCES += \
        $$files(res/*.qrc)

# Название программы
TARGET = BesMesServer
# Директория, в которую будет помещён файл
DESTDIR = bin/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
