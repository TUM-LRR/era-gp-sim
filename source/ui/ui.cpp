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

#include "arch/common/architecture-formula.hpp"
#include "common/assert.hpp"
#include "common/utility.hpp"


Ui::Ui(int& argc, char** argv)
: _architectureMap(), _qmlApplication(argc, argv), _engine(), _projects() {
  _loadArchitectures();
}

int Ui::runUi() {
  qRegisterMetaType<std::size_t>("std::size_t");
  _engine.rootContext()->setContextProperty("ui", this);

  _engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  return _qmlApplication.exec();
}

void Ui::addProject(QQuickItem* tabItem,
                    QQmlComponent* projectComponent,
                    const QVariant& memorySizeQVariant,
                    const QString& architecture,
                    const QString& optionName,
                    const QString& parser) {
  // create ArchitectureFormula
  ArchitectureFormula architectureFormula(architecture.toStdString());

  // add all extensions which are defined for this option
  for (const auto& qstring : _getOptionFormula(architecture, optionName)) {
    architectureFormula.addExtension(qstring.toStdString());
  }
  // get the memory size from the qvariant object.
  std::size_t memorySize = memorySizeQVariant.value<std::size_t>();

  // parent is tabItem, so it gets destroyed at the same time
  QQmlContext* context = new QQmlContext(qmlContext(tabItem), tabItem);

  // the pointer is not needed anywhere, the object is deleted by qml when
  // tabItem is deleted
  _projects.push_back(new GuiProject(
      context, architectureFormula, memorySize, parser.toStdString(), tabItem));

  // instantiate the qml project item with the prepared context
  QQuickItem* projectItem =
      qobject_cast<QQuickItem*>(projectComponent->create(context));

  // set the parent of projectItem, so its deletion is handled by qml
  projectItem->setParent(tabItem);

  // set visual parent of the projectItem
  projectItem->setParentItem(tabItem);
}

QStringList Ui::getArchitectures() const {
  return _architectureMap.keys();
}

QStringList Ui::getOptionNames(QString architectureName) const {
  auto formulaMap =
      std::get<0>(_architectureMap.find(architectureName).value());
  return formulaMap.keys();
}

QStringList
Ui::_getOptionFormula(QString architectureName, QString optionName) const {
  auto formulaMap =
      std::get<0>(_architectureMap.find(architectureName).value());
  return formulaMap.find(optionName).value();
}

QStringList Ui::getParsers(QString architectureName) const {
  return std::get<1>(_architectureMap.find(architectureName).value());
}

void Ui::_loadArchitectures() {
  assert::that(_architectureMap.empty());
  std::string path = Utility::joinToRoot("isa", "isa-list.json");
  Ui::Json data = Ui::Json::parse(Utility::loadFromFile(path));
  assert::that(data.count("architectures"));
  assert::that(!data["architectures"].empty());

  for (auto& architecture : data["architectures"]) {
    assert::that(architecture.count("name"));
    assert::that(architecture.count("options"));
    assert::that(!architecture["options"].empty());
    assert::that(architecture.count("parsers"));
    assert::that(!architecture["parsers"].empty());

    QMap<QString, QStringList> optionNameFormulaMap;
    QStringList parserList;
    for (const auto& option : architecture["options"]) {
      assert::that(option.count("name"));
      assert::that(option.count("formula"));
      assert::that(!option["formula"].empty());
      QStringList formula;
      for (const auto& extension : option["formula"]) {
        formula.push_back(QString::fromStdString(extension));
      }
      optionNameFormulaMap.insert(QString::fromStdString(option["name"]),
                                  formula);
    }
    for (const auto& parser : architecture["parsers"]) {
      parserList.push_back(QString::fromStdString(parser));
    }
    _architectureMap.insert(QString::fromStdString(architecture["name"]),
                            std::make_tuple(optionNameFormulaMap, parserList));
  }
}

void Ui::removeProject(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects.erase(_projects.begin() + index);
}

void Ui::changeSystem(int index, QString base) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->changeSystem(base.toStdString());
}

void Ui::parse(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->parse();
}

void Ui::run(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->run();
}

void Ui::runLine(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->runLine();
}

void Ui::runBreakpoint(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->runBreakpoint();
}

void Ui::stop(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->stop();
}

void Ui::reset(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->reset();
}

void Ui::save(int index) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->save();
}

void Ui::saveAs(int index, QString name) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->saveAs(name);
}

void Ui::saveSnapshot(int index, QString name) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->saveSnapshot(name);
}

void Ui::loadSnapshot(int index, QString name) {
  assert::that(index >= 0);
  assert::that(index < _projects.size());
  _projects[index]->loadSnapshot(name);
}
