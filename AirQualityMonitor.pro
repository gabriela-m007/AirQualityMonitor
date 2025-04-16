QT += core gui network charts widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ApiService.cpp \
    DataAnalyzer.cpp \
    DataParser.cpp \
    DataStorage.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    ApiService.h \
    DataAnalyzer.h \
    DataParser.h \
    DataStorage.h \
    DataStructures.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
