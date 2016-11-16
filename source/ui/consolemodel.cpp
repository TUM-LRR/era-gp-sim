#include "ui/consolemodel.hpp"



ConsoleModel::ConsoleModel(QQmlContext* context, MemoryAccess memoryAccess): QObject(), context(context), _memoryAccess(memoryAccess), text(""){
    context->setContextProperty("consoleModel",  this);
}

QString ConsoleModel::getText(){
    QString s=QString::fromStdString(text);
    std::cout<<text<<std::endl;
    std::cout<<s.toStdString()<<std::endl;
    return s;
}

void ConsoleModel::onDataChanged(std::size_t address, std::size_t length){
    if(address>(start+maximumLength)){
        return;
    }
    else if((address+length)<start){
        return;
    }
    else{
        emit dataChanged();
    }
}

void ConsoleModel::getData(){
    text="";
    //Variabe start: start der Eingabe
    //Variable maxLength: Maximale Länge der Eingabe
    if(mode ==0/*ArrayBased*/){
        //Daten vom Core holen
        //text löschen
        //Werte in dezimalzahlen verwandeln, max bis 255 bis nullbyte erreicht
        //in chars umwandeln und an text anhängen
    }
    else/*pipeline*/{
        //Daten in angegebener Speicherzelle vom Core holen
        //Daten an text anhängen
    }

    //Beispiel
    text += char(70);
    text += char(80);
    text += char(10);
    text += char(95);

    emit textChanged();
}

void ConsoleModel::modeChanged(int newMode){
    mode=newMode;
    getData();
}

void ConsoleModel::clear(){
    if(mode!=0){
        text="";
        emit textChanged();
    }
}

void ConsoleModel::changeStart(int newStart){
    start=newStart;
}

