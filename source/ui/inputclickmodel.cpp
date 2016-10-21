#include <iostream>

#include "inputclickmodel.hpp"

InputClickModel::InputClickModel(QQmlContext *context): QObject(), context(context), xMouseClick(0), yMouseClick(0){
    context->setContextProperty("inputClickMod", this);
}

void InputClickModel::newClick(int x, int y){
    xMouseClick=x;
    yMouseClick=y;
    //Memory speichern als 2 Byte
    std::cout << "X: "<<xMouseClick<<" Y: "<<yMouseClick<<std::endl;
}
