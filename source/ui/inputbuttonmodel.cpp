#include <iostream>

#include "inputbuttonmodel.hpp"

InputButtonModel::InputButtonModel(QQmlContext* context) : QObject(), context(context){
    context->setContextProperty("inputButtonMod", this);
}

void InputButtonModel::buttonClicked(int id){
    //id an startadresse im Memory speichern
    std::cout << id<<std::endl;
}
