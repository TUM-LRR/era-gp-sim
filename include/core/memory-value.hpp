#ifndef ERAGPSIM_CORE_MEMORYVALUE_H
#define ERAGPSIM_CORE_MEMORYVALUE_H

#include <iostream>
#include <vector>

// Set of bits for in/out-putting to/from memory/registers
class MemoryValue {
public:
  // Creates a MemoryValue of width 'width' (default=8)
  MemoryValue(int width = 8);
  // Creates a MemoryValue with a copy of the std::vector<bool> 'data'
  MemoryValue(const std::vector<bool> data);
  // copy constructor
  MemoryValue(const MemoryValue &) = default;
  // copy operator
  MemoryValue &operator=(const MemoryValue &) = default;
  // move constructor
  MemoryValue(MemoryValue &&) = default;
  // move operator
  MemoryValue &operator=(MemoryValue &&) = default;
  // destructor
  ~MemoryValue() = default;

  // Returns a reference to the element at specified location 'index', with
  // bounds checking.
  // If 'index' is not within the range of the container, an exception of type
  // std::out_of_range is thrown.
  std::vector<bool>::reference at(const int index);
  // Returns a reference to the element at specified location 'index', with
  // bounds checking.
  // If 'index' is not within the range of the container, an exception of type
  // std::out_of_range is thrown.
  std::vector<bool>::const_reference at(const int index) const;
  // Returns a reference to the element at specified location 'index'. No
  // bounds checking is performed.
  std::vector<bool>::reference operator[](const int index);
  // Returns a reference to the element at specified location 'index'. No
  // bounds checking is performed.
  std::vector<bool>::const_reference operator[](const int index) const;
  // Toggles each bool in the vector (replaces with its opposite value).
  void flip();
  int size() const;

  // returns true iff second._data==this->_data
  bool operator==(const MemoryValue &) const;
  // puts a binary representation of 'this Memory Value'value' into the ostream
  friend std::ostream &operator<<(std::ostream &stream,
                                  const MemoryValue &value);

private:
  std::vector<bool> _data;
};

#endif // ERAGPSIM_CORE_MEMORYVALUE_H
