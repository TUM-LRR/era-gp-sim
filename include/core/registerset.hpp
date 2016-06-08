#ifndef ERAGPSIM_CORE_REGISTERSET_H
#define ERAGPSIM_CORE_REGISTERSET_H

#include <string>

#include "memoryvalue.hpp"

class RegisterSet{
public:
  //creates a RegisterSet
  RegisterSet()=default;
  //destructor
  ~RegisterSet()=default;

  //creates a register in this RegisterSet with name 'name' and width 'width'
  void createRegister(const std::string&, const int);
  //creates an alias for a register with the name 'name' to the bits
  //['begin';'end'[ of the register 'parent'
  void aliasRegister (const std::string&,const std::string&, const int,
                      const int);
  //creates an alias for a register with the name 'name' to the bits
  //['begin';'parent'->end[ of the register 'parent'
  void aliasRegister (const std::string&,const std::string&,const int=0);

  //returns the value of the register with the name 'name'
  MemoryValue get(const std::string&)const;
  //sets the value of the register with the name 'name'
  void put(const std::string&,const  MemoryValue&);
  //sets the value of the register with the name 'name' and returns its former
  //value
  inline MemoryValue set(const std::string&,const  MemoryValue&);
};

#endif // ERAGPSIM_CORE_REGISTERSET_H
