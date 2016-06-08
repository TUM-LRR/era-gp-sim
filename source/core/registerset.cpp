#include "registerset.hpp"

void RegisterSet::createRegister(const std::string& name,const  int width){}
void RegisterSet::aliasRegister (const std::string& name,
                                 const std::string& parent,
                                 const int begin,
                                 const int end){}
void RegisterSet::aliasRegister (const std::string& name,
                                 const std::string& parent,
                                 const int begin){}

MemoryValue RegisterSet::get(const std::string& name)const{
  return MemoryValue();
}

void RegisterSet::put(const std::string&,const  MemoryValue&){}
MemoryValue RegisterSet::set(const std::string& name,const  MemoryValue& value){
  MemoryValue previousValue=get(name);
  put(name,value);
  return previousValue;
}

