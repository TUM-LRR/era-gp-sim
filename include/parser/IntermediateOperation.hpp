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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_OPERATION_HPP

#include <vector>
#include <string>
#include "SymbolTable.hpp"
#include "CompileState.hpp"
#include"FinalRepresentation.hpp"

/**
 * \class LineInterval
 * \brief Representa an interval of lines, denoted by an upper and lower line bound.
 */
using LineInterval = std::pair<unsigned int, unsigned int>;

/**
 * \class DummyMemoryAddress
 * \brief A memory address substitute as long as we do not have one.
 */
using DummyMemoryAddress = unsigned int;

/**
 * \var kNullAddress
 * \brief A substitute for a not-initialized address.
 */
static constexpr DummyMemoryAddress kNullAddress = 0;

/**
 * \class IntermediateOperation
 * \brief Represents an abstract assembler operation in the parser-internal intermediate form.
 */
class IntermediateOperation
{
public:
    /**
     * \fn IntermediateOperation::IntermediateOperation(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name)
     * \brief Instantiates a new IntermediateOperation with the given arguments. (only for subclass use!)
     * \param lines The line interval the operation occupies.
     * \param labels The vector of labels assigned to the operation.
     * \param name The name of the operation.
     */
    IntermediateOperation(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name)
        : _lines(lines), _labels(labels), _name(name), _address(kNullAddress)
        {}

    /**
     * \fn IntermediateOperation::IntermediateOperation(const IntermediateOperation& other)
     * \brief Default copy constructor.
     * \param other The source IntermediateOperation instance.
     */
    IntermediateOperation(const IntermediateOperation& other) = default;

    /**
     * \fn IntermediateOperation::IntermediateOperation(IntermediateOperation&& other)
     * \brief Default move constructor.
     * \param other The source IntermediateOperation instance.
     */
    IntermediateOperation(IntermediateOperation&& other) = default;

    /**
     * \fn IntermediateOperation::operator =(const IntermediateOperation& other)
     * \brief Default copy assignment operator.
     * \param other The source IntermediateOperation instance.
     */
    IntermediateOperation& operator =(const IntermediateOperation& other) = default;

    /**
     * \fn IntermediateOperation::operator =(IntermediateOperation&& other)
     * \brief Default move assignment operator.
     * \param other The source IntermediateOperation instance.
     */
    IntermediateOperation& operator =(IntermediateOperation&& other) = default;

    /**
     * \fn IntermediateOperation::~IntermediateOperation()
     * \brief Default destructor.
     */
    ~IntermediateOperation() = default;

    /**
     * \fn IntermediateOperation::execute(FinalRepresentation& finalRepresentator, const SymbolTable& table, CompileState& state)
     * \brief Executes the given operation (somehow).
     * \param finalRepresentator The FinalRepresentation for possible output.
     * \param table The SymbolTable for possible replacements.
     * \param state The CompileState to log possible errors.
     */
    virtual void execute(FinalRepresentation& finalRepresentator, const SymbolTable& table, CompileState& state) = 0;

    /**
     * \fn IntermediateOperation::enhanceSymbolTable(SymbolTable& table, CompileState& state)
     * \brief Enhances the symbol table by the labels of the operation.
     * \param table The SymbolTable to insert into.
     * \param state The CompileState to log possible errors.
     */
    virtual void enhanceSymbolTable(SymbolTable& table, CompileState& state);

    /**
     * \fn IntermediateOperation::address()
     * \brief Returns the memory address.
     * \return The memory address.
     */
    inline DummyMemoryAddress address()
    {
        return _address;
    }

protected:
    /**
     * \fn IntermediateOperation::determineMemoryPosition()
     * \brief Reserves space in memory for the operation and sets the address.
     */
    virtual void determineMemoryPosition() = 0;

    /**
     * \var IntermediateOperation::_lines
     * \brief The internal line interval.
     */
    LineInterval _lines;

    /**
     * \var IntermediateOperation::_labels
     * \brief The internal label list.
     */
    std::vector<std::string> _labels;

    /**
     * \var IntermediateOperation::_name
     * \brief The internal operation name.
     */
    std::string _name;

    /**
     * \var IntermediateOperation::_address
     * \brief The internal memory address.
     */
    DummyMemoryAddress _address;
};

#endif
