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

#include <QByteArray>
#include <QUrl>
#include <string>

#include "arch/common/architecture-formula.hpp"
#include "common/assert.hpp"
#include "common/translateable.hpp"
#include "common/utility.hpp"
#include "parser/common/final-representation.hpp"
#include "ui/console-component.hpp"
#include "ui/output-component.hpp"
#include "ui/pixel-display-painted-item.hpp"
#include "ui/settings.hpp"
#include "ui/snapshot-component.hpp"
#include "ui/theme.hpp"

Q_DECLARE_METATYPE(FinalRepresentation)
Q_DECLARE_METATYPE(Status)

Ui::id_t Ui::_rollingProjectId = 0;

Ui::Ui(int& argc, char** argv)
: _architectureMap(), _qmlApplication(argc, argv), _engine(), _projects() {
  _loadArchitectures();
}

int Ui::runUi() {
  _registerCustomTypes();
  if (_setupEngine()) {
    _startMainEngine();
  } else {
    _startErrorEngine();
  }

  return _qmlApplication.exec();
}

Ui::id_t Ui::addProject(QQuickItem* tabItem,
                        QQmlComponent* projectComponent,
                        const QVariant& memorySizeQVariant,
                        const QString& architecture,
                        const QString& optionName,
                        const QString& parser,
                        const QString& projectName) {
  // create ArchitectureFormula
  ArchitectureFormula architectureFormula(architecture.toStdString());

  // Add all extensions which are defined for this option
  for (const auto& qstring : _getOptionFormula(architecture, optionName)) {
    architectureFormula.addExtension(qstring.toStdString());
  }

  // Get the memory size from the qvariant object.
  auto memorySize = memorySizeQVariant.value<std::size_t>();

  return _createProject(tabItem,
                        projectComponent,
                        architectureFormula,
                        memorySize,
                        parser.toStdString(),
                        projectName.toStdString());
}

QStringList Ui::getArchitectures() const {
  return _architectureMap.keys();
}

QStringList Ui::getOptionNames(const QString& architectureName) const {
  auto formulaMap =
      std::get<0>(_architectureMap.find(architectureName).value());
  return formulaMap.keys();
}

QStringList Ui::getParsers(const QString& architectureName) const {
  return std::get<1>(_architectureMap.find(architectureName).value());
}

void Ui::removeProject(id_t id) {
  _projects.erase(id);
}

void Ui::changeSystem(id_t id, const QString& base) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->changeSystem(base.toStdString());
}

void Ui::parse(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->parse();
}

void Ui::run(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->run();
}

void Ui::runLine(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->runLine();
}

void Ui::runBreakpoint(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->runBreakpoint();
}

void Ui::stop(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->stop();
}

void Ui::reset(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->reset();
}


void Ui::saveText(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveText();
}

void Ui::saveTextAs(id_t id, const QUrl& path) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveTextAs(path);
}

void Ui::loadText(id_t id, const QUrl& path) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->loadText(path);
}

void Ui::saveSnapshot(id_t id, const QString& name) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveSnapshot(name);
}

void Ui::loadSnapshot(id_t id, const QString& name) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->loadSnapshot(name);
}

void Ui::saveProject(id_t id) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveProject();
}

void Ui::saveProjectAs(id_t id, const QUrl& path) {
  auto iterator = _projects.find(id);
  assert::that(iterator != _projects.end());
  iterator->second->saveProjectAs(path);
}

Ui::id_t Ui::loadProject(QQuickItem* tabItem,
                         QQmlComponent* projectComponent,
                         const QUrl& url,
                         int newTab) {
  auto path = url.toLocalFile();
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) {
    _sendCreationFailedSignal(
        Translateable(QT_TRANSLATE_NOOP("GUI error messages",
                                        "Could not load project file!")),
        newTab);
  }

  auto contents = file.readAll();

  if (contents.isEmpty()) {
    _sendCreationFailedSignal(
        Translateable(
            QT_TRANSLATE_NOOP("GUI error messages", "Project file is empty!")),
        newTab);
  }

  auto json = Json::parse(contents.toStdString());
  Snapshot projectSnapshot(json);

  if (!projectSnapshot.isValidProject()) {
    _sendCreationFailedSignal(
        Translateable(QT_TRANSLATE_NOOP(
            "GUI error messages", "This snapshot is not a project file!")),
        newTab);
  }

  auto projectId = _createProject(tabItem,
                                  projectComponent,
                                  projectSnapshot.getArchitectureFormula(),
                                  projectSnapshot.getMemorySize(),
                                  projectSnapshot.getParserName(),
                                  projectSnapshot.getProjectName());
  auto iterator = _projects.find(projectId);
  assert::that(iterator != _projects.end());
  iterator->second->projectNameChanged(
      QString::fromStdString(projectSnapshot.getProjectName()));
  iterator->second->setText(QString::fromStdString(projectSnapshot.getCode()));
  iterator->second->loadSnapshot(projectSnapshot);
  iterator->second->loadProjectSettings(projectSnapshot.getProjectSettings());
  iterator->second->setDefaultProjectPath(url);

  return projectId;
}


QString Ui::translate(const Translateable& translateable) {
  QString translation = QObject::tr(translateable.getBaseString().c_str());
  for (const auto& operand : translateable.getOperands()) {
    translation = translation.arg(translate(*operand));
  }
  return translation;
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

QStringList
Ui::_getOptionFormula(QString architectureName, QString optionName) const {
  auto formulaMap =
      std::get<0>(_architectureMap.find(architectureName).value());
  return formulaMap.find(optionName).value();
}

void Ui::_registerCustomTypes() {
  qRegisterMetaType<std::size_t>("std::size_t");
  qRegisterMetaType<std::size_t>("size_t");
  qRegisterMetaType<FinalRepresentation>();
  qRegisterMetaType<Status>();
  qRegisterMetaType<id_t>("id_t");
  qRegisterMetaType<OutputComponent*>("OutputComponentPointer");
  qmlRegisterType<PixelDisplayPaintedItem>(
      "eragpsim.pixeldisplaypainteditem", 1, 0, "PixelDisplayPaintedItem");
}

bool Ui::_setupEngine() {
  QString message;

  auto status = Settings::Make();
  if (status) {
    _setupSnapshots(Settings::get("snapshotLocation").toString());
    status = Theme::Make(Settings::get("theme").toString());
  }

  if (!status) message = QString::fromStdString(status.message());
  _engine.rootContext()->setContextProperty("errorMessageFromStartup", message);

  // No error message means the setup was successful
  return message.isEmpty();
}

void Ui::_startMainEngine() {
  // clang-format off
  qmlRegisterSingletonType<Theme>(
      "Theme", 1, 0, "Theme",
      [](auto* qmlEngine, auto* jsEngine) -> QObject* {
        return Theme::pointer();
      });
  qmlRegisterSingletonType<Settings>(
      "Settings", 1, 0, "Settings",
      [](auto* qmlEngine, auto* jsEngine) -> QObject* {
        return Settings::pointer();
      });
  // clang-format on

  _engine.rootContext()->setContextProperty("ui", this);
  _engine.rootContext()->setContextProperty("snapshotComponent",
                                            _snapshots.get());
  _engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
}

void Ui::_startErrorEngine() {
  _engine.load(QUrl(QStringLiteral("qrc:/ErrorDialog.qml")));
}

void Ui::_setupSnapshots(const QString& snapshotLocation) {
  _snapshots = std::make_shared<SnapshotComponent>(snapshotLocation);
  // clang-format off
  QObject::connect(Settings::pointer(), &Settings::snapshotLocationChanged,
                   [this](const auto& path){
                     this->_snapshots->snapshotDirectory(path);
                   });
  // clang-format on
}

Ui::id_t Ui::_createProject(QQuickItem* tabItem,
                            QQmlComponent* projectComponent,
                            const ArchitectureFormula& architectureFormula,
                            size_t memorySize,
                            const std::string& parserName,
                            const std::string& projectName) {
  // Parent is tabItem, so it gets destroyed at the same time
  auto context = new QQmlContext(qmlContext(tabItem), tabItem);

  // save the project pointer in a vector, the object is deleted by qml when
  // tabItem is deleted
  auto projectId = _rollingProjectId;
  auto project = new GuiProject(context,
                                architectureFormula,
                                memorySize,
                                parserName,
                                projectName,
                                _snapshots,
                                tabItem);
  _projects.emplace(_rollingProjectId++, project);

  // instantiate the qml project item with the prepared context
  auto projectItem =
      qobject_cast<QQuickItem*>(projectComponent->create(context));

  // set the parent of projectItem, so its deletion is handled by qml
  projectItem->setParent(tabItem);

  // set visual parent of the projectItem
  projectItem->setParentItem(tabItem);

  return projectId;
}

void Ui::_sendCreationFailedSignal(const Translateable& message, int index) {
  auto translatedMessage = translate(message);
  emit projectCreationFailed(translatedMessage, index);
}
