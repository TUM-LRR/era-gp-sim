#ifndef INPUTTEXTMODEL_H
#define INPUTTEXTMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputTextModel : QObject {
  Q_OBJECT
 public:
  InputTextModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief Sets the new text which should be stored in the memory
   * \param text the new Text
   */
  Q_INVOKABLE void newText(QString text);

  /**
   * \brief Sets the new startindex
   * \param text the index
   */
  Q_INVOKABLE void setStart(unsigned int start);

  /**
   * \brief sets the new Maximum Length
   * \param text the new length
   */
  Q_INVOKABLE void setMaximumLength(unsigned int maximumLength);

  /**
   * \brief Gets the lenght
   * \return the length
   */
  Q_INVOKABLE unsigned int getMaximumLength();

  /**
   * \brief Gets the start index
   * \return the length
   */
  Q_INVOKABLE QString getStart();

 private:
  QQmlContext* context;
  int start;
  unsigned int maximumLength;
  MemoryAccess memoryAccess;
};

#endif// INPUTTEXTMODEL_H
