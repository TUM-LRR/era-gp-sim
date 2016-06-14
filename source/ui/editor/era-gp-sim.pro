TEMPLATE = app

QT += qml quick widgets

CONFIG += c++14

SOURCES += main.cpp \
    syntaxhighlighter.cpp \
    qproject.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    syntaxhighlighter.hpp \
    qproject.hpp
