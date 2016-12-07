#ifndef INPUTCLICKMODEL_H
#define INPUTCLICKMODEL_H

#include <QObject>
#include <QQmlContext>

#include "core/memory-access.hpp"

class InputClickModel : QObject {
  Q_OBJECT
 public:
  InputClickModel(QQmlContext* context, MemoryAccess memoryAccess);

  /**
   * \brief the coordinates of the click
   * \param x x coordinate
   * \param y y coordinate
   */
  Q_INVOKABLE void newClick(int x, int y);

  /**
   * \brief returns value of start
   */
  Q_INVOKABLE QString getStart();

  /**
    * \brief sets value of start
    * \param start new value
    */
  Q_INVOKABLE void setStart(unsigned int start);


 private:
  /* Should be obviously*/
  QQmlContext* context;
  unsigned int start;
  MemoryAccess memoryAccess;
};

#endif// INPUTCLICKMODEL_H
