#include "snapshotmodel.hpp"
#include <QStringList>
#include <string>
#include <iostream>
#include <QQmlApplicationEngine>

QStringList list;


snapshotModel::snapshotModel(QObject *parent): QAbstractListModel(parent){

}


void snapshotModel::add(std::string s){
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    QString st=QString::fromUtf8(s.c_str());
    list.append(st);
    endInsertRows();
}

bool snapshotModel::removeRow(int row, const QModelIndex &parent){
    return removeRows(row, 1, parent);
}

bool snapshotModel::removeRows(int row, int count, const QModelIndex &parent){
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), row, row+count-1);
    bool erfolg=true;
    for(int i=0; i<count; i++){
        list.removeAt(row);
    }
    endRemoveRows();
    return erfolg;
}


void snapshotModel::deleteClicked(QByteArray i){
    std::string st=i.toStdString();
    int index=list.indexOf(QString::fromUtf8(st.c_str()));
    removeRow(index);
    std::cout<<"row "+st+" "+std::to_string(index)<<std::endl;
    QString liststring=list.join(", ");
    std::string stri=liststring.toStdString();
    std::cout<<stri<<std::endl;
}

void snapshotModel::loadClicked(QByteArray i){
    std::string st=i.toStdString();
    std::cout<<"row "+st+" load"<<std::endl;
}

int snapshotModel::rowCount(const QModelIndex &parent)const{
    Q_UNUSED(parent);
    return list.count();
}

QVariant snapshotModel::data(const QModelIndex &index, int role)const{
    if (index.row() < 0 || index.row() >= list.count()){
            return QVariant();
    }

    QString s =list.at(index.row());
    if (role == NameRole){
            return s;
    }
    return QVariant();

}

QHash<int, QByteArray> snapshotModel::roleNames()const{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    return roles;
}



