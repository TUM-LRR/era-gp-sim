#include "registerset.hpp"

void RegisterSet::createRegister(std::string& name, int width){}
void RegisterSet::aliasRegister (std::string& name,std::string& parent, int begin, int end){}
void RegisterSet::aliasRegister (std::string& name,std::string& parent,int begin){}

MemoryValue RegisterSet::get(std::string& name)const{
    return MemoryValue();
}

void RegisterSet::put(std::string&, MemoryValue&){}
MemoryValue RegisterSet::set(std::string& name, MemoryValue& value){
    MemoryValue previousValue=get(name);
    put(name,value);
    return previousValue;
}

