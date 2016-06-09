#include "registervec.hpp"

RegisterVec::RegisterVec():_data{std::vector<Register>()}{}

int RegisterVec::createRegister(const int width){
  _data.push_back(Register{width});
  return _nextAdress++;
}

MemoryValue RegisterVec::get(const RegisterID& id)const{
  return _data[id._adress].get(id._begin,id._end);
}
void RegisterVec::put(const RegisterID& id,const MemoryValue& value){
  _data[id._adress].put(id._begin,id._end,value);
}

MemoryValue RegisterVec::set(const RegisterID& id, const MemoryValue& value){
  return _data[id._adress].set(id._begin,id._end,value);
}
