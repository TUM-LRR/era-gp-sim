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

#ifndef ERAGPSIM_ARCH_RISCV_FACTORY_MAP_HPP
#define ERAGPSIM_ARCH_RISCV_FACTORY_MAP_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "arch/common/abstract-syntax-tree-node.hpp"
#include "arch/common/instruction-information.hpp"
#include "arch/riscv/integer-instructions.hpp"

namespace riscv {

/**
 * A utility class to map instruction names to their factory
 * functions.
 *
 * The purpose of this class is to reduce the boilerplate for adding nodes  *
 * to the factory mapping in the instruction-nod-factory, as well as for
 * accessing factories to create new
 * nodes.
 */
class FactoryMap {
 public:
  using Factory = std::function<std::unique_ptr<AbstractSyntaxTreeNode>(
      const InstructionInformation &)>;
  using Node = std::unique_ptr<AbstractSyntaxTreeNode>;

  /**
   * An abstract base class for facades.
   */
  class AbstractFacade {
   public:
    /**
     * Constructs the facade for a factory map.
     *
     * \param factories The factory map to create the facade for.
     */
    explicit AbstractFacade(FactoryMap &factories);

    /** Pure virtual constructor to make this class abstract. */
    virtual ~AbstractFacade() = 0;

   protected:
    /** The underlying factory map for which this class is a facade. */
    FactoryMap &_factories;
  };

  /**
   * A facade that stores any number of template parameters in its type.
   *
   * This class is useful to store common template parameters with which more
   * than one instruction have to be instantiated, such as word types.
   *
   * \tparam Ts... The common template parameters.
   */
  template <typename... Ts>
  struct TypeFacade : public AbstractFacade {
    using super = AbstractFacade;
    using super::super;

    /**
     * Adds a factory to the factory map.
     *
     * \tparam InstructionType The instruction class to add.
     * \param instructionName The name of the instruction to add.
     * \param args... Additional (optional) variadic arguments.
     */
    template <template <typename...> class InstructionType, typename... Args>
    void add(const std::string &instructionName, Args &&... args) {
      // clang-format off
      _factories.add<InstructionType<Ts...>>(
        instructionName,
        std::forward<Args>(args)...);
      // clang-format on
    }
  };

  /**
   * A facade specifically for integer instructions.
   *
   * This facade facilitates adding integer
   * instructions by storing the common template parameters.
   *
   * \tparam SizeTupe The word size type for integer instructions.
   */
  template <typename SizeType>
  struct IntegerInstructionFacade : public AbstractFacade {
    using super = AbstractFacade;
    using super::super;

    /**
     * Adds a factory to the factory map.
     *
     * \tparam InstructionType The instruction class to add.
     * \param instructionName The name of the instruction to add.
     * \param hasImmediateVersion Whether there exists an immediate (-'i')
     *                            version of the instruction.
     */
    template <template <typename> class InstructionType>
    void
    add(const std::string &instructionName, bool hasImmediateVersion = true) {
      // clang-format off
      _factories.addIntegerInstruction<InstructionType<SizeType>>(
          instructionName,
          hasImmediateVersion
      );
      // clang-format on
    }
  };

  /**
   * Returns an integer facade for the factory map.
   *
   * \tparam SizeType The word size type for integer instructions.
   */
  template <typename SizeType>
  auto integerInstructionFacade() noexcept {
    return IntegerInstructionFacade<SizeType>(*this);
  }

  /**
   * Returns a generic type facade for the factory map.
   *
   * \tparam Ts... The common template types.
   */
  template <typename... Ts>
  auto typeFacade() noexcept {
    return TypeFacade<Ts...>(*this);
  }

  /**
   * Adds an integer instruction to the factory map.
   *
   * This method makes it easy to create both a register and immediate version
   * for a given instruction. If this feature is activated, an instruction
   * with the 'i' suffix will be created as well.
   *
   * \tparam The instruction class to associate the instruction (name) with.
   * \param name The name of the instruction.
   * \param hasImmediateVersion If true, also create an immediate version of
   *        the instruction.
   */
  template <typename InstructionType>
  void addIntegerInstruction(const std::string &instructionName,
                             bool hasImmediateVersion = true) {
    using Operands = typename InstructionType::Operands;

    // clang-format off
    _map.emplace(instructionName, [](const auto &information) {
      return std::make_unique<InstructionType>(
        information, Operands::REGISTERS);
    });

    if (hasImmediateVersion) {
      _map.emplace(instructionName + 'i', [](const auto &information) {
        return std::make_unique<InstructionType>(
            information,
            Operands::IMMEDIATES
        );
      });
      // clang-format on
    }
  }

  /**
   * Adds an instruction to the factory map.
   *
   * \tparam InstructionType The instruction class to add.
   * \param instructionName The name of the instruction.
   * \param args Any number of variadic arguments.
   */
  template <typename InstructionType, typename... Args>
  void add(const std::string &instructionName, Args &&... args) {
    // Cannot preserve value category (simply) unfortunately
    _map.emplace(instructionName, [args...](const auto &information) {
      return std::make_unique<InstructionType>(information, args...);
    });
  }

  /**
   * Returns the factory for a given instruction name.
   *
   * Error (existence) checking is performed inside this function.
   *
   * \param instructionName The name of the instruction to retrieve.
   *
   * \return The factory fucntion for the given instruction.
   */
  const Factory &get(const std::string &instructionName) const;

  /** \copydoc get() */
  const Factory &operator[](const std::string &instructionName) const;

  /**
   * Creates an instruction node for the given instruction name.
   *
   * Calling this function is the same as invoking the result of `get()`.
   *
   * \param instructionName The name of the instruction to retrieve.
   * \param information The information object for the instruction.
   *
   * \return An abstract syntax tree node for the instruction.
   */
  Node create(const std::string &instructionName,
              const InstructionInformation &information) const;

  /** \copydoc create() */
  Node operator()(const std::string &instructionName,
                  const InstructionInformation &information) const;

 private:
  using Map = std::unordered_map<std::string, Factory>;

  /** The actual underlying string to factory mapping. */
  std::unordered_map<std::string, Factory> _map;
};
}

#endif /* ERAGPSIM_ARCH_RISCV_FACTORY_MAP_HPP */
