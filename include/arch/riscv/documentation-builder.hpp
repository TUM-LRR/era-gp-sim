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
#ifndef ERAGPSIM_ARCH_RISCV_DOCUMENTATIONBUILDER_HPP
#define ERAGPSIM_ARCH_RISCV_DOCUMENTATIONBUILDER_HPP

#include <unordered_map>
#include <vector>
#include "common/translateable.hpp"
#include "common/utility.hpp"

/**
 * A documentation builder can be used to build a text containing the
 * documentation/help text for an instruction.
 * The builder can be filled with string description of certain components of
 * the help text in an arbitrary order.
 * When building, the content of each component is html formatted and assembled
 * into one Translateable text.
 */
class DocumentationBuilder {
 public:
    using StringList = Translateable::StringList;

  /**
 * A Key is the identifier for a certain component of the text.
 */
  enum class Key {
    /** The key for the instruction name*/
    INSTRUCTION,
    /** The key for a short syntax statement, e.g. "rd, rs, imm"*/
    SHORT_SYNTAX,
    /** The key for operand description */
    OPERAND_DESCRIPTION,
    /** The key for a short description of what the instruction does expressed
       in some kind of pseudo-code: e.g. "rd=rs + imm" */
    SHORT_DESCRIPTION,
    /** The key for the detailed description of what the instruction does. */
    DETAIL_DESCRIPTION
  };

  /**
   * Constructs a empty DocumentationBuilder
   */
  DocumentationBuilder();

  /**
   * Builds and assembles all components of this builder into a Translateable
   * text.
   * It is asserted that at least the instruction name (Key::INSTRUCTION) and
   * the short syntax statement (Key::S_SYNTAX)
   * are present
   * \return The Translateable, html formatted text
   */
  Translateable build();

  /**
   * Convenience function for adding a detail description component.
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param s The detailed description
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& detailDescription(const std::string& s);

  /**
   * Convenience function for adding a detail description component.
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param msg A Translateable wich contains the detail description
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& detailDescription(
      const Translateable::TranslateablePtr& msg);

  /**
   * Convenience function for adding a short description component.
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param s The detailed description
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& shortDescription(const std::string& s);

  /**
   * Convenience function for adding one operand description.
   * The operand descriptions will be stacked so that multiple operands are supported
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param name The name of the operand
   * \param description A detailed description of the operand
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& operandDescription(const std::string& name,
                                           const std::string& description);

  /**
   * Convenience function for adding the instruction mnemonic/name
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param s The name of the instruction
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& instruction(const std::string& s);

  /**
   * Convenience function for adding a short syntax description
   * Note: Any literal text that is meant to be translated must be wrapped with
   * one of
   * Qts translation macros prior to this call
   * \param operands A list of all operand names that this instruction takes (in the order of definition)
   * \return The current DocumentationBuilder for chaining convenience
   */
  DocumentationBuilder& shortSyntax(
      StringList operands);

 private:
  /** A list of colors in a html compliant format. These colors are used to mark each operand in a seperate color*/
  static const std::vector<std::string> _colors;

  /**
   * Puts the key-value pair into the builders internal map
   * \param key The components key
   * \param value The components value
   */
  void _add(const Key& key, const Translateable::TranslateablePtr& value);

  /**
   * Checks if the given key is available in the internal map
   * \param key
   * \return True, if there is a component already associated with this key, otherwise false
   */
  bool _hasKey(const Key& key) const;

  /**
   * Returns the next html compliant color definition from the static color list
   * \return
   */
  const std::string& _nextColor() const;

  /**
   * Returns the associated Translateable if the key exists in the internal map, otherwise a Translateable of an empty string
   * \param key The key of the key-value pair
   * \return
   */
  Translateable::TranslateablePtr _optional(const Key& key);

  /** A map to store all components until building to offer arbitrary filling order */
  std::unordered_map<Key, Translateable::TranslateablePtr, Utility::EnumHash<Key>> _components;

  /** A counter, counting the amount of operands */
  size_t _operandCount;
};

#endif  // ERAGPSIM_ARCH_RISCV_DOCUMENTATIONBUILDER_HPP
