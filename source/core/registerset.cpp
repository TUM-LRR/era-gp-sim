#include "registerset.hpp"

RegisterSet::RegisterSet(int bucketCount):
  _dict{std::unordered_map<std::string,RegisterID>(bucketCount)},
  _registers{RegisterVec()}{}

void RegisterSet::createRegister(const std::string& name,const  int width){
    //Do I need to force this copy here?              ||
    //and subsequently the same in the other methods? \/
  _dict.insert(std::pair<const std::string,RegisterID>(std::string(name),
                         RegisterID(_registers.createRegister(width),0,width)));
}
void RegisterSet::aliasRegister (const std::string& name,
                                 const std::string& parent,
                                 const int begin,
                                 const int end){
  RegisterID parentR=_dict.at(parent);
  _dict.insert(std::pair<std::string,RegisterID>(std::string(name),
        RegisterID(parentR._adress, parentR._begin+begin, parentR._begin+end)));
}
void RegisterSet::aliasRegister (const std::string& name,
                                 const std::string& parent,
                                 const int begin){
  RegisterID parentR=_dict.at(parent);
  _dict.insert(std::pair<std::string,RegisterID>(std::string(name),
              RegisterID(parentR._adress, parentR._begin+begin, parentR._end)));
}
MemoryValue RegisterSet::get(const std::string& name)const{
  return _registers.get(_dict.at(name));
}
void RegisterSet::put(const std::string& name,const  MemoryValue& value){
  _registers.put(_dict.at(name),value);
}
MemoryValue RegisterSet::set(const std::string& name,const  MemoryValue& value){
  return _registers.set(_dict.at(name),value);
}

