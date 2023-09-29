QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(Assembly1234QtGui.pri)

DEST_STATIC_LIBS_DIR=$$PWD/../build-staticlibs
win32:{
STATIC_LIB_PREFIX = $${DEST_STATIC_LIBS_DIR}/
}

!win32:{
STATIC_LIB_PREFIX = $${DEST_STATIC_LIBS_DIR}/lib
}

android:{
    STATIC_LIB_SUFFIX = _arm64-v8a.a
}

unix:!android:{
    STATIC_LIB_SUFFIX = .a
}

win32:{
    STATIC_LIB_SUFFIX = .lib
}


LIBS +=  $${STATIC_LIB_PREFIX}AssemblyLib$${STATIC_LIB_SUFFIX}

# Default rules for deployment.
qnx: target.path = $$PWD/binQt

DISTFILES +=

contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    ANDROID_PACKAGE_SOURCE_DIR = \
        $$PWD/android
}

