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

#include <iostream>
#include <string>
#include <vector>

#include "ast.hpp"
#include "big_integer.hpp"
#include "memory_access.hpp"


template <class IntType>
class CmdAdd : public AbstractSyntaxTreeNode<IntType> {
	using AbstractSyntaxTreeNode<IntType>::_children;

 public:
	/**
		 Constructs the CmdAdd node.

		 @param children Vector with the destination register node and the summand
		 nodes.
	*/
	CmdAdd(
			std::vector<typename AbstractSyntaxTreeNode<IntType>::NodePtr> children)
	: AbstractSyntaxTreeNode<IntType>(NodeType::INSTRUCTION) {
		for (auto& child : children) addChild(child);
	}

	/**
		 Calls recursively the child nodes, adds the returned values and saves them
		 into the destination register.

		 @param memAccess The memory block of the RAM including a set of registers
		 and methods for accessing and modyfying them.

		 @return Indicates how the operation exited.
	 */
	IntType getValue(MemoryAccess& memAccess) override {
		// first child is the destination register
		// result of getValue() for registers is their id
		auto destinationId = _children[0].getIdentifier();
		// summands
		auto leftSummand = _children[1].getValue(memAccess);
		auto rightSummand = _children[2].getValue(memAccess);

		// BigInteger is an integer with variable size, that can be instantiated
		// from MemoryValue
		auto sum = static_cast<BigInteger> leftSummand +
							 static_cast<BigInteger> rightSummand;

		memAccess.putRegister(destinationId, sum);

		return 0;
	}

	/**
		 Validates the corectness of the given syntax tree.

		 @return The result of the validation.
	 */
	bool validate() override {
		if (_children.size() != 3) return false;

		for (int i = 0; i < 3; i++) {
			if (_children[i].getType() != NodeType::REGISTER) return false;
		}

		return true;
	}

	// TODO
	std::vector<bool> assemble() override {
		return std::vector<bool>{};
	}
};
