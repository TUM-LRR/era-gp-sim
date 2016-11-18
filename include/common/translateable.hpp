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
#ifndef ERAGPSIM_COMMON_TRANSLATEABLE_HPP
#define ERAGPSIM_COMMON_TRANSLATEABLE_HPP

#include <QApplication>
#include <QString>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

/**
 * A Translateable is a piece of text that can be translated and retrieved.
 * A Translateable consists of a frame string (base string) and some operands.
 * These operands themselves are also Translateables, so in the translation
 * process the operands are
 * translated as well and then placed into the frame string.
 */
class Translateable {
  Q_DECLARE_TR_FUNCTIONS(
      Translateable)  // needed for declaring the use of QObject::tr()

 public:
  using TranslateablePtr = std::shared_ptr<Translateable>;

  /**
   * Constructs a Translateable with the given base string and empty arguments
   * \param baseString The base/frame string of this Translateable
   */
  explicit Translateable(const std::string& baseString)
      : _baseString(baseString) {}

  /**
   * Constructs a Translateable with the given base string and the given list of
   * Translateables as operands
   * \param baseString The base/frame string of this Translateable
   * \param operands List of pointers to other Translateables to act as operands
   */
  Translateable(const std::string& baseString,
                const std::initializer_list<TranslateablePtr>& operands)
      : _baseString(baseString), _operands(operands) {}

  /**
   * Constructs a Translateable with the given base string and the given list of
   * strings as operands
   * \param baseString The base/frame string of this Translateable
   * \param operands List of strings that will be converted to a Translateable
   * via the one-argument string constructor
   */
  Translateable(const std::string& baseString,
                const std::initializer_list<const std::string>& operands);

  /**
   * Constructs a Translateable with the given base string and the given list of
   * Translateables as operands.
   * Note: At least gcc sometimes seems to have a hard time identifying this
   * constructor when applying template type deduction;
   * Other possibilities are:
   * - Use the constructor with smart pointers to Translateable
   * - Use the one-argument string constructor and add other operands via
   * Translateable::addOperand
   * \param baseString The base/frame string of this Translateable
   * \param operands List of references to existing Translateables.
   */
  Translateable(const std::string& baseString,
                const std::initializer_list<
                    std::reference_wrapper<Translateable>>& operands);

  /**
   * Returns a translated QString that is ready for e.g. being displayed.
   * \param translater A Qt instance capable of providing the translation
   * function
   * \return A QString that consists of all translated operands placed into the
   * translated base string
   */
  QString translate(const QApplication& translater) const;

  /**
   * Returns a reference to the base string of this Translateable
   */
  std::string& getModifiableBaseString();

  /**
   * Adds the given operand to the operands of this Translateable
   * \param op A pointer to the existing operand
   */
  void addOperand(const TranslateablePtr& op);

  /**
   * Adds a new created operand with the given base string to the list of
   * operands of this Translateable
   * \param op A string that is the base string of the new operand
   */
  void addOperand(const std::string& op);

 private:
  std::string _baseString;
  std::vector<TranslateablePtr> _operands;
};

#endif  // ERAGPSIM_COMMON_TRANSLATEABLE_HPP
