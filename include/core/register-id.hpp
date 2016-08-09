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

#ifndef ERAGPSIM_CORE_REGP_HPP_
#define ERAGPSIM_CORE_REGP_HPP_

// The information to uniquely identify a register
struct RegisterID {
  int _adress;
  int _begin;
  int _end;
  RegisterID() = default;
  RegisterID(int adress, int begin, int end)
  : _adress{adress}, _begin{begin}, _end{end} {
  }
  RegisterID(RegisterID&& other)      = default;
  RegisterID(const RegisterID& other) = default;
  RegisterID& operator=(RegisterID&& other) = default;
  RegisterID& operator=(const RegisterID& other) = default;
  ~RegisterID()                                  = default;
};

#endif// ERAGPSIM_CORE_REGP_HPP_
