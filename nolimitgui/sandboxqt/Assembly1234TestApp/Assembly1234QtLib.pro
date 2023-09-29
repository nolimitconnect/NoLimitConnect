# Global
TEMPLATE = lib
QT       -= gui
CONFIG += staticlib
CONFIG -= sharedlib

CONFIG += c++17

include(config_assembly_sandbox.pri)

include(Assembly1234QtLib.pri)

TARGET=AssemblyLib

DESTDIR = $$PWD/build-staticlibs

CONFIG(debug, debug|release){
    OBJECTS_DIR=.boxtestobjsd
}

CONFIG(release, debug|release){
    OBJECTS_DIR=.boxtestobjs
}


