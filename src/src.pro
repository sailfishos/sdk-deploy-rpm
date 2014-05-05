QT -= gui

CONFIG += link_pkgconfig
PKGCONFIG += packagekit-qt5

SOURCES += main.cpp

SOURCES += deployer.cpp
HEADERS += deployer.h

target.path = /usr/bin
TARGET = sdk-deploy-rpm
INSTALLS += target
