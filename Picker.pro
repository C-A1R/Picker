QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

LIBS += -L$$PWD/lib/ -lPDFWriter
LIBS += -L$$PWD/lib/ -lFreeType
LIBS += -L$$PWD/lib/ -lLibAesgm
LIBS += -L$$PWD/lib/ -lZlib

SOURCES += \
    BuildWidget/BuildWidget.cpp \
    BuildWidget/ProjectModel.cpp \
    BuildWidget/ProjectProxyModel.cpp \
    BuildWidget/ProjectTreeView.cpp \
    FileSystemWidget/FileSystemWidget.cpp \
    MainWindow.cpp \
    PdfBuilder/AbstractPdfBuilder.cpp \
    PdfBuilder/ToDefenitFolderPdfBuilder.cpp \
    PdfBuilder/ToParentAndDefenitFolderPdfBuilder.cpp \
    PdfBuilder/ToParentFoldersPdfBuilder.cpp \
    Settings.cpp \
    main.cpp

HEADERS += \
    BuildWidget/BuildWidget.h \
    BuildWidget/ProjectModel.h \
    BuildWidget/ProjectProxyModel.h \
    BuildWidget/ProjectTreeView.h \
    FileSystemWidget/FileSystemWidget.h \
    MainWindow.h \
    PdfBuilder/AbstractPdfBuilder.h \
    PdfBuilder/IPdfBuilder.h \
    PdfBuilder/ToDefenitFolderPdfBuilder.h \
    PdfBuilder/ToParentAndDefenitFolderPdfBuilder.h \
    PdfBuilder/ToParentFoldersPdfBuilder.h \
    Settings.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

RC_FILE = picker.rc
