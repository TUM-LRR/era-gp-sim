#include <iostream>

#include "core/conversions.hpp"
#include "ui/input-click-model.hpp"

InputClickModel::InputClickModel(QQmlContext *context,
                                 MemoryAccess memoryAccess)
: QObject(), context(context), memoryAccess(memoryAccess), start(0) {
  context->setContextProperty("inputClickMod", this);
}

void InputClickModel::newClick(int x, int y) {
  // save in Memory as 2 bytes
  MemoryValue m1 = conversions::convert(x, 32);
  MemoryValue m2 = conversions::convert(y, 32);

  memoryAccess.putMemoryValueAt(start, m1);
  memoryAccess.putMemoryValueAt(start + 1, m2);
}

void InputClickModel::setStart(unsigned int start) {
  if (memoryAccess.getMemorySize().get() > start + 2) {
    this->start = start;
  }
}

QString InputClickModel::getStart() {
  return QString::fromStdString(std::to_string(start));
}
