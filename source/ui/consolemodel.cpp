#include "consolemodel.hpp"

ConsoleModel::ConsoleModel(QQmlContext* context): QObject(), context(context), text(""){
    context->setContextProperty("consoleModel",  this);
}

QString ConsoleModel::getText(){
    QString s=QString::fromStdString(text);
    std::cout<<text<<std::endl;
    std::cout<<s.toStdString()<<std::endl;
    return s;
}

void ConsoleModel::onDataChanged(){
    emit dataChanged();
}

void ConsoleModel::getData(){
    //Daten vom Core holen
    //text löschen
    //Werte in dezimalzahlen verwandeln, max bis 255
    //in chars umwandeln und an text anhängen

    text += char(70);
    text += char(80);
    text += char(95);

    emit textChanged();
}
