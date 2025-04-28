QT += core gui widgets network charts concurrent testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    ApiService.cpp \
    DataAnalyzer.cpp \
    DataParser.cpp \
    DataStorage.cpp \
    Main.cpp \
    MainWindow.cpp \
    TestDataAnalyzer.cpp \
    TestDataParser.cpp \
    TestDataStorage.cpp \
    #TestMain.cpp

HEADERS += \
    ApiService.h \
    DataAnalyzer.h \
    DataParser.h \
    DataStorage.h \
    DataStructures.h \
    MainWindow.h \
    TestDataAnalyzer.h \
    TestDataParser.h \
    TestDataStorage.h

FORMS += \
    MainWindow.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
