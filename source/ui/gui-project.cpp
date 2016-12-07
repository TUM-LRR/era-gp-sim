#include <functional>

#include "ui/gui-project.hpp"

GuiProject::GuiProject(QQmlContext* context,
                       const ArchitectureFormula& formula,
                       std::size_t memorySize,
                       const std::string& parserName,
                       QObject* parent)
: QObject(parent)
, _projectModule(formula, memorySize, parserName)
, _editorComponent(context,
                   _projectModule.getParserInterface(),
                   _projectModule.getCommandInterface())
, _memoryModel(_projectModule.getMemoryAccess(),
               _projectModule.getMemoryManager(),
               context)
, _registerModel(_projectModule.getArchitectureAccess(),
                 _projectModule.getMemoryManager(),
                 _projectModule.getMemoryAccess(),
                 context)
, _outputComponent(_projectModule.getMemoryManager(),
                   _projectModule.getMemoryAccess(),
                   context)
/*, registermodel(context)
, editormodel(context)
, snapmodel(context)
, memorymodel(context)*/ {
  // set the callback for memory and register
  _projectModule.getMemoryManager().setUpdateRegisterCallback(
      [this](const std::string& name) {
        emit registerChanged(QString::fromStdString(name));
      });

  _projectModule.getMemoryManager().setUpdateMemoryCallback(
      [this](std::size_t address, std::size_t length) {
        emit memoryChanged(address, length);
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

  std::string name[] = {"Apfel", "Banane"};
  // snapmodel.addList(name);
  // An alle Komponenten weitergeben
  // alle Komponenten initialisieren
  // Alle Functions initialisieren
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
}

void GuiProject::save() {
  // tell core
}

void GuiProject::saveAs(QString name) {
  std::string stdname = name.toStdString();
  // tell core
}

void GuiProject::saveSnapshot(QString name) {
  std::string stdname = name.toStdString();
  // dont know, what to do
}

void GuiProject::loadSnapshot(QString name) {
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
