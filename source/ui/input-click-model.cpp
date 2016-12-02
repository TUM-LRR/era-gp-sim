#include <iostream>

#include "ui/input-click-model.hpp"
#include "core/conversions.hpp"


InputClickModel::InputClickModel(QQmlContext *context, MemoryAccess m): QObject(), context(context), memoryAccess(m), start(0){
    context->setContextProperty("inputClickMod", this);
}

void InputClickModel::newClick(int x, int y){
    //save in Memory as 2 bytes
    MemoryValue m1= conversions::convert(x, 32);
    MemoryValue m2= conversions::convert(y, 32);

    memoryAccess.putMemoryValueAt(start, m1);
    memoryAccess.putMemoryValueAt(start+1, m2);

   // std::cout << "X: "<<xMouseClick<<" Y: "<<yMouseClick<<std::endl;
}

void InputClickModel::setStart(int start){
    if(memoryAccess.getMemorySize().get() > start+2){
        this->start=start;
    }
}

QString InputClickModel::getStart(){
    return QString::fromStdString(std::to_string(start));
}
