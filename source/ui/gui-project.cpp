#include <functional>

#include "ui/gui-project.hpp"

GuiProject::GuiProject(QQmlContext* context,
                       const ArchitectureFormula& formula,
                       const std::size_t& memorySize,
                       std::string parserName,
                       QObject* parent)
: QObject(parent)
, _projectModule(formula, memorySize, parserName)
, _editorComponent(context,
                   _projectModule.getParserInterface(),
                   _projectModule.getCommandInterface())
/*, registermodel(context)
, editormodel(context)
, snapmodel(context)
, memorymodel(context)*/ {
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
}

void GuiProject::run() {
  // tell Editor
}

void GuiProject::runLine() {
  // tell Editor
}

void GuiProject::runBreakpoint() {
  // tell Editor
}

void GuiProject::stop() {
  // tell editor
}

void GuiProject::reset() {
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
