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

#ifndef ERAGPSIM_ARCH_COMMON_NODE_FACTORY_COLLECTION_MAKER_HPP
#define ERAGPSIM_ARCH_COMMON_NODE_FACTORY_COLLECTION_MAKER_HPP

#include "arch/common/abstract-factory-types.hpp"
#include "arch/common/architecture.hpp"
#include "arch/common/node-factory-collection.hpp"

class Architecture;

namespace NodeFactoryCollectionMaker {
/**
 * Creates a NodeFactoryCollection for the given architecture.
 *
 * This is the preferered way to create a factory collection. This function
 * checks the architecture's name to determine which FactoryTypes to use to call
 * the `Create` function.
 *
 * \param architecture The architecture to create the collection for.
 *
 * \return A NodeFactoryCollection for the requested Architecture.
 */
NodeFactoryCollection CreateFor(const Architecture& architecture);

/**
 * Creates a NodeFactoryCollection for the given architecture.
 *
 * This function explicitly requires the FactoryTypes to create.
 *
 * \tparam FactoryTypes The collection of factory types to create.
 * \param architecture The architecture to create the collection for.
 *
 * \return A NodeFactoryCollection for the requested Architecture.
 */
template <typename FactoryTypes>
NodeFactoryCollection Create(const Architecture& architecture) {
  return NodeFactoryCollection::Create<FactoryTypes>(architecture.getInstructions(), architecture);
}
}

#endif /* ERAGPSIM_ARCH_COMMON_NODE_FACTORY_COLLECTION_MAKER_HPP */
