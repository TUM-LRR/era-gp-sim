#include <iostream>

#include "inputtextmodel.hpp"

InputTextModel::InputTextModel(QQmlContext* context): QObject(), context(context)
{
    context->setContextProperty("inputtextMod", this);
}

void InputTextModel::newText(QString text){
    std::string stdText=text.toStdString();
    std::cout<<stdText<<std::endl;


    for(unsigned i=0; i<stdText.length(); i++){
        char z=stdText.at(i);
        unsigned zahl=int(z);
        if(zahl<225){//else do nothing, was not a ascii sign
            std::cout<<zahl<<std::endl;
        }
    }
}
