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

#ifndef ERAGPSIM_ARCH_RISCV_PROPERTIES_HPP
#define ERAGPSIM_ARCH_RISCV_PROPERTIES_HPP

#include <cstdint>

#include "arch/common/architecture-properties.hpp"

namespace riscv {
using Endianness = ArchitectureProperties::Endianness;
using SignedRepresentation = ArchitectureProperties::SignedRepresentation;

/** Byte order used in RISC-V architecture. */
const Endianness ENDIANNESS = Endianness::LITTLE;

/** Signed representation used in RISCV-V architecture */
const SignedRepresentation SIGNED_REPRESENTATION =
    SignedRepresentation::TWOS_COMPLEMENT;

/** Bits per byte in RISC-V architecture. */
const std::size_t BITS_PER_BYTE = 8;

/** Internal integer type to represent 32bit for arithmetic operations. */
using unsigned32_t = std::uint32_t;

/** Internal integer type to represent 64bit for arithmetic operations. */
using unsigned64_t = std::uint64_t;

/** Internal integer type to represent 32bit for arithmetic operations. */
using signed32_t = std::int32_t;

/** Internal integer type to represent 64bit for arithmetic operations. */
using signed64_t = std::int64_t;
}

#endif /* ERAGPSIM_ARCH_RISCV_PROPERTIES_HPP */
