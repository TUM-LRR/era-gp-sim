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

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

/**
 * One Note about the translating concept:
 * We use the build-in Qt translating features, which roughly work like this:
 * A string literal in the source code is marked for translation using one of
 * the QT-supplied macros (like QT_TRANSLATE_NOOP).
 * These macros just serve as markers for an external Qt-Application (named
 * QTLinguist) which scans the source code, gathers all literals
 * and creates a table with collected literal associated with its translated
 * version. The table can then be saved to an external file that will
 * be loaded at runtime.
 * The string literal in the source code will NOT be translated just by the
 * marker macros, the actual translation (if a table was loaded) is done via
 * a call of QObject::tr().
 * So far, so good.
 * When writing translateable messages, care must be taken that the string to be
 * translated (e.g. the message displayed to the user) is a
 * string LITERAL (of type const char* or char[n]). Any other representation of
 * a string, like std::string or QString, is only available at runtime,
 * therefore QTLinguist cannot determine its value by looking at the source
 * code.
 * This is the reason why no constructor with std::string or QString exists,
 * because in most of the cases where one would like to call a std::string
 * constructor,
 * the call will look like this: Translateable("error in line "+_line+". Severe
 * error!").
 * As mentioned above, the Translateable should be created with a runtime
 * parameter: Translateable(QT_TR_NOOP("context info", "error in line %1. Severe
 * error!"), _line).
 * For those rare cases, where a std::string is needed as base string, a special
 * constructor is provided with a special key "NO_TR_POSSIBLE".
 *
 * A Translateable is a piece of text that can be translated and retrieved.
 * A Translateable consists of a frame string (base string) and some operands.
 * These operands themselves are also Translateables, so in the translation
 * process the operands are
 * translated as well and then placed into the frame string.
 */
class Translateable {
 public:
  using TranslateablePtr = std::shared_ptr<Translateable>;

  struct NO_TR_POSSIBLE {};

  /**
   * Constructs a Translateable with the given base string and empty arguments
   * \param baseString The base/frame string of this Translateable
   */
  explicit Translateable(const char* baseString);

  Translateable(const std::string& baseString, const NO_TR_POSSIBLE& key);

  Translateable();

  Translateable(Translateable& copy) = default;

  Translateable(const Translateable& copy) = default;

  Translateable(const char* base, const std::initializer_list<std::string> arguments);

  template <typename... Args>
  Translateable(const char* base, Args&&... arguments)
      : _baseString(base),
        _operands{createShared(std::forward<Args>(arguments))...} {}

  /**
   * Returns a reference to the base string of this Translateable
   */
  std::string& getModifiableBaseString();

  /**
   * \return A reference to the base string of this Translateable
   */
  const std::string& getBaseString() const;

  /**
   * \return All (runtime) operands of this Translateable
   */
  const std::vector<TranslateablePtr>& getOperands() const;

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

  template <typename... Args>
  void addOperand(const char* base, Args&&... args) {
    addOperand(std::make_shared<Translateable>(base, args...));
  }

 private:
  Translateable(const std::string& baseString);

  template <typename T>
  static TranslateablePtr createShared(const T& arg) {
      //not using std::make_shared as the function cannot access
      //a private/protected constructor...
      //solutions, like a wrapper or derived type accessing the constructor
      //would make the code less understandable
    return std::shared_ptr<Translateable>(new Translateable{arg});
  }

  TranslateablePtr createShared(const TranslateablePtr& arg);

  std::string _baseString;
  std::vector<TranslateablePtr> _operands;
};

#endif  // ERAGPSIM_COMMON_TRANSLATEABLE_HPP
