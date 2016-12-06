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
#ifndef ERAGPSIM_UI_TRANSLATEABLE_PREPARATION_HPP
#define ERAGPSIM_UI_TRANSLATEABLE_PREPARATION_HPP

#include <QString>
#include "common/translateable.hpp"

/**
 * Returns a translated QString that is ready for e.g. being displayed.
 * \param translateable The translateable holding the data
 * \return A QString that consists of all translated operands placed into the
 * translated base string
 */
QString translate(const Translateable& translateable);

/**
 * Returns a translated QString that is ready for e.g. being displayed.
 * \param transPtr A pointer to translateable holding the data
 * \return A QString that consists of all translated operands placed into the
 * translated base string
 */
QString translate(const Translateable::TranslateablePtr& transPtr);

#endif // ERAGPSIM_UI_TRANSLATEABLE_PREPARATION_HPP
