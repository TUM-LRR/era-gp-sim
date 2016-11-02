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

#ifndef MENUBARMODEL_H
#define MENUBARMODEL_H

#include <QObject>
#include <QQmlContext>

class MenubarModel : public QObject{
    Q_OBJECT
public:
    MenubarModel(QQmlContext* context);
    Q_INVOKABLE void open(QByteArray name);
    Q_INVOKABLE void save(int index);
    Q_INVOKABLE void saveAs(QByteArray name, int index);
    Q_INVOKABLE void newTab(QByteArray arch, QByteArray name);
    Q_INVOKABLE void snapshot(QByteArray name, int index);
    Q_INVOKABLE void closeProject(int index);
    Q_INVOKABLE void settings();
    Q_INVOKABLE void help();

private:
    QQmlContext* context;


};

#endif // MENUBARMODEL_H
