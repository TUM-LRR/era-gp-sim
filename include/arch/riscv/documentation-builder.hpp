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

#include <vector>
#include <unordered_map>
#include "common/translateable.hpp"

class DocumentationBuilder {
 public:
  enum class Key { INSTRUCTION, S_SYNTAX, OPERAND_DESC, S_DESC, D_DESC };

  DocumentationBuilder();

  Translateable build();

  DocumentationBuilder& detailDescription(const std::string& s);

  DocumentationBuilder& detailDescription(const Translateable::TranslateablePtr& msg);

  DocumentationBuilder& shortDescription(const std::string& s);

  DocumentationBuilder& operandDescription(const std::string &name,
                                           const std::string& description);

  DocumentationBuilder& instruction(const std::string& s);

  DocumentationBuilder& shortSyntax(
      std::initializer_list<const std::string> operands);

 private:
  struct KeyHash {
    size_t operator()(const Key& k) const { return static_cast<size_t>(k); }
  };

  void _add(const Key& key, const Translateable::TranslateablePtr& value) {
    _components[key] = value;
  }

  bool _hasKey(const Key& key) const { return _components.count(key); }

  const std::string& _nextColor() const;

  Translateable::TranslateablePtr _optional(const Key& key);

  std::unordered_map<Key, Translateable::TranslateablePtr, KeyHash> _components;

  static const std::vector<std::string> _colors;

  size_t _operandCount;
};

#endif  // ERAGPSIM_ARCH_RISCV_DOCUMENTATIONBUILDER_HPP
