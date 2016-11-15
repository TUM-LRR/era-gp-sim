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

#include <string>
#include <vector>
#include <initializer_list>
#include <memory>
#include <QApplication>
#include <QString>

class Translateable {
    Q_DECLARE_TR_FUNCTIONS(Translateable)

    using TranslateablePtr = std::shared_ptr<Translateable>;
    public:

    explicit Translateable(const std::string& baseString) : _baseString(baseString) {}

    Translateable(const std::string& baseString, const std::initializer_list<TranslateablePtr>& operands) : _baseString(baseString), _operands(operands) {}

    Translateable(const std::string& baseString, const std::initializer_list<const std::string>& operands);

    Translateable(const std::string& baseString, const std::initializer_list<std::reference_wrapper<Translateable>>& operands);

    QString translate(const QApplication& translater) const;
private:
    std::string _baseString;
    std::vector<TranslateablePtr> _operands;
};

#endif // ERAGPSIM_COMMON_TRANSLATEABLE_HPP
