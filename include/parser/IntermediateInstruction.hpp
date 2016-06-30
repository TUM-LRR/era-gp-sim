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

#ifndef ERAGPSIM_PARSER_INTERMEDIATE_INSTRUCTION_HPP
#define ERAGPSIM_PARSER_INTERMEDIATE_INSTRUCTION_HPP

#include <vector>
#include <string>
#include <map>
#include <memory>
#include "IntermediateOperation.hpp"
#include "../arch/AbstractNodeFactories.hpp"
#include "SymbolTable.hpp"

/**
 * \class IntermediateInstruction
 * \brief Represents a machine instruction in the parser-internal intermediate form.
 */
class IntermediateInstruction : public IntermediateOperation {
public:
    /**
     * \fn IntermediateInstruction::IntermediateInstruction(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name, const std::vector<std::string>& sources, const std::vector<std::string>& targets)
     * \brief Instantiates a new compile error with the given arguments.
     * \param lines The line interval the operation occupies.
     * \param labels The vector of labels assigned to the operation.
     * \param name The name of the operation.
     * \param sources The source operands of the instruction.
     * \param targets The target operands of the instruction.
     */
    IntermediateInstruction(const LineInterval& lines, const std::vector<std::string>& labels, const std::string& name, const std::vector<std::string>& sources, const std::vector<std::string>& targets)
        : IntermediateOperation(lines, labels, name), _sources(sources), _targets(targets) {}

    /**
     * \fn IntermediateInstruction::IntermediateInstruction(const IntermediateInstruction& other)
     * \brief Default copy constructor.
     * \param other The source IntermediateInstruction instance.
     */
    IntermediateInstruction(const IntermediateInstruction& other) = default;

    /**
     * \fn IntermediateInstruction::IntermediateInstruction(IntermediateInstruction&& other)
     * \brief Default move constructor.
     * \param other The source IntermediateInstruction instance.
     */
    IntermediateInstruction(IntermediateInstruction&& other) = default;

    /**
     * \fn IntermediateInstruction::operator =(const IntermediateInstruction& other)
     * \brief Default copy assignment operator.
     * \param other The source IntermediateInstruction instance.
     */
    IntermediateInstruction& operator =(const IntermediateInstruction& other) = default;

    /**
     * \fn IntermediateInstruction::operator =(IntermediateInstruction&& other)
     * \brief Default move assignment operator.
     * \param other The source IntermediateInstruction instance.
     */
    IntermediateInstruction& operator =(IntermediateInstruction&& other) = default;

    /**
     * \fn IntermediateInstruction::~IntermediateInstruction()
     * \brief Default destructor.
     */
    ~IntermediateInstruction() = default;

    /**
     * \fn IntermediateInstruction::execute(FinalRepresentation& finalRepresentator, const SymbolTable& table, CompileState& state)
     * \brief Converts this instruction into a syntax tree and inserts it into the final representation.
     * \param finalRepresentator The FinalRepresentation to insert into.
     * \param table The SymbolTable required for replacing the arguments.
     * \param state The CompileState logging all errors occuring.
     */
    virtual void execute(FinalRepresentation& finalRepresentator, const SymbolTable& table, CompileState& state);

    /**
     * \fn IntermediateInstruction::compileInstruction(const SymbolTable& table, CompileState& state)
     * \brief Converts this instruction into a syntax tree.
     * \param table The SymbolTable required for replacing the arguments.
     * \param state The CompileState logging all errors occuring.
     * \return The resulting syntax tree of the node.
     */
    std::unique_ptr<AbstractSyntaxTreeNode> compileInstruction(const SymbolTable& table, CompileState& state);

protected:

    /**
     * \fn IntermediateInstruction::determineMemoryPosition()
     * \brief Gets the position in program memory space.
     */
    virtual void determineMemoryPosition();

private:
    /**
     * \var IntermediateInstruction::_sources
     * \brief The internal source arguments.
     */
    std::vector<std::string> _sources;

    /**
     * \var IntermediateInstruction::_targets
     * \brief The internal target arguments.
     */
    std::vector<std::string> _targets;
};

#endif
