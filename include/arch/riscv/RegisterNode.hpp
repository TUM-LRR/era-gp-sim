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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <unordered_set>

#include "ast.hpp"
#include "memory_access.hpp"

// dummy set
std::unordered_set<int> registers{1, 2, 3};

template <class IntType>
class RegisterNode : public ConcreteValueNode<IntType> {
	using ConcreteValueNode<IntType>::_value;

 public:
	/**
	 * Constructs a register node.
	 *
	 * @param identifier The id number of the constructed register.
	 */
	RegisterNode(IntType identifier)
	: ConcreteValueNode<IntType>(NodeType::REGISTER, identifier) {
	}

	/**
	 * @return Identifier of the register.
	 */
	IntType getIdentifier() override {
		return _value;
	}

	/**
	 * @return The value held in the register
	 */
	IntType getValue(MemorryAccess& memAccess) override {
		// _value holds the id of the register
		return memAccess.getRegister(_value);
	}

	bool validate() override {
		if (!ConcreteValueNode<IntType>::validate()) return false;

		// registers is a set of register in the architecture
		if (registers.find(_value) == registers.end()) return false;

		return true;
	}

	// TODO
	std::vector<bool> assemble() override {
		return std::vector<bool>{};
	}
};
