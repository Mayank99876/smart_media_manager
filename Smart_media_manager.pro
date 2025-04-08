QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = media_sorter
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    file_scanner.cpp

HEADERS += \
    mainwindow.h \
    file_scanner.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    icons.qrc
