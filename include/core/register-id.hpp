/* C++ Assembler Interpreter
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

#ifndef ERAGPSIM_CORE_REGISTERID_HPP_
#define ERAGPSIM_CORE_REGISTERID_HPP_

// The information to uniquely identify a register
struct RegisterID {
  int address; /**< Brief Index of this Register within the vector of Registers
                 with no parents */
  int begin;  /**< Brief First index within the transitive grand-parent Register
                 with no parents of this Register*/
  int end;    /**< Brief First index within the transitive grand-parent Register
                 with no parents no longer within this Register*/
  /**
   * \brief Default constructor. Constructs an empty RegisterID
   */
  RegisterID() = default;
  /**
   * \brief Constructs an RegisterID with address addres, begin bgein and end
   *        end
   * \param address Index of this Register within the vector of Registers with
   *        no parents
   * \param begin First index within the transitive grand-parent Register with
   *        no parents of this Register
   * \param end First index within the transitive grand-parent Register with no
   *        parents no longer within this Register
   */
  RegisterID(int address, int begin, int end)
  : address{address}, begin{begin}, end{end} {
  }
  /**
   * \brief Move constructor. Constructs the RegisterID with the contents of
   *        other using move semantics.
   * \param other another RegisterID to be used as source to initialize the
   *        elements of the RegisterID with
   */
  RegisterID(RegisterID&& other) = default;
  /**
   * \brief Copy constructor. Constructs the RegisterID with the copy of the
   *        contents of other.
   * \param other another RegisterID to be used as source to initialize the
   *        elements of the RegisterID with
   */
  RegisterID(const RegisterID& other) = default;
  /**
   * \brief Move constructor. Constructs the RegisterID with the contents of
   *        other using move semantics.
   * \param other another RegisterID to be used as source to initialize the
   *        elements of the RegisterID with
   */
  RegisterID& operator=(RegisterID&& other) = default;
  /**
   * \brief Copy constructor. Constructs the RegisterID with the copy of the
   *        contents of other.
   * \param other another RegisterID to be used as source to initialize the
   *        elements of the RegisterID with
   */
  RegisterID& operator=(const RegisterID& other) = default;
  /**
   * \brief Destructs the RegisterID
   */
  ~RegisterID() = default;
};

#endif// ERAGPSIM_CORE_REGISTERID_HPP_
