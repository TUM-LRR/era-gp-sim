TEMPLATE = app

QT += qml quick
CONFIG += c++11

SOURCES += main.cpp \
    registeritem.cpp \
    registermodel.cpp \
    qproject.cpp \
    syntaxhighlighter.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    registeritem.hpp \
    registermodel.hpp \
    qproject.hpp \
    syntaxhighlighter.hpp
