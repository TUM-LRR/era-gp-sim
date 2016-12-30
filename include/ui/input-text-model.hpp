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

class InputTextModel : public QObject {
  Q_OBJECT

 public:
  using size_t = size_t;

  enum class Mode { ARRAY_BASED, PIPELIKE };

  InputTextModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief Sets the new text which should be stored in the memory
   * \param text the new Text
   */
  Q_INVOKABLE void newText(QString text);

  /**
   * \brief Sets a number in the memory, used for left/up/etc.
   * \param number the number
   */
  Q_INVOKABLE void newNumber(size_t number);

  /**
   * \brief Sets the new startindex
   * \param text the index
   */
  Q_INVOKABLE void setStart(size_t start);

  /**
   * \brief sets the new Maximum Length
   * \param text the new length
   */
  Q_INVOKABLE void setMaximumLength(size_t maximumLength);

  /**
   * \brief Gets the lenght
   * \return the length
   */
  Q_INVOKABLE size_t getMaximumLength();

  /**
   * \brief Gets the start index
   * \return the length
   */
  Q_INVOKABLE QString getStart();

  /**
   * \brief sets the new Mode
   * \param text the new mode
   */
  Q_INVOKABLE void setMode(int mode);

  /**
   * \brief Gets the mode
   * \return the length
   */
  Q_INVOKABLE int getMode();

 private:
  /**
   * \brief the context property
   */
  QQmlContext* _context;

  /**
   * \brief the start address in memory
   */
  size_t _start;

  /**
   * \brief the maximum length of the input
   */
  size_t _maximumLength;

  /**
   * \brief the component for accessing the memory
   */
  MemoryAccess _memoryAccess;

  /**
   * \brief the current mode
   */
  Mode _mode;

 signals:
  /**
   * \brief called when maximum Length has changed
   */
  void maximumLengthChanged();

  /**
   * \brief called when mode has changed
   */
  void modeChanged();
};

#endif  // INPUTTEXTMODEL_HPP
