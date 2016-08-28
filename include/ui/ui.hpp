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

#ifndef ERAGPSIM_UI_UI_HPP_
#define ERAGPSIM_UI_UI_HPP_

#include <QObject>
#include <QQuickItem>
#include <QApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlComponent>
#include <vector>
#include <memory>

#include "ui/qproject.hpp"

class Ui : public QObject{
Q_OBJECT
public:

    /**
     * \brief Ui
     * \param argc argument count, passed by main function
     * \param argv argument vector, passed by main function
     */
    Ui(int& argc, char** argv);

    /**
     * Starts and runs the qml application
     *
     * This method is blocking, as it executes the message loop. It does only return when the application is stopped
     *
     * \return exit code of the application
     */
    int runUi();

    /**
     *  creates a new Project
     *
     */
    Q_INVOKABLE void addProject(QQuickItem* tabItem, QQmlComponent* projectComponent);

private:
    QApplication _qmlApplication;
    QQmlApplicationEngine _engine;



};

#endif /* ERAGPSIM_UI_UI_HPP_ */
