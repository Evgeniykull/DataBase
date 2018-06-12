#-------------------------------------------------
#
# Project created by QtCreator 2018-01-12T20:53:18
#
#-------------------------------------------------

QT       += core gui sql serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dataBaseFirebird
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    tree/treeitem.cpp \
    tree/treemodel.cpp \
    utils/jsonconvertor.cpp \
    utils/treadworker.cpp \
    utils/catalogswrither.cpp \
    port/portsettings.cpp \
    port/port.cpp \
    dialogs/addfuelsdialog.cpp \
    dialogs/adduserdialog.cpp \
    models/modeluser.cpp \
    dialogs/addtanksdialog.cpp \
    dialogs/addpointsdialog.cpp \
    dialogs/getcarddialog.cpp \
    models/modellimit.cpp \
    dialogs/addlimitsdialog.cpp \
    dialogs/addobjectdialog.cpp \
    models/modelhistory.cpp

HEADERS += \
        mainwindow.h \
    tree/treeitem.h \
    tree/treemodel.h \
    utils/jsonconvertor.h \
    utils/treadworker.h \
    utils/catalogswrither.h \
    port/portsettings.h \
    port/port.h \
    dialogs/addfuelsdialog.h \
    dialogs/adduserdialog.h \
    models/modeluser.h \
    dialogs/addtanksdialog.h \
    dialogs/addpointsdialog.h \
    dialogs/getcarddialog.h \
    models/modellimit.h \
    dialogs/addlimitsdialog.h \
    dialogs/addobjectdialog.h \
    utils/utils.h \
    models/modelhistory.h

FORMS += \
        mainwindow.ui \
    port/portsettings.ui \
    port/port.ui \
    dialogs/addfuelsdialog.ui \
    dialogs/adduserdialog.ui \
    dialogs/addtanksdialog.ui \
    dialogs/addpointsdialog.ui \
    dialogs/getcarddialog.ui \
    dialogs/addlimitsdialog.ui \
    dialogs/addobjectdialog.ui
