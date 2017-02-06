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

#include "ui/gui-project.hpp"

#include <QByteArray>
#include <QUrl>
#include <functional>
#include <string>

#include "common/string-conversions.hpp"
#include "common/utility.hpp"
#include "core/snapshot.hpp"
#include "ui/snapshot-component.hpp"
#include "ui/translateable-processing.hpp"
#include "ui/ui.hpp"

GuiProject::MemoryToStringConverterMap GuiProject::_memoryToStringMap = {
    {"BinaryData", StringConversions::toBinString},
    {"HexData", StringConversions::toHexString},
    {"SignedDecimalData", StringConversions::toSignedDecString},
    {"UnsignedDecimalData", StringConversions::toUnsignedDecString}};

GuiProject::StringToMemoryConverterMap GuiProject::_stringToMemoryMap = {
    {"BinaryData", StringConversions::binStringToMemoryValue},
    {"HexData", StringConversions::hexStringToMemoryValue},
    {"SignedDecimalData", StringConversions::signedDecStringToMemoryValue},
    {"UnsignedDecimalData", StringConversions::unsignedDecStringToMemoryValue}};


GuiProject::GuiProject(
    QQmlContext* context,
    const ArchitectureFormula& formula,
    std::size_t memorySize,
    const std::string& parserName,
    const std::string& projectName,
    const std::shared_ptr<SnapshotComponent>& snapshotComponent,
    QObject* parent)
: QObject(parent)
, _projectModule(formula, memorySize, parserName)
, _registerModel(_projectModule.getArchitectureAccess(),
                 _projectModule.getMemoryManager(),
                 _projectModule.getMemoryAccess(),
                 context)
, _editorComponent(context,
                   _projectModule.getParserInterface(),
                   _projectModule.getCommandInterface())
, _outputComponent(_projectModule.getMemoryManager(),
                   _projectModule.getMemoryAccess(),
                   context)
, _inputKeys(context, _projectModule.getMemoryAccess())
, _inputMouse(context, _projectModule.getMemoryAccess())
, _consoleComponent(context, _projectModule.getMemoryAccess())
, _memoryModel(_projectModule.getMemoryAccess(),
               _projectModule.getMemoryManager(),
               context)
, _projectSettings(context)
, _defaultTextFileSavePath()
, _defaultProjectSavePath()
, _snapshotComponent(snapshotComponent)
, _architectureFormulaString(SnapshotComponent::architectureToString(formula))
, _parserName(parserName)
, _projectName(projectName)
, _commandList()
, _helpCache() {
  context->setContextProperty("guiProject", this);
  // set the callback for memory and register
  _projectModule.getMemoryManager().setUpdateRegisterCallback(
      [this](const auto& name) {
        emit this->registerChanged(QString::fromStdString(name));
      });

  _projectModule.getMemoryManager().setUpdateMemoryCallback(
      [this](auto address, auto length) {
        emit this->memoryChanged(address, length);
      });

  _projectModule.getMemoryManager().setUpdateMemorySizeCallback(
      [this](auto newSize) { emit this->memorySizeChanged(newSize); });

  _projectModule.getParserInterface().setThrowErrorCallback(
      [this](const Translateable& message) { this->_throwError(message); });

  _projectModule.getMemoryManager().setErrorCallback(
      [this](const Translateable& message) { this->_throwError(message); });

  _projectModule.getParserInterface().setFinalRepresentationCallback(
      [this](const auto& finalRepresentation) {
        this->finalRepresentationChanged(finalRepresentation);
      });
  _projectModule.getCommandInterface().setExecutionStoppedCallback(
      [this] { emit this->executionStopped(); });

  _projectModule.getCommandInterface().setSyncCallback(
      [this] { emit this->guiSync(); });

  // connect all receiving components to the callback signals
  QObject::connect(this,
                   SIGNAL(registerChanged(const QString&)),
                   &_registerModel,
                   SLOT(updateContent(const QString&)),
                   Qt::QueuedConnection);

  QObject::connect(this,
                   SIGNAL(memoryChanged(std::size_t, std::size_t)),
                   &_memoryModel,
                   SLOT(onMemoryChanged(std::size_t, std::size_t)),
                   Qt::QueuedConnection);

  QObject::connect(this,
                   SIGNAL(memoryChanged(std::size_t, std::size_t)),
                   &_outputComponent,
                   SLOT(updateMemory(std::size_t, std::size_t)),
                   Qt::QueuedConnection);

  QObject::connect(this,
                   SIGNAL(memorySizeChanged(std::size_t)),
                   &_memoryModel,
                   SLOT(onMemorySizeChanged(std::size_t)),
                   Qt::QueuedConnection);

  QObject::connect(
      this,
      SIGNAL(finalRepresentationChanged(const FinalRepresentation&)),
      &_editorComponent,
      SLOT(onFinalRepresentationChanged(const FinalRepresentation&)),
      Qt::QueuedConnection);

  QObject::connect(
      this,
      SIGNAL(finalRepresentationChanged(const FinalRepresentation&)),
      this,
      SLOT(_updateCommandList(const FinalRepresentation&)),
      Qt::QueuedConnection);

  QObject::connect(
      this, SIGNAL(guiSync()), this, SLOT(_notifyCore()), Qt::QueuedConnection);
}

GuiProject::~GuiProject() {
  stop();
  _notifyCore();
}

void GuiProject::changeSystem(const std::string& base) {
  // Alle Komponenten informieren
}

void GuiProject::parse() {
  _editorComponent.parse(true);
}

void GuiProject::run() {
  emit runClicked(false);
  _editorComponent.parse();
  _projectModule.getCommandInterface().execute();
}

void GuiProject::runLine() {
  emit runClicked(true);
  _editorComponent.parse();
  _projectModule.getCommandInterface().executeNextLine();
}

void GuiProject::runBreakpoint() {
  emit runClicked(false);
  _editorComponent.parse();
  _projectModule.getCommandInterface().executeToBreakpoint();
}

void GuiProject::stop() {
  _projectModule.stopExecution();
}

void GuiProject::reset() {
  emit runClicked(false);
  _projectModule.reset();
  _projectModule.getCommandInterface().setExecutionPoint(0);
  _editorComponent.parse(true);
}

void GuiProject::saveText() {
  if (_defaultTextFileSavePath.isEmpty()) {
    emit saveTextAs();
  } else {
    saveTextAs(_defaultTextFileSavePath);
  }
}

void GuiProject::saveTextAs(const QUrl& path) {
  auto qName = path.toLocalFile();
  _defaultTextFileSavePath = path;
  auto name = qName.toStdString();
  auto text = _editorComponent.getText().toStdString();
  try {
    Utility::storeToFile(name, text);
  } catch (const std::exception& exception) {
    _throwError(Translateable(
        QT_TRANSLATE_NOOP("GUI error messages", "Could not save file!\n%1"),
        exception.what()));
  }
}

void GuiProject::loadText(const QUrl& path) {
  auto qName = path.toLocalFile();
  auto filePath = qName.toStdString();
  std::string text;
  try {
    text = Utility::loadFromFile(filePath);
    auto qText = QString::fromStdString(text);
    _editorComponent.setText(qText);
  } catch (const std::exception& exception) {
    _throwError(Translateable(
        QT_TRANSLATE_NOOP("GUI error messages", "Could not load file!\n%1"),
        exception.what()));
  }
}

void GuiProject::saveSnapshot(const QString& qName) {
  // clang-format off
  auto snapshot = _projectModule
    .getMemoryManager()
    .generateSnapshot()
    .get()
    .getJson()
    .dump(4);
  // clang-format on

  try {
    _snapshotComponent->addSnapshot(
        _architectureFormulaString, qName, snapshot);
  } catch (const std::exception& exception) {
    _throwError(Translateable(
        QT_TRANSLATE_NOOP("GUI error messages",
                          "Could not write snapshot to disk!\n%1"),
        exception.what()));
  }
}

void GuiProject::saveProject() {
  if (_defaultProjectSavePath.isEmpty()) {
    emit saveProjectAsSignal();
  } else {
    saveProjectAs(_defaultProjectSavePath);
  }
}

void GuiProject::saveProjectAs(const QUrl& url) {
  _defaultProjectSavePath = url;
  auto path = url.toLocalFile();
  auto snapshot = _projectModule.getMemoryManager().generateSnapshot().get();

  snapshot.setCode(_editorComponent.getText().toStdString());
  snapshot.setParserName(_parserName);
  snapshot.setProjectName(_projectName);
  snapshot.setProjectSettings(_projectSettings.toJson());

  auto snapshotString = QByteArray(snapshot.getJson().dump(4).c_str());

  QFile file(path);

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    _throwError(Translateable(QT_TRANSLATE_NOOP(
        "GUI error messages", "Could not open project file for writing")));
  }

  if (file.write(snapshotString) == -1) {
    _throwError(Translateable(QT_TRANSLATE_NOOP(
        "GUI error messages", "Could not write to project file")));
  }
}

void GuiProject::setText(const QString& text) {
  _editorComponent.setText(text);
}

void GuiProject::loadSnapshot(const Snapshot& snapshot) {
  _projectModule.getMemoryManager().loadSnapshot(snapshot);
}

void GuiProject::loadProjectSettings(const Json& json) {
  _projectSettings.loadSettings(json);
}

void GuiProject::setDefaultProjectPath(const QUrl& url) {
  _defaultProjectSavePath = url;
}

void GuiProject::removeSnapshot(const QString& qName, bool removePermanently) {
  _snapshotComponent->removeSnapshot(
      _architectureFormulaString, qName, removePermanently);
}

void GuiProject::loadSnapshot(const QString& qName) {
  try {
    auto path =
        _snapshotComponent->snapshotPath(_architectureFormulaString, qName);
    Json snapshotData = Json::parse(Utility::loadFromFile(path));
    Snapshot snapshot(snapshotData);
    _projectModule.getMemoryManager().loadSnapshot(snapshot);
    _editorComponent.parse(true);
  } catch (const std::exception& exception) {
    _throwError(Translateable(
        QT_TRANSLATE_NOOP("GUI error messages",
                          "Could not load snapshot from file!\n%1"),
        exception.what()));
  }
}

QStringList GuiProject::getSnapshots() {
  return _snapshotComponent->getSnapshotList(_architectureFormulaString);
}

bool GuiProject::snapshotExists(QString name) {
  return _snapshotComponent->snapshotExists(_architectureFormulaString, name);
}

QString GuiProject::getCommandHelp(std::size_t line) {
  QString help = "";
  // try to find the helptext in the cache
  auto iterator = _helpCache.find(line);
  if (iterator != _helpCache.end()) {
    help = iterator->second;
  } else {
    bool helpFound = false;
    for (const auto& command : _commandList) {
      if (command.node() && command.position().startLine() == line) {
        auto translateable = command.node()->getInstructionDocumentation();
        help = Ui::translate(translateable);
        _helpCache.emplace(line, help);
        helpFound = true;
        break;
      }
    }
    if (!helpFound) {
      _helpCache.emplace(line, "");
    }
  }
  return help;
}

const GuiProject::MemoryToStringConverterMap&
GuiProject::getMemoryToStringConversions() {
  return _memoryToStringMap;
}

const GuiProject::StringToMemoryConverterMap&
GuiProject::getStringToMemoryConversions() {
  return _stringToMemoryMap;
}

void GuiProject::_throwError(const Translateable& message) {
  auto errorMessage = translate(message);
  emit error(errorMessage);
}

void GuiProject::_updateCommandList(
    const FinalRepresentation& finalRepresentation) {
  _commandList = finalRepresentation.commandList();
  _helpCache.clear();
  emit commandListUpdated();
}

void GuiProject::_notifyCore() {
  _projectModule.guiReady();
}
