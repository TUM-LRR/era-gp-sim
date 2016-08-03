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

#ifndef ERAGPSIM_ARCH_COMMON_UNIT_INFORMATION_CONTAINER_HPP
#define ERAGPSIM_ARCH_COMMON_UNIT_INFORMATION_CONTAINER_HPP

#include <functional>
#include <string>

#include "arch/common/unit-information.hpp"

struct UnitNameComparator {
  bool operator()(const UnitInformation& first,
                  const UnitInformation& second) const {
    return first.getName() == second.getName();
  }
};

namespace std {
template <>
struct hash<UnitInformation> {
  std::size_t operator()(const UnitInformation& unit) const {
    return std::hash<std::string>()(unit.getName());
  }
};
}

using UnitContainer = std::unordered_set<UnitInformation,
                                         std::hash<UnitInformation>,
                                         UnitNameComparator>;

#endif /* ERAGPSIM_ARCH_COMMON_UNIT_INFORMATION_CONTAINER_HPP */
