#ifndef SNAPSHOTMODEL_H
#define SNAPSHOTMODEL_H

#include <string>
#include <QObject>
#include <QStringList>
#include <QAbstractListModel>

class snapshotModel : public QAbstractListModel{
    Q_OBJECT
public:
    enum Roles {
            NameRole
     };

    snapshotModel(QObject *parent = 0);

    bool removeRow(int row, const QModelIndex &parent = QModelIndex());

    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

    Q_INVOKABLE void deleteClicked(QByteArray i);

    Q_INVOKABLE void loadClicked(QByteArray i);

    void add(std::string s);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;

    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;


protected:
    QHash<int, QByteArray> roleNames() const;

private:
    QStringList list;
};

#endif // SNAPSHOTMODEL_H
