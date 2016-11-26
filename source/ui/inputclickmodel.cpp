#include <iostream>

#include "ui/inputclickmodel.hpp"
#include "core/conversions.hpp"


InputClickModel::InputClickModel(QQmlContext *context, MemoryAccess m): QObject(), context(context), memoryAccess(m),  xMouseClick(0), yMouseClick(0){
    context->setContextProperty("inputClickMod", this);
}

void InputClickModel::newClick(int x, int y){
    xMouseClick=x;
    yMouseClick=y;
    //Memory speichern als 2 Byte
    MemoryValue m1= conversions::convert(xMouseClick);
    MemoryValue m2= conversions::convert(yMouseClick);

    memoryAccess.setMemoryValueAt(start, m1, 8);
    memoryAccess.setMemoryValueAt(start+1, m2, 8);

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
