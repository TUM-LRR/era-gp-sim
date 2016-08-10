/*
 * C++ Assembler Interpreter
 * Copyright (C) 2016 Chair of Computer Architecture
 * at Technical University of Munich
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "ui/registermodel.hpp"
#include "ui/editor-component.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString registerDescription =
        "EAX\tAB01CD23\tHH HH HH HH\n"
        " AX\tAB01\tHH HH\n"
        "  AH\tAB\tHH\n"
        "  AL\t01\tHH\n"
        "EBX\t56AB78CD90\tHH HH HH HH\n"
        " BX\t56AB\tHH HH\n"
        "  BH\t56\tHH\n"
        "  BL\tAB\tHH";
    RegisterModel registerModel(registerDescription);

    QQmlApplicationEngine engine;

    EditorComponent editor;
    // TODO: set only for one component
    engine.rootContext()->setContextProperty("editor", &editor);

    engine.rootContext()->setContextProperty("registerModel", &registerModel);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
