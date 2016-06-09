#ifndef ERAGPSIM_CORE_REGISTER_H
#define ERAGPSIM_CORE_REGISTER_H

#include <vector>

#include "memoryvalue.hpp"

//A simulated register
class Register{
public:
  //creates a register of width 'width'
  Register(int width);
  //destructor
  ~Register()=default;

  //returns the value of this register between the indices [begin;end[
  MemoryValue get(const int begin, const int end)const;
  //changes the value of this register between the indices [begin;end[
  void put(const int begin, const int end, const MemoryValue& value);
  //changes the value of this register between the indices [begin;end[ and
  //returns the previous value
  MemoryValue set(const int begin, const int end, const MemoryValue& value);

private:
  std::vector<bool> _data;//for now until we could decide on a better solution
};

#endif // ERAGPSIM_CORE_REGISTER_H
