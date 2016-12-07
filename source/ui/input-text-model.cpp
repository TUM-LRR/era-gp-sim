#include <iostream>

#include "core/conversions.hpp"
#include "ui/input-text-model.hpp"

InputTextModel::InputTextModel(QQmlContext* context, MemoryAccess memoryAccess)
: QObject()
, context(context)
, start(0)
, maxLength(20)
, memoryAccess(memoryAccess) {
  context->setContextProperty("inputtextMod", this);
}

void InputTextModel::newText(QString text) {
  std::string stdText = text.toStdString();
  for (size_t i = 0; i < stdText.length(); i++) {
    char z = stdText.at(i);
    unsigned number = int(z);
    if (zahl < 225) {// else do nothing, was not a ascii sign
      MemoryValue m = conversions::convert(number, 32);
      memoryAccess.putMemoryValueAt(start + i, m);
    }
  }
}

void InputTextModel::setStart(unsigned int start) {
  if (memoryAccess.getMemorySize().get() >= start + maximumLength) {
    this->start = start;
  }
}

QString InputTextModel::getStart() {
  return QString::fromStdString(std::to_string(start));
}

void InputTextModel::setMaximumLength(unsigned int maximumLength) {
  this->maximumLength = maximumLength;
}

unsigned int InputTextModel::getMaximumLength() {
  return maximumLength;
}
