TARGET = MONSTER_tg

TEMPLATE = app

VERSION = 3.2.2

QT += \
    qml \
    quick \
    widgets

CONFIG += \
    c++11
CONFIG(debug, debug|release) {
    CONFIG += \
        console
}

DEFINES += \
    SW_VERSION=\\\"$$VERSION\\\"
CONFIG(release, debug|release) {
    DEFINES += \
        QT_NO_DEBUG_OUTPUT
}

CONFIG(debug, debug|release) {
    DESTDIR = $${OUT_PWD}/debug
}
CONFIG(release, debug|release) {
    DESTDIR = $${OUT_PWD}/release
}
MOC_DIR = $${DESTDIR}/.moc
OBJECTS_DIR = $${DESTDIR}/.obj
RCC_DIR = $${DESTDIR}/.rcc

win32 {
    INCLUDEPATH += \
        "Z:/OpenCV/current/build/include"
}
unix {
    INCLUDEPATH += \
        "/opt/opencv/current/include"
}

win32 {
    CONFIG(debug, debug|release) {
        LIBS += \
            -L"Z:/OpenCV/current/build/x64/vc14/lib/Debug" \
            -lopencv_core310d \
            -lopencv_highgui310d \
            -lopencv_imgproc310d \
            -lopencv_imgcodecs310d
    }
    CONFIG(release, debug|release) {
        LIBS += \
            -L"Z:/OpenCV/current/build/x64/vc14/lib/Release" \
            -lopencv_core310 \
            -lopencv_highgui310 \
            -lopencv_imgproc310 \
            -lopencv_imgcodecs310
    }
}
unix {
    LIBS += \
        -L"/opencv/current/build/lib" \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_imgcodecs
}

HEADERS += \
    applicationmanager.h \
    applicationutils.h \
    base.h \
    destinationmanager.h \
    enums.h \
    filemanager.h \
    logmanager.h \
    settingsmanager.h \
    sourcemanager.h \
    thumbnailgenerator.h \
    thumbnailgeneratorimpl.h \
    utils.h \
    windowmanager.h

SOURCES += \
    applicationmanager.cpp \
    applicationutils.cpp \
    base.cpp \
    destinationmanager.cpp \
    filemanager.cpp \
    logmanager.cpp \
    main.cpp \
    settingsmanager.cpp \
    sourcemanager.cpp \
    thumbnailgenerator.cpp \
    thumbnailgeneratorimpl.cpp \
    utils.cpp \
    windowmanager.cpp

RESOURCES += \
    resources.qrc

win32 {
    RC_ICONS = 1UpMushroom256x256.ico
}

win32 {
    OTHER_FILES += \
        "$$PWD/ffmpeg/bin/ffmpeg.exe" \
        "$$PWD/ffmpeg/bin/ffprobe.exe" \
        $(QTDIR)/bin/d3dcompiler_47.dll
    lessThan(QT_MINOR_VERSION, 7) {
        OTHER_FILES += \
            $(QTDIR)/bin/icudt54.dll \
            $(QTDIR)/bin/icuin54.dll \
            $(QTDIR)/bin/icuuc54.dll
    }
    CONFIG(debug, debug|release) {
        OTHER_FILES += \
            $(QTDIR)/bin/libEGLd.dll \
            $(QTDIR)/bin/libGLESv2d.dll \
            $(QTDIR)/bin/Qt5Cored.dll \
            $(QTDIR)/bin/Qt5Guid.dll \
            $(QTDIR)/bin/Qt5Networkd.dll \
            $(QTDIR)/bin/Qt5Qmld.dll \
            $(QTDIR)/bin/Qt5Quickd.dll \
            $(QTDIR)/bin/Qt5Widgetsd.dll \
            "Z:/OpenCV/current/build/x64/vc14/bin/Debug/opencv_core310d.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Debug/opencv_highgui310d.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Debug/opencv_imgcodecs310d.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Debug/opencv_imgproc310d.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Debug/opencv_videoio310d.dll"
    }
    CONFIG(release, debug|release) {
        OTHER_FILES += \
            $(QTDIR)/bin/libEGL.dll \
            $(QTDIR)/bin/libGLESv2.dll \
            $(QTDIR)/bin/Qt5Core.dll \
            $(QTDIR)/bin/Qt5Gui.dll \
            $(QTDIR)/bin/Qt5Network.dll \
            $(QTDIR)/bin/Qt5Qml.dll \
            $(QTDIR)/bin/Qt5Quick.dll \
            $(QTDIR)/bin/Qt5Widgets.dll \
            "Z:/OpenCV/current/build/x64/vc14/bin/Release/opencv_core310.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Release/opencv_highgui310.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Release/opencv_imgcodecs310.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Release/opencv_imgproc310.dll" \
            "Z:/OpenCV/current/build/x64/vc14/bin/Release/opencv_videoio310.dll"
    }
}

win32 {
    for(FILE, OTHER_FILES){
        QMAKE_POST_LINK += \
            $$quote(copy /Y \"$$shell_path($${FILE})\" \"$$shell_path($${DESTDIR})\"$$escape_expand(\n\t))
    }
}
