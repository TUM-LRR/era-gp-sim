#ifndef ERAGPSIM_CORE_REGP_H
#define ERAGPSIM_CORE_REGP_H

//The information to uniquely identify a register
struct RegisterID{
  int _adress;
  int _begin;
  int _end;
  RegisterID()=default;
  RegisterID(int adress, int begin, int end):
      _adress{adress},
      _begin{begin},
      _end{end}{}
  ~RegisterID()=default;
};

#endif // ERAGPSIM_CORE_REGP_H
