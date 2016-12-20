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

#include <QObject>

#include "ui/translateable-processing.hpp"
#include "common/assert.hpp"

QString translate(const Translateable &translateable) {
    // translate the base string (still filled with the %s)
    QString translatedTotal = QObject::tr(translateable.getBaseString().c_str());
    for (const auto &op : translateable.getOperands()) {
      // translate recursively all operands and insert them into the base string
      translatedTotal = translatedTotal.arg(translate(op));
    }
    return translatedTotal;
}

QString translate(const Translateable::TranslateablePtr &transPtr) {
    assert::that(transPtr);
    return translate(*transPtr);
}
