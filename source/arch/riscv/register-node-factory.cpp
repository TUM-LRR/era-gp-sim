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
* along with this program. If not, see <http://www.gnu.org/licenses/>.*/

#include <memory>
#include <string>

#include "arch/riscv/register-node.hpp"
#include "arch/riscv/register-node-factory.hpp"
#include "common/utility.hpp"

namespace riscv {

RegisterNodeFactory::RegisterNodeFactory(const Architecture &arch) : AbstractRegisterNodeFactory(arch), _availableRegisters() {
    for(auto& unit : arch.getUnits()) {
        for(auto& registerInfo : unit) {
            _availableRegisters.insert(Utility::toLower(registerInfo.second.getName()));
        }
    }
}

RegisterNodeFactory::Node RegisterNodeFactory::createRegisterNode(
    const std::string &id) const {
  if (_availableRegisters.count(Utility::toLower(id)) > 0) {
    return std::make_unique<RegisterNode>(Utility::toLower(id));
  } else {
    return nullptr;
  }
}
}
