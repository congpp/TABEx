#-------------------------------------------------
#
# Project created by QtCreator 2020-07-15T10:33:24
#
#-------------------------------------------------

QT       += gui core

TARGET = TABProject
TEMPLATE = lib
CONFIG += staticlib

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
        tabproject.cpp \
    tabproject.cpp \
    tabproject.cpp \
    tabproject.cpp \
    qtpf.cpp \
    tablinemodel.cpp \
    projecthistory.cpp \
    imagemodel.cpp

HEADERS += \
        tabproject.h \
    tabproject.h \
    tabproject.h \
    tabproject.h \
    qtpf.h \
    tablinemodel.h \
    projecthistory.h \
    imagemodel.h

INCLUDEPATH += ../Include

unix {
    target.path = /usr/lib
    INSTALLS += target
}

#Other files
DISTFILES +=

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
