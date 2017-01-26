/*
  * ERASIM Assembler Interpreter
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

#ifndef INPUTBUTTONMODEL_H
#define INPUTBUTTONMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputButtonModel : public QObject {
  Q_OBJECT
 public:
  InputButtonModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief wrotes clicked Button in memory
   * \param id  Id of clicked Button
   */
  Q_INVOKABLE void buttonClicked(unsigned int id);

  /**
   * \brief Returns value of start
   */
  Q_INVOKABLE QString getStart();

  /**
   * \brief sets start
   * \param start new value
   */
  Q_INVOKABLE void setStart(unsigned int start);

 private:
  QQmlContext* _context;
  /**
   * @brief the base addresss in the memory
   */
  unsigned int _start;
  MemoryAccess _memoryAccess;
};

#endif// INPUTBUTTONMODEL_H
