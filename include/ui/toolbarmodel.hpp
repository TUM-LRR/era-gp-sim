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

#ifndef TOOLBARMODEL_H
#define TOOLBARMODEL_H

#include <QObject>
#include <QQmlContext>

class ToolbarModel: public QObject{
    Q_OBJECT
public:
    ToolbarModel(QQmlContext* context);
    Q_INVOKABLE void run(int index);
    Q_INVOKABLE void runLine(int index);
    Q_INVOKABLE void runBreakpoint(int index);
    Q_INVOKABLE void stop(int index);
    Q_INVOKABLE void changeSystem(QByteArray s);
    void finishExecution();

private:
    QQmlContext* context;

signals:
    void disableStop();
};

#endif // TOOLBARMODEL_H
