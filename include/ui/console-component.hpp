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

#ifndef ERASIM_UI_CONSOLE_COMPONENT_HPP
#define ERASIM_UI_CONSOLE_COMPONENT_HPP

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

/**
 * The ConsoleComonent class is the C++ interface of the combined input output
 * console.
 */
class ConsoleComponent : public QObject {
  Q_OBJECT

 public:
  using size_t = std::size_t;

  /**
   * Create a new console component.
   * \param context The qml context of the parent project.
   * \param memoryAccess The memory access of the parent project.
   */
  ConsoleComponent(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * Appends text to the stored text in memory.
   *
   * \param text The new text.
   */
  Q_INVOKABLE void appendText(QString text);

  /**
    * Gets the Text from the Memory until a Nullbyte
    * or end of Memory is reached
    */
  Q_INVOKABLE QString getText();

  /**
   * Sets the new startindex.
   *
   * \param text the index
   */
  Q_INVOKABLE void setStart(size_t start);

  /**
   * Sets the address of the simulated interrupt.
   * \param interruptAddress The interrupt address.
   */
  Q_INVOKABLE void setInterruptAddress(size_t interruptAddress);

  /**
   * \return The address of the simulated interrupt.
   */
  Q_INVOKABLE size_t getInterruptAddress();

  /**
   * \return The current length of the text (in characters).
   */
  Q_INVOKABLE size_t getLength();

  /**
   * Gets the start index.
   *
   * \return the length
   */
  Q_INVOKABLE size_t getStart();

  /**
   * Sets the new Mode.
   *
   * \param deleteBuffer If set to true, only the last line of the text is
   * written to memory, else the whole text of the console is kept in memory.
   */
  Q_INVOKABLE void setDeleteBuffer(bool deleteBuffer);

  /**
   * \return True if only the last line is written to memory, false otherwise.
   */
  Q_INVOKABLE bool deleteBuffer();

  /** Checks wether or not the simulated console interrupt is set. */
  Q_INVOKABLE bool checkInterrupt();

  /** Sets the simulated interrupt. */
  Q_INVOKABLE void setInterrupt();

  /** Resets the simulated interrupt. */
  Q_INVOKABLE void resetInterrupt();

  /** Deletes the text data in the memory. */
  Q_INVOKABLE void deleteTextInMemory();

 private:
  /** The start address in memory. */
  size_t _start;

  /** Current length of the text (in characters). */
  size_t _length;

  /** The address of the simulated interrupt for the console. */
  size_t _interruptAddress;

  /** The component for accessing the memory. */
  MemoryAccess _memoryAccess;

  /** The current mode. */
  bool _deleteBuffer;

  /** Flag to save wether or not the interrupt was already accepted.
   * Assures that the interrupt is edge triggered.
   */
  bool _interruptTriggered;

 signals:

  /**
   * Called when the settings have changed.
   */
  void settingsChanged();
};

#endif  // ERASIM_UI_CONSOLE_COMPONENT_HPP
