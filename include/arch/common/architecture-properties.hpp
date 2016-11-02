/*
*C++ Assembler Interpreter
*Copyright (C) 2016 Chair of Computer Architecture
*at Technical University of Munich
*
*This program is free software: you can redistribute it and/or modify
*it under the terms of the GNU General Public License as published by
*the Free Software Foundation, either version 3 of the License, or
*(at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ERAGPSIM_ARCH_ARCHITECTURE_PROPERTIES_HPP
#define ERAGPSIM_ARCH_ARCHITECTURE_PROPERTIES_HPP

/**
 * Contains some enum and type declarations used for information objects.
 */
namespace ArchitectureProperties {

/** Type for the processor word size, in bits. */
using word_size_t = unsigned short;

/** Type for the architectures byte size, in bits */
using byte_size_t = unsigned short;

/*
 * The different kinds of endianness an extension may support.
 *
 * For clarification, let our 32-bit word be 0xDEADBEEF:
 * - Little Endian: low-order values at lower addresses: 0xEFBEADDE
 * - Big Endian: high-order values at lower addresses: 0xDEADBEEF
 * - Mixed Endian: a mixture of the first two, e.g. storing 32-bit words
 * in
 *   little-endian but swapping 16-bit portions (as if the word-size were
 *   actually 16-bit): 0xADDEEFBE
 * - Bi Endian: Allows for dynamic configuration of the system's endianess
 * to
 *   be either little or big endian (usually at startup).
 */
enum class Endianness { LITTLE, BIG, MIXED, BI };

/**
 * Describes how the architecture handles misaligned accesses.
 *
 * Strict means the architecture does not allow misaligned memory accesses.
 * Relaxed means it does.
 */
enum class AlignmentBehavior { ALIGN_STRICT, ALIGN_RELAXED };

/**
 * Describes how the architecture represents signed values.
 *
 * - TWOS_COMPLEMENT: The most common representation of signed values as an
 *                    unsigned number's complement, plus one.
 * - ONES_COMPLEMENT: Represents signed values entirely through their
 *                    complement. As such, all-zeros and all-ones are both
 *                    representations of zero (+0 and -0, respectively). Also,
 *                    there is an issue with "end-around-carries".
 * - SIGN_BIT: An explicit sign bit to indicate if the given unsigned
 *             representation of a number is to be interpreted as negative or
 *             positive. This does not allow for the natural arithmetic
 *             semantics we are used to from two's (and, to some extent, one's)
 *             complement.
 *
 * https://en.wikipedia.org/wiki/Signed_number_representations
 */
enum class SignedRepresentation { TWOS_COMPLEMENT, ONES_COMPLEMENT, SIGN_BIT };
}

#endif /* ERAGPSIM_ARCH_ARCHITECTURE_PROPERTIES_HPP */
