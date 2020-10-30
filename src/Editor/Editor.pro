#-------------------------------------------------
#
# Project created by QtCreator 2020-03-13T10:43:48
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = Editor
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

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qmainwidget.cpp \
    qpreviewitem.cpp \
    qpreviewwidget.cpp \
    qworkingareawidget.cpp \
    qworkingareaitem.cpp \
    qappdata.cpp \
    qtablineitem.cpp \
    qprojectconfigdialog.cpp \
    qimagebutton.cpp \
    qtablineconfigdialog.cpp \
    qtablinedetailwidget.cpp \
    qtablinewidget.cpp \
    qtablineitemlistdelegate.cpp \
    qworkingitemlistdelegate.cpp \
    qpreviewlistdelegate.cpp

HEADERS += \
    mainwindow.h \
    qmainwidget.h \
    qpreviewitem.h \
    qpreviewwidget.h \
    qworkingareawidget.h \
    qworkingareaitem.h \
    qappdata.h \
    qtablineitem.h \
    qprojectconfigdialog.h \
    qimagebutton.h \
    qtablineconfigdialog.h \
    qtablinedetailwidget.h \
    qtablinewidget.h \
    qtablineitemlistdelegate.h \
    qworkingitemlistdelegate.h \
    qpreviewlistdelegate.h

FORMS += \
        mainwindow.ui \
    qprojectconfigdialog.ui \
    qtablineconfigdialog.ui

INCLUDEPATH += \
    ../Include

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# link to project.lib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Project/release/ -lTABProject
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Project/debug/ -lTABProject
else:unix: LIBS += -L$$OUT_PWD/../Project/ -lTABProject

INCLUDEPATH += $$PWD/../Project
DEPENDPATH += $$PWD/../Project

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Project/release/libTABProject.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Project/debug/libTABProject.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Project/release/TABProject.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Project/debug/TABProject.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Project/libTABProject.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Utils/release/ -lUtils
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Utils/debug/ -lUtils
else:unix: LIBS += -L$$OUT_PWD/../Utils/ -lUtils

INCLUDEPATH += $$PWD/../Utils
DEPENDPATH += $$PWD/../Utils

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utils/release/libUtils.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utils/debug/libUtils.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utils/release/Utils.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../Utils/debug/Utils.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../Utils/libUtils.a

win32{
RC_FILE += editor.rc
DISTFILES += \
    editor.rc
}

RESOURCES += \
    resource.qrc
