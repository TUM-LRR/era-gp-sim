#include <iostream>

#include "ui/input-text-model.hpp"
#include "core/conversions.hpp"

InputTextModel::InputTextModel(QQmlContext* context, MemoryAccess m): QObject(), context(context), start(0), maxLength(20), memoryAccess(m)
{
    context->setContextProperty("inputtextMod", this);
}

void InputTextModel::newText(QString text){
    std::string stdText=text.toStdString();
    //std::cout<<stdText<<std::endl;


    for(unsigned i=0; i<stdText.length(); i++){
        char z=stdText.at(i);
        unsigned zahl=int(z);
        if(zahl<225){//else do nothing, was not a ascii sign
            //std::cout<<zahl<<std::endl;
            MemoryValue m=conversions::convert(zahl, 32);
            memoryAccess.putMemoryValueAt(start+i, m);
        }
    }
}

void InputTextModel::setStart(int start){
    if(memoryAccess.getMemorySize().get() >= start+maxLength){
        this->start=start;
    }
}

QString InputTextModel::getStart(){
    return QString::fromStdString(std::to_string(start));
}

void InputTextModel::setMaxLength(int maxL){
    this->maxLength=maxL;
}

int InputTextModel::getMaxLength(){
    return maxLength;
}
