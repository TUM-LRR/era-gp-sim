#ifndef ERAGPSIM_CORE_REGISTERSET_H
#define ERAGPSIM_CORE_REGISTERSET_H

#include <string>

#include "memoryvalue.hpp"

class RegisterSet
{
public:
    //creates a RegisterSet
    RegisterSet()=default;
    //destructor
    ~RegisterSet()=default;

    //creates a register in this RegisterSet with name 'name' and width 'width'
    void createRegister(std::string&, int);
    //creates an alias for a register with the name 'name' to the bits ['begin';'end'[ of the register 'parent'
    void aliasRegister (std::string&,std::string&, int, int);
    //creates an alias for a register with the name 'name' to the bits ['begin';'parent'->end[ of the register 'parent'
    void aliasRegister (std::string&,std::string&,int=0);

    //returns the value of the register with the name 'name'
    MemoryValue get(std::string&)const;
    //sets the value of the register with the name 'name'
    void put(std::string&, MemoryValue&);
    //sets the value of the register with the name 'name' and returns its former value
    inline MemoryValue set(std::string&, MemoryValue&);
};

#endif // ERAGPSIM_CORE_REGISTERSET_H
