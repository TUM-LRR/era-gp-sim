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

#ifndef ERAGPSIM_UI_EDITOR_GUI_PROJECT_HPP
#define ERAGPSIM_UI_EDITOR_GUI_PROJECT_HPP

#include <QObject>
#include <QQmlContext>
#include <QString>

#include "ui/editor-component.hpp"
#include "ui/registermodel.hpp"

class ArchitectureFormula;

/**
 * The GuiProject class. This class encapsulates all gui c++ components and the
 * core project component.
 * This version will be replaced by GuiProject, which is currently in
 * developement. This is just an incomplete placeholder to demonstrate the
 * project creation.
 *
 * That is also the reason why this is not fully documented at the
 * moment, sorry!
 *
 */
class GuiProject : public QObject {
  Q_OBJECT
 public:
  explicit GuiProject(QQmlContext* projectContext,
                      ArchitectureFormula architectureFormula,
                      std::size_t memorySize,
                      QObject* parent = 0);

  void run(){}
  void runLine(){}
  void runBreakpoint(){}
  void stop(){}
  void save(){}
  void saveAs(QString name){}
  void saveSnapshot(QString name){}
  void loadSnapshot(QString name){}


 private:
  QString _registerDescription;
  RegisterModel _registerModel;
  EditorComponent _editor;

 signals:

 public slots:
};

#endif// ERAGPSIM_UI_EDITOR_GUI_PROJECT_HPP
