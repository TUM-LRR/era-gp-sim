/*
* C++ Assembler Interpreter
* Copyright (C) 2016 Chair of Computer Architecture
* at Technical University of Munich
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "ui/ui.hpp"
#include "ui/clipboard-adapter.hpp"

Ui::Ui(int& argc, char** argv) : _qmlApplication(argc, argv), _engine(){

}

int Ui::runUi() {
    qmlRegisterType<ClipboardAdapter>("ClipboardAdapter", 1, 0, "ClipboardAdapter");

    _engine.rootContext()->setContextProperty("ui", this);

    _engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return _qmlApplication.exec();
}

void Ui::addProject(QQuickItem* tabItem, QQmlComponent* projectComponent) {

    //TODO: create a new core-project
    //coreProject = core->createNewProject();

    //parent is tabItem, so it gets destroyed at the same time -> danger if context gets destroyed before creation of the projectItem
    QQmlContext* context = new QQmlContext(qmlContext(tabItem), tabItem);

    //the pointer is not needed anywhere, it is deleted by qml when tabItem is deleted
    new QProject(context, tabItem);

    //instantiate the qml project item with the prepared context
    QQuickItem* projectItem = qobject_cast<QQuickItem*>(projectComponent->create(context));

    //set the parent of projectItem, so its deletion is handled by qml
    projectItem->setParent(tabItem);

    //set visual parent of the projectItem
    projectItem->setParentItem(tabItem);
}
