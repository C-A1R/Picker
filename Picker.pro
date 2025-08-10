QT += core gui sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBDIR = $$PWD/lib
win32: LIBDIR = $$LIBDIR/win32
unix:  LIBDIR = $$LIBDIR/unix

LIBS += -L$$LIBDIR/ -lPDFWriter
LIBS += -L$$LIBDIR/ -lFreeType
LIBS += -L$$LIBDIR/ -lLibAesgm
LIBS += -L$$LIBDIR/ -lZlib
unix {
    LIBS += -L$$LIBDIR/ -lLibPng
    LIBS += -L$$LIBDIR/ -lLibTiff
    LIBS += -L$$LIBDIR/ -lLibJpeg
}

VERSION = 1.5
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

SOURCES += \
    FileExplorerWidget/FileExplorerWidget.cpp \
    FileExplorerWidget/FileSystemView.cpp \
    FileExplorerWidget/FileSystemModel.cpp \
    ProjectWidget/Commands/ItemsCheckedCmd.cpp \
    ProjectWidget/Commands/ResultHolderCheckedCmd.cpp \
    ProjectWidget/ProgectDelegate.cpp \
    ProjectWidget/ProjectItem.cpp \
    ProjectWidget/ProjectModel.cpp \
    ProjectWidget/ProjectTreeView.cpp \
    ProjectWidget/ProjectWidget.cpp \
    MainWindow.cpp \
    PdfBuilder/AbstractPdfBuilder.cpp \
    PdfBuilder/ToProjectAndSeparateDirectoriesPdfBuilder.cpp \
    PdfBuilder/ToProjectDirectoriesPdfBuilder.cpp \
    PdfBuilder/ToSeparateDirectoryPdfBuilder.cpp \
    Settings.cpp \
    SqlMgr.cpp \
    main.cpp

HEADERS += \
    FileExplorerWidget/FileExplorerEnums.h \
    FileExplorerWidget/FileExplorerWidget.h \
    FileExplorerWidget/FileSystemView.h \
    FileExplorerWidget/FileSystemModel.h \
    ProjectWidget/Commands/ItemsCheckedCmd.h \
    ProjectWidget/Commands/ResultHolderCheckedCmd.h \
    ProjectWidget/ProgectDelegate.h \
    ProjectWidget/ProjectEnums.h \
    ProjectWidget/ProjectItem.h \
    ProjectWidget/ProjectModel.h \
    ProjectWidget/ProjectTreeView.h \
    ProjectWidget/ProjectWidget.h \
    MainWindow.h \
    PdfBuilder/AbstractPdfBuilder.h \
    PdfBuilder/IPdfBuilder.h \
    PdfBuilder/ToProjectAndSeparateDirectoriesPdfBuilder.h \
    PdfBuilder/ToProjectDirectoriesPdfBuilder.h \
    PdfBuilder/ToSeparateDirectoryPdfBuilder.h \
    Settings.h \
    SqlMgr.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

RC_FILE = picker.rc

DISTFILES += \
    README.md
