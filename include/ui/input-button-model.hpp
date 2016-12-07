#ifndef INPUTBUTTONMODEL_H
#define INPUTBUTTONMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputButtonModel : public QObject {
  Q_OBJECT
 public:
  InputButtonModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief wrotes clicked Button in memory
   * \param id  Id of clicked Button
   */
  Q_INVOKABLE void buttonClicked(unsigned int id);

  /**
   * \brief Returns value of start
   */
  Q_INVOKABLE QString getStart();

  /**
   * \brief sets start
   * \param start new value
   */
  Q_INVOKABLE void setStart(unsigned int start);

 private:
  QQmlContext* context;
  /**
   * @brief the base addresss in the memory
   */
  unsigned int start;
  MemoryAccess memoryAccess;
};

#endif// INPUTBUTTONMODEL_H
