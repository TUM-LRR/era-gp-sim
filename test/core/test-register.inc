#ifndef ERAGPSIM_CORE_TESTREGISTER_H
#define ERAGPSIM_CORE_TESTREGISTER_H

#include "gtest/gtest.h"

TEST(TestRegister,InOutCreate){
  RegisterSet instance{};
  for(int i=1;i<=32;++i){
    MemoryValue instance1{i};
    MemoryValue instance2{i};
    instance.createRegister("create"+i,i);
    for(int j=i%2;j<i;j+=2){
      instance1[j]=true;
    }
    instance.put("create"+i,instance1);
    instance2=instance.get("create"+i);
    //ASSERT_EQ(instance1.size(),instance2.size());
    ASSERT_EQ(instance1,instance2);
  }
}

TEST(TestRegister,InOutAlias){
  RegisterSet instance{};
  instance.createRegister("alias33",33);
  for(int i=32;i>0;--i){
    MemoryValue instance1{i};
    MemoryValue instance2{i};
    instance.aliasRegister("alias"+i,"alias"+(i+1),0,i);
    for(int j=i%2;j<i;j+=2){
      instance1[j]=true;
    }
    instance.put("alias"+i,instance1);
    instance2=instance.get("alias"+i);
    ASSERT_EQ(instance1,instance2);
  }
}

#endif // ERAGPSIM_CORE_TESTREGISTER_H
