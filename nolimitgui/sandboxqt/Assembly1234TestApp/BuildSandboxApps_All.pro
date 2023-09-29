

TEMPLATE     = subdirs
CONFIG += no_docs_target


SUBDIRS += $$PWD/Assembly1234TestApp/Assembly1234QtGui.pro
SUBDIRS += $$PWD/Assembly1234TestApp/Assembly1234QtLib.pro

Assembly1234QtGui.pro.depends = Assembly1234QtLib.pro
