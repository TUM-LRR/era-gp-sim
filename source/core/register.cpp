#include "register.hpp"

Register::Register(int width):_data{std::vector<bool>(width,false)}{}

MemoryValue Register::get(const int begin, const int end)const{
  MemoryValue res{end-begin};
  for(int i=0;i<end-begin;++i)res[i]=_data[i+begin];
  return res;
}

void Register::put(const int begin, const int end, const MemoryValue& value){
  for(int i=end-begin;i-->0;)_data[i+begin]=value[i];
}

MemoryValue Register::set(const int begin,
                          const int end,
                          const MemoryValue& value){
  MemoryValue previousValue=get(begin,end);
  put(begin,end,value);
  return previousValue;
}
