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

#include "parser/expression-compiler-clike.hpp"

ExpressionCompiler<int8_t> CLikeExpressionCompilers::CLikeCompilerI8 =
    CLikeExpressionCompilers::createCLikeCompiler<int8_t>();

ExpressionCompiler<uint8_t> CLikeExpressionCompilers::CLikeCompilerU8 =
    CLikeExpressionCompilers::createCLikeCompiler<uint8_t>();

ExpressionCompiler<int16_t> CLikeExpressionCompilers::CLikeCompilerI16 =
    CLikeExpressionCompilers::createCLikeCompiler<int16_t>();

ExpressionCompiler<uint16_t> CLikeExpressionCompilers::CLikeCompilerU16 =
    CLikeExpressionCompilers::createCLikeCompiler<uint16_t>();

ExpressionCompiler<int32_t> CLikeExpressionCompilers::CLikeCompilerI32 =
    CLikeExpressionCompilers::createCLikeCompiler<int32_t>();

ExpressionCompiler<uint32_t> CLikeExpressionCompilers::CLikeCompilerU32 =
    CLikeExpressionCompilers::createCLikeCompiler<uint32_t>();

ExpressionCompiler<int64_t> CLikeExpressionCompilers::CLikeCompilerI64 =
    CLikeExpressionCompilers::createCLikeCompiler<int64_t>();

ExpressionCompiler<uint64_t> CLikeExpressionCompilers::CLikeCompilerU64 =
    CLikeExpressionCompilers::createCLikeCompiler<uint64_t>();
