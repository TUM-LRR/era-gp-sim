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

#ifndef ERAGPSIM_COMMON_CONTAINER_ADAPTER_HPP
#define ERAGPSIM_COMMON_CONTAINER_ADAPTER_HPP

#include <memory>

/**
 * Note: Macros are bad, bad, bad! They are not real code, they make debugging
 * horrible because you cannot directly see the generated code, they are an
 * old C construct etc. However, in this case, I must concede that it makes
 * derivation from ContainerAdapter more legible. Who wants to see 20 using
 * statements? Also, those 20 using statements in 5-10 subclasses are not
 * maintainable, at all.
 *
 * Use it like this (for example):
 *
 * `using CONTAINER_ADAPTER_MEMBERS`
 */
#define CONTAINER_ADAPTER_MEMBERS \
  super::_container;              \
  using super::begin;             \
  using super::cbegin;            \
  using super::end;               \
  using super::cend;              \
  using super::clear;             \
  using super::size;              \
  using super::isEmpty;           \
  using super::getUnderlying

/**
 * A container-adapter base template.
 *
 * Defines some methods common to all concrete adapter classes. That is, if you
 * want to adapt some standard container, you can inherit your adapter from this
 * class and it will take care of some boilerplate methods.
 */
template <typename UnderlyingTemplate>
class ContainerAdapter {
 public:
  using Underlying      = UnderlyingTemplate;
  using Iterator        = typename Underlying::iterator;
  using ConstIterator   = typename Underlying::const_iterator;
  using ValueType       = typename Underlying::value_type;
  using InitializerList = std::initializer_list<ValueType>;

  /**
   * Constructs a new container adapter from a range of elements.
   *
   * \tparam Range a range-like type.
   *
   * \param range A range of elements to initialize the container adapter with.
   */
  template <typename Range>
  explicit ContainerAdapter(const Range& range)
  : _container(std::begin(range), std::end(range)) {
  }

  /**
   * Constructs a new container adapter from the list of elements.
   *
   * \param instructions A list of elements to initialize the container adapter
   *                     with.
   */
  ContainerAdapter(InitializerList list = InitializerList())
  : _container(list) {
  }

  virtual ~ContainerAdapter() = default;

  /**
   * Returns an `Iterator` to the beginning of the container.
   */
  virtual Iterator begin() noexcept {
    return _container.begin();
  }

  /**
   * Returns a `ConstIterator` to the beginning of the container.
   */
  virtual ConstIterator begin() const noexcept {
    return _container.begin();
  }

  /**
   * Returns a `ConstIterator` to the beginning of the container.
   *
   * This method allows you to get a `ConstIterator` even if your object isn't
   * `const`.
   */
  virtual ConstIterator cbegin() const {
    return _container.cbegin();
  }

  /**
   * Returns an `Iterator` to the beginning of the container.
   */
  virtual Iterator end() noexcept {
    return _container.end();
  }

  /**
   * Returns a `ConstIterator` to the end of the container.
   */
  virtual ConstIterator end() const noexcept {
    return _container.end();
  }

  /**
   * Returns a `ConstIterator` to the end of the container.
   *
   * This method allows you to get a `ConstIterator` even if your object isn't
   * `const`.
   */
  virtual ConstIterator cend() const noexcept {
    return _container.cend();
  }

  /**
   * Removes all elements in the container.
   */
  virtual void clear() {
    _container.clear();
  }

  /**
   * Return the number of elements stored in the container.
   *
   * \return The number of elements stored in the container.
   */
  virtual size_t size() const noexcept {
    return _container.size();
  }

  /**
   * Returns whether or not the container is empty.
   *
   * \return True if there are no elements stored in the container at all, else
   *         false.
   */
  virtual bool isEmpty() const noexcept {
    return _container.empty();
  }

  /**
  * Returns the underlying container.
  */
  virtual const Underlying& getUnderlying() const noexcept {
    return _container;
  }

 protected:
  /** The underlying container. */
  Underlying _container;
};

#endif /* ERAGPSIM_COMMON_CONTAINER_ADAPTER_HPP */
