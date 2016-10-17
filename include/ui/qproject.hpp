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

#ifndef INCLUDE_UI_EDITOR_QPROJECT_H
#define INCLUDE_UI_EDITOR_QPROJECT_H

#include <QObject>
#include <QQmlContext>

#include "ui/editor-component.hpp"
#include "ui/registermodel.hpp"
#include "ui/memorycomponentpresenter.hpp"

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
class QProject : public QObject {
  Q_OBJECT
 public:
  explicit QProject(QQmlContext* projectContext, QObject* parent = 0);


 private:
  QString _registerDescription;
  MemoryAccess _memoryAccess;
  RegisterModel _registerModel;
  EditorComponent _editor;
  MemoryComponentPresenter _memory;

 signals:

 public slots:
};

#endif// INCLUDE_UI_EDITOR_QPROJECT_H
