#ifndef ERAGPSIM_CORE_REGISTERSET_H
#define ERAGPSIM_CORE_REGISTERSET_H

#include <string>
#include <unordered_map>

#include "memoryvalue.hpp"
#include "registerid.hpp"
#include "registervec.hpp"

//a set of Registers and a map registername->register
class RegisterSet{
public:
  //creates a RegisterSet
  RegisterSet(int bucketCount=10);
  //destructor
  ~RegisterSet()=default;

  //creates a register in this RegisterSet with name 'name' and width 'width'
  void createRegister(const std::string& name, const int width);
  //creates an alias for a register with the name 'name' to the bits
  //['begin';'end'[ of the register 'parent'
  void aliasRegister (const std::string& name,
                      const std::string& parent,
                      const int begin,
                      const int end);
  //creates an alias for a register with the name 'name' to the bits
  //['begin';'parent'->end[ of the register 'parent'
  void aliasRegister (const std::string& name,
                      const std::string& parent,
                      const int begin=0);

  //returns the value of the register with the name 'name'
  MemoryValue get(const std::string& name)const;
  //sets the value of the register with the name 'name'
  void put(const std::string& name,const  MemoryValue& value);
  //sets the value of the register with the name 'name' and returns its former
  //value
  MemoryValue set(const std::string& name,const  MemoryValue& value);
private:
  std::unordered_map<std::string, RegisterID> _dict;
  RegisterVec _registers;
};

#endif // ERAGPSIM_CORE_REGISTERSET_H
