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

#ifndef INPUTTEXTMODEL_HPP
#define INPUTTEXTMODEL_HPP

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

using length_t = unsigned ;

class InputTextModel : QObject {
  Q_OBJECT
 public:
  InputTextModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief Sets the new text which should be stored in the memory
   * \param text the new Text
   */
  Q_INVOKABLE void newText(QString text);

  /**
   * \brief Sets the new startindex
   * \param text the index
   */
  Q_INVOKABLE void setStart(unsigned int start);

  /**
   * \brief sets the new Maximum Length
   * \param text the new length
   */
  Q_INVOKABLE void setMaximumLength(length_t maximumLength);

  /**
   * \brief Gets the lenght
   * \return the length
   */
  Q_INVOKABLE length_t getMaximumLength();

  /**
   * \brief Gets the start index
   * \return the length
   */
  Q_INVOKABLE QString getStart();

 private:
  QQmlContext* _context;
  int _start;
  length_t _maximumLength;
  MemoryAccess _memoryAccess;

 signals:
  /**
   * \brief called when maximum Length is changed
   */
  void maximumLengthChanged();
};

#endif// INPUTTEXTMODEL_HPP
