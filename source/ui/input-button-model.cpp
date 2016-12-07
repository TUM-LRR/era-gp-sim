#include <iostream>

#include "core/conversions.hpp"
#include "ui/input-button-model.hpp"

InputButtonModel::InputButtonModel(QQmlContext* context,
                                   MemoryAccess memoryAccess)
: QObject(), context(context), start(0), memoryAccess(memoryAccess) {
  context->setContextProperty("inputButtonMod", this);
}

void InputButtonModel::buttonClicked(unsigned int id) {
  // save id at start in Memory
  MemoryValue m = conversions::convert(id, 32);
  memoryAccess.putMemoryValueAt(start, m);
}

QString InputButtonModel::getStart() {
  return QString::fromStdString(std::to_string(start));
}

void InputButtonModel::setStart(unsigned int start) {
  if (memoryAccess.getMemorySize().get() > start + 2) {
    this->start = start;
  }
}
