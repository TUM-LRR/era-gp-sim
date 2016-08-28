TEMPLATE = app

QT += qml quick widgets
CONFIG += c++14

SOURCES += main.cpp \
    registeritem.cpp \
    registermodel.cpp \
    registerdata.cpp \
    qproject.cpp \
    syntaxhighlighter.cpp \
    editor-component.cpp

RESOURCES += qml.qrc

INCLUDEPATH += ../../include/

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    ../../include/ui/registeritem.hpp \
    ../../include/ui/registermodel.hpp \
    ../../include/ui/qproject.hpp \
    ../../include/ui/syntaxhighlighter.hpp \
    ../../include/ui/registerdata.hpp \
    ../../include/ui/editor-component.hpp
