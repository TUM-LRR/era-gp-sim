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
#include "arch/riscv/word-instruction-wrapper.hpp"

//namespace riscv {

//using SizeType = WordInstructionWrapper::SizeType;

//SizeType WordInstructionWrapper::performIntegerOperation(SizeType op1, SizeType op2) const {
//    SizeType result = _wrapped->performIntegerOperation(op1, op2);

//    if(_signedResult && getSign(result) == SizeType(1)) {
//        result = (~result)+1;
//        result = result & SIGNED_WORD_MASK_LOWER30;//preserve lower 30 bit
//        result = ~result +1;
//        return result;
//    }else{
//        return result & WORD_MASK_LOWER31;
//    }
//}

//SizeType WordInstructionWrapper::getSign(SizeType n) const {
//    SizeType signBit = n & (SizeType(1) << 63) >> 63;
//    return signBit;
//}

//}
