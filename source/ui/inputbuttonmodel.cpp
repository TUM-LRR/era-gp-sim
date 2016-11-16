#include <iostream>

#include "ui/inputbuttonmodel.hpp"
#include "core/conversions.hpp"

InputButtonModel::InputButtonModel(QQmlContext* context, MemoryAccess m) : QObject(), context(context), memoryAccess(m){
    context->setContextProperty("inputButtonMod", this);
}

void InputButtonModel::buttonClicked(int id){
    //id an startadresse im Memory speichern
    MemoryValue m=conversions::convert(id);
    memoryAccess.setMemoryValueAt(start, m, 8);
    std::cout << id<<std::endl;
}
