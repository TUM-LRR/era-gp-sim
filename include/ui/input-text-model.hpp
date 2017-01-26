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

#ifndef INPUTTEXTMODEL_HPP
#define INPUTTEXTMODEL_HPP

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputTextModel : public QObject {
  Q_OBJECT

 public:
  using size_t = std::size_t;


  /**
   * \brief The Modes which are possible
   */
  enum Mode { ARRAY_BASED, PIPELIKE };

  InputTextModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * Sets the new text which should be stored in the memory.
   *
   * \param text the new Text
   */
  Q_INVOKABLE void newText(QString text);

  /**
   * Sets a number in the memory, used for left/up/etc.
   *
   * \param number the number
   */
  Q_INVOKABLE void newNumber(size_t number);

  /**
   * Sets the new startindex.
   *
   * \param text the index
   */
  Q_INVOKABLE void setStart(size_t start);

  /**
   * Sets the new Maximum Length.
   *
   * \param text the new length
   */
  Q_INVOKABLE void setMaximumLength(size_t maximumLength);

  /**
   * Gets the lenght.
   *
   * \return the length
   */
  Q_INVOKABLE size_t getMaximumLength();

  /**
   * Gets the start index.
   *
   * \return the length
   */
  Q_INVOKABLE QString getStart();

  /**
   * Sets the new Mode.
   *
   * \param text the new mode
   */
  Q_INVOKABLE void setMode(int mode);

  /**
   * Gets the current mode.
   *
   * \return the length
   */
  Q_INVOKABLE int getMode();

 private:
  /** The context property. */
  QQmlContext* _context;

  /** The start address in memory. */
  size_t _start;

  /** The maximum length of the input. */
  size_t _maximumLength;

  /** The component for accessing the memory. */
  MemoryAccess _memoryAccess;

  /** The current mode. */
  Mode _mode;

 signals:

  /**
   * Called when maximum Length has changed.
   */
  void maximumLengthChanged();

  /**
   * Called when mode has changed.
   */
  void modeChanged();
};

#endif// INPUTTEXTMODEL_HPP
