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

#include "ui/gui-project.hpp"

#include "arch/common/architecture-formula.hpp"

GuiProject::GuiProject(QQmlContext* projectContext,
                       ArchitectureFormula architectureFormula,
                       std::size_t memorySize,
                       QObject* parent)
: QObject(parent)
, _registerDescription(
      "EAX\tAB01CD23\tHH HH HH HH\n"
      " AX\tAB01\tHH HH\n"
      "  AH\tAB\tHH\n"
      "  AL\t01\tHH\n"
      "EBX\t56AB78CD90\tHH HH HH HH\n"
      " BX\t56AB\tHH HH\n"
      "  BH\t56\tHH\n"
      "  BL\tAB\tHH")
, _registerModel(_registerDescription)
, _editor(projectContext) {
}
