#ifndef ERAGPSIM_CORE_REGISTERVEC_H
#define ERAGPSIM_CORE_REGISTERVEC_H

#include <vector>

#include "registerid.hpp"
#include "memoryvalue.hpp"
#include "register.hpp"

//a vector of registers that are not accessible by using std::strings
class RegisterVec
{
public:
  //creates an empty RegisterVec
  RegisterVec();
  //destructor
  ~RegisterVec()=default;

  //creates a register of width 'width'
  int createRegister(const int width);

  //returns the value of the register identified by 'id'
  MemoryValue get(const RegisterID& id)const;
  //changes the value of the register identified by 'id' to 'value'
  void put(const RegisterID& id,const MemoryValue& value);
  //changes the value of the register identified by 'id' to 'value' and returns
  //the previous value
  MemoryValue set(const RegisterID& id, const MemoryValue& value);
private:
  std::vector<Register> _data;
  int _nextAdress=0;
};

#endif // ERAGPSIM_CORE_REGISTERVEC_H
