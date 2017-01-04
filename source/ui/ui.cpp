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

#include "ui/clipboard-adapter.hpp"
#include "ui/ui.hpp"

#include <QUrl>

#include "arch/common/architecture-formula.hpp"
#include "common/assert.hpp"
#include "common/translateable.hpp"
#include "common/utility.hpp"
#include "parser/common/final-representation.hpp"
#include "ui/input-text-model.hpp"
#include "ui/output-component.hpp"
#include "ui/pixel-display-painted-item.hpp"
#include "ui/snapshot-component.hpp"
#include "ui/snapshot-component.hpp"
#include "ui/snapshot-component.hpp"// why is this multiplying?

Q_DECLARE_METATYPE(FinalRepresentation)

Ui::id_t Ui::_rollingProjectId = 0;

Ui::Ui(int& argc, char** argv)
: _architectureMap()
, _qmlApplication(argc, argv)
, _engine()
, _projects()
, _snapshots(std::make_shared<SnapshotComponent>(
      QString::fromStdString(Utility::joinToRoot("snapshots")))) {
  _loadArchitectures();
}

int Ui::runUi() {
  qmlRegisterType<ClipboardAdapter>(
      "ClipboardAdapter", 1, 0, "ClipboardAdapter");
  qRegisterMetaType<std::size_t>("std::size_t");
  qRegisterMetaType<size_t>("size_t");
  qRegisterMetaType<FinalRepresentation>();
  qRegisterMetaType<OutputComponent*>("OutputComponentPointer");
  qmlRegisterType<PixelDisplayPaintedItem>(
      "eragpsim.pixeldisplaypainteditem", 1, 0, "PixelDisplayPaintedItem");
  //qRegisterMetaType<InputText::length_t>("length_t");
  qRegisterMetaType<Ui::id_t>("id_t");
  _engine.rootContext()->setContextProperty("ui", this);
  _engine.rootContext()->setContextProperty("snapshotComponent",
                                            _snapshots.get());
  _engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return _qmlApplication.exec();
}

Ui::id_t Ui::addProject(QQuickItem* tabItem,
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
  //_engine.addImageProvider("pixeldisplayprovider", new PixelDisplayProvider);

  // save the project pointer in a vector, the object is deleted by qml when
  // tabItem is deleted
  unsigned int projectId = _rollingProjectId;
  auto project = new GuiProject(context,
                                architectureFormula,
                                memorySize,
                                parser.toStdString(),
                                _snapshots,
                                tabItem);
  _projects.emplace(_rollingProjectId++, project);

  // instantiate the qml project item with the prepared context
  QQuickItem* projectItem =
      qobject_cast<QQuickItem*>(projectComponent->create(context));

  // set the parent of projectItem, so its deletion is handled by qml
  projectItem->setParent(tabItem);

  // set visual parent of the projectItem
  projectItem->setParentItem(tabItem);

  return projectId;
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

void Ui::removeProject(int id) {
  _projects.erase(id);
}

void Ui::changeSystem(int id, QString base) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->changeSystem(base.toStdString());
}

void Ui::parse(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->parse();
}

void Ui::run(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->run();
}

void Ui::runLine(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->runLine();
}

void Ui::runBreakpoint(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->runBreakpoint();
}

void Ui::stop(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->stop();
}

void Ui::reset(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->reset();
}


void Ui::saveText(int id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveText();
}

void Ui::saveTextAs(int id, QUrl path) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveTextAs(path);
}

void Ui::loadText(int id, QUrl path) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->loadText(path);
}

void Ui::saveSnapshot(int id, QString name) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveSnapshot(name);
}

void Ui::loadSnapshot(int id, QString name) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->loadSnapshot(name);
}

QString Ui::translate(const Translateable& translateable) {
  QString translation = QObject::tr(translateable.getBaseString().c_str());
  for (const auto& operand : translateable.getOperands()) {
    translation = translation.arg(translate(*operand));
  }
  return translation;
}
