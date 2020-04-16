#-------------------------------------------------
#
# Project created by QtCreator 2019-04-17T15:41:21
#
#-------------------------------------------------

QT       += core gui
QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mygui
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
    camera_calibration.cpp \
    point_collect.cpp \
    point_show.cpp \
    projector_calibration.cpp \

HEADERS += \
        mainwindow.h \
    camera_calibration.h \
    projector_calibration.h \
    point_show.h \
    point_collect.h \


FORMS += \
        mainwindow.ui \
    camera_calibration.ui \
    projector_calibration.ui \
    point_show.ui \
    point_collect.ui


RESOURCES += \
    rc.qrc

RC_FILE = app.rc


INCLUDEPATH += $$PWD/OpenCV/include
DEPENDPATH += $$PWD/OpenCV/include

LIBS += $$PWD/OpenCV/lib/libopencv_*.a
LIBS += $$PWD/OpenCV/bin/libopencv_*.dll
#LIBS += D:/bs/OpenCV/lib/libopencv_highgui430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_core430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_highgui430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_imgproc430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_features2d430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_calib3d430.dll.a
#LIBS += D:/bs/OpenCV/lib/libopencv_imgcodecs430.dll.a

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../opencv/build/x64/vc15/lib/ -lopencv_world430
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../opencv/build/x64/vc15/lib/ -lopencv_world430d
#else:unix: LIBS += -L$$PWD/../opencv/build/x64/vc15/lib/ -lopencv_world430

#INCLUDEPATH += $$PWD/../opencv/build/include
#DEPENDPATH += $$PWD/../opencv/build/include
