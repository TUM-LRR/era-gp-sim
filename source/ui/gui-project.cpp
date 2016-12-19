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

#include <QUrl>
#include <cstdio>
#include <functional>

#include "common/translateable.hpp"
#include "common/utility.hpp"
#include "ui/snapshot-component.hpp"
#include "ui/ui.hpp"

GuiProject::GuiProject(
    QQmlContext* context,
    const ArchitectureFormula& formula,
    std::size_t memorySize,
    const std::string& parserName,
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
, _inputBM(context, _projectModule.getMemoryAccess())
, _inputTM(context, _projectModule.getMemoryAccess())
, _inputCM(context, _projectModule.getMemoryAccess())
, _memoryModel(_projectModule.getMemoryAccess(),
               _projectModule.getMemoryManager(),
               context)
, _defaultTextFileSavePath()
, _snapshotComponent(snapshotComponent)
, _architectureFormulaString(SnapshotComponent::architectureToString(formula))
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

  _projectModule.getParserInterface().setThrowErrorCallback(
      [this](const auto& message, const auto& arguments) {
        this->_throwError(message, arguments);
      });

  _projectModule.getMemoryManager().setErrorCallback(
      [this](const auto& message, const auto& arguments) {
        this->_throwError(message, arguments);
      });

  _projectModule.getParserInterface().setFinalRepresentationCallback(
      [this](const auto& finalRepresentation) {
        this->finalRepresentationChanged(finalRepresentation);
      });

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
}

GuiProject::~GuiProject() {
  stop();
}

void GuiProject::changeSystem(std::string base) {
  // Alle Komponenten informieren
}

void GuiProject::parse() {
  _editorComponent.parse();
}

void GuiProject::run() {
  _editorComponent.parse();
  _projectModule.getCommandInterface().execute();
}

void GuiProject::runLine() {
  _editorComponent.parse();
  _projectModule.getCommandInterface().executeNextLine();
}

void GuiProject::runBreakpoint() {
  _editorComponent.parse();
  _projectModule.getCommandInterface().executeToBreakpoint();
}

void GuiProject::stop() {
  _projectModule.stopExecution();
}

void GuiProject::reset() {
  _projectModule.reset();
  _projectModule.getCommandInterface().setExecutionPoint(1);
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
  auto qName = path.path();
  _defaultTextFileSavePath = qName;
  auto name = qName.toStdString();
  auto text = _editorComponent.getText().toStdString();
  try {
    Utility::storeToFile(name, text);
  } catch (const std::exception& exception) {
    _throwError(std::string("Could not save file! ") + exception.what(), {});
  }
}

void GuiProject::loadText(const QUrl& path) {
  auto qName = path.path();
  auto filePath = qName.toStdString();
  std::string text;
  try {
    text = Utility::loadFromFile(filePath);
    auto qText = QString::fromStdString(text);
    _editorComponent.setText(qText);
  } catch (const std::exception& exception) {
    _throwError(std::string("Could not load file!") + exception.what(), {});
  }
}

void GuiProject::saveSnapshot(const QString& qName) {
  Json snapshot = _projectModule.getMemoryManager().generateSnapshot().get();
  std::string snapshotString = snapshot.dump(4);
  try {
    _snapshotComponent->addSnapshot(
        _architectureFormulaString, qName, snapshotString);
  } catch (const std::exception& exception) {
    _throwError(
        std::string("Could not write snapshot to disk! ") + exception.what(),
        {});
  }
}

void GuiProject::removeSnapshot(const QString& qName) {
  _snapshotComponent->removeSnapshot(_architectureFormulaString, qName);
}

void GuiProject::loadSnapshot(const QString& qName) {
  try {
    std::string path =
        _snapshotComponent->snapshotPath(_architectureFormulaString, qName);
    Json snapshot = Json::parse(Utility::loadFromFile(path));
    _projectModule.getMemoryManager().loadSnapshot(snapshot);
    _editorComponent.parse(true);
  } catch (const std::exception& exception) {
    _throwError(
        std::string("Could not load snapshot from file! ") + exception.what(),
        {});
  }
}

QStringList GuiProject::getSnapshots() {
  return _snapshotComponent->getSnapshotList(_architectureFormulaString);
}

QString GuiProject::getCommandHelp(std::size_t line) {
  QString help = "";
  // try to find the helptext in the cache
  auto iterator = _helpCache.find(line);
  if (iterator != _helpCache.end()) {
    help = iterator->second;
  } else {
    for (const auto& command : _commandList) {
      if (command.position.lineStart == line) {
        auto translateable = command.node->getInstructionDocumentation();
        help = Ui::translate(translateable);
        _helpCache.emplace(line, help);
      }
    }
  }
  return help;
}

std::function<std::string(MemoryValue)> GuiProject::getHexConversion() {
  return hexConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getBinConversion() {
  return binConversion;
}

std::function<std::string(MemoryValue)> GuiProject::getOctConversion() {
  return octConversion;
}

std::function<std::string(MemoryValue)>
GuiProject::getSignedDecimalConversion() {
  return signedDecimalConversion;
}

std::function<std::string(MemoryValue)>
GuiProject::getUnsignedDecimalConversion() {
  return unsignedDecimalConversion;
}

std::function<std::string(MemoryValue)>
GuiProject::getDecimalFloatConversion() {
  return decimalFloatConversion;
}

std::function<MemoryValue(std::string)> GuiProject::getSignedToMemoryValue() {
  return signedToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getHexToMemoryValue() {
  return hexToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getBinToMemoryValue() {
  return binToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getOctToMemoryValue() {
  return octToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getUnsignedToMemoryValue() {
  return unsignedToMemoryValue;
}

std::function<MemoryValue(std::string)> GuiProject::getFloatToMemoryValue() {
  return floatToMemoryValue;
}

void GuiProject::_throwError(const std::string& message,
                             const std::vector<std::string>& arguments) {
  auto errorMessage = QString::fromStdString(message);
  emit error(errorMessage);
}

void GuiProject::_updateCommandList(
    const FinalRepresentation& finalRepresentation) {
  _commandList = finalRepresentation.commandList;
  _helpCache.clear();
}
