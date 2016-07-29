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

#ifndef ERAGPSIM_ARCH_COMMON_REGISTER_INFORMATION_CONTAINER_HPP
#define ERAGPSIM_ARCH_COMMON_REGISTER_INFORMATION_CONTAINER_HPP

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_set>

#include "arch/common/register-information.hpp"

struct RegisterComparator {
  bool operator()(const RegisterInformation& first,
                  const RegisterInformation& second) const {
    return first.getID() == second.getID();
  }
};

namespace std {
template <>
struct hash<RegisterInformation> {
  std::size_t operator()(const RegisterInformation& unit) const {
    return std::hash<std::size_t>()(unit.getID());
  }
};
}

using RegisterContainer = std::unordered_set<RegisterInformation,
                                             std::hash<RegisterInformation>,
                                             RegisterComparator>;

#endif /* ERAGPSIM_ARCH_COMMON_REGISTER_INFORMATION_CONTAINER_HPP */
