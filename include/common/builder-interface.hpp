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

#ifndef ERAGPSIM_COMMON_BUILDER_INTERFACE_HPP
#define ERAGPSIM_COMMON_BUILDER_INTERFACE_HPP

/**
 * Abstract builder interface.
 *
 * All classes following the builder pattern should inherit from this class to
 * ensure that they implement the `isValid()` function, as all builders should
 * make sure that whatever they are building is valid, at some point (e.g. when
 * inside `build()`).
 *
 * While this cannot be enforced via an abstract interface
 * (due the fact that concrete builders always return their own concrete
 * instances, which the abstract class obviously does not know of), it is
 * strongly recommended that each concrete builder implement a `validate()`
 * builder method (i.e. that returns the instance), such that this method can be
 * called at the end of a building process. The `validate()` method, in the
 * simplest case, would just `assert(isValid())`. This could also just be
 * integrated into the `build()` method, if one exists.
 */
class BuilderInterface {
 public:
  /** Destructor. */
  virtual ~BuilderInterface() = default;

  /**
   * Returns whether or not the builder's state is valid.
   *
   * When `isValid()` is true, that means it is safe to call the `build()`
   * method or access members of the instance (if it is an "implicit" builder).
   *
   * \return True if the builder's state is valid, else false.
  */
  virtual bool isValid() const noexcept = 0;
};

#endif /* ERAGPSIM_COMMON_BUILDER_INTERFACE_HPP */
