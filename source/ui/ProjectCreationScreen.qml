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

import QtQuick 2.6
import QtQuick.Controls 1.4
import "Components/Common"

Item {
    id: root

    signal buttonClicked(var memorySize,
                  string architecture, var baseExtensions, var extensions, string parser);

    property var tab;

    property real marginWidth: width*0.1;
    property real marginHeight: height*0.1;

    property var main;

    property var baseExtensionsChecked: [];
    property var extensionsChecked: [];

    //create the project
    Button {
        id: button
        anchors {
            top: extensionGrid.bottom
            horizontalCenter: parent.horizontalCenter

            topMargin: 0
            bottomMargin: marginHeight
            leftMargin: marginWidth
            rightMargin: marginWidth
        }
        enabled: false
        text: "create Project"
        onClicked: {
            tab.title = textInputName.text;
            root.buttonClicked(memorySizeSelector._value,
              architectureSelector.currentText, baseExtensionsChecked, extensionsChecked,
              parserSelector.currentText);
        }
    }

    //project name
    TextField {
        id: textInputName
        anchors {
            top: parent.top
            right: parent.horizontalCenter
            left: parent.left

            topMargin: marginHeight
            bottomMargin: marginHeight
            leftMargin: marginWidth
            rightMargin: marginWidth
        }

        placeholderText: "project name"
    }

    Text {
        id: memorySizeText
        anchors {
            left: memorySizeSelector.left
            bottom: memorySizeSelector.top
        }
        text: "Memory size"
    }

    Text {
      id: memorySizeFormat
      anchors {
        left: memorySizeSelector.right
        verticalCenter: memorySizeSelector.verticalCenter
        leftMargin: 3
      }
      text: "bytes"
    }

    //choose the memory size
    NumericUpDown {
        id: memorySizeSelector
        anchors {
            top: parent.top
            left: parent.horizontalCenter
            right: parent.right

            topMargin: marginHeight
            bottomMargin: 0
            leftMargin: marginWidth
            rightMargin: marginWidth
        }
        _value: 1024
        _maxValue: 16384
        _step: 4
    }

    Text {
        id: archText
        anchors {
            left: architectureSelector.left
            bottom: architectureSelector.top
        }
        text: "Architecture"
    }

    //choose the architecture
    ComboBox {
        id: architectureSelector
        anchors {
            top: textInputName.bottom
            left: parent.left
            right: parent.horizontalCenter

            topMargin: marginHeight
            bottomMargin: marginHeight
            leftMargin: marginWidth
            rightMargin: marginWidth
        }
        model: ui.getArchitectures();
        onCurrentIndexChanged: {
          var currentArchitecture = model[currentIndex];
          baseExtensionGrid.model = ui.getBaseExtensions(currentArchitecture);
          extensionGrid.model = ui.getExtensions(currentArchitecture);
          parserSelector.model = ui.getParsers(currentArchitecture);
          extensionsChecked = [];
          baseExtensionsChecked = [];
          button.enabled = false;
        }
    }

    Text {
      id: parserText
      anchors {
        left: parserSelector.left
        bottom: parserSelector.top
      }
      text: "Parser"
    }

    //choose the parser
    ComboBox {
      id: parserSelector
      anchors {
        top: textInputName.bottom
        left: parent.horizontalCenter
        right: parent.right

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }
    }

    Text {
      id: baseExtensionText
      anchors {
        left: baseExtensionGrid.left
        bottom: baseExtensionGrid.top

        bottomMargin: marginHeight/5
      }
      text: "Base extensions (check at least one)"
    }

    //choose base extensions
    GridView {
      id: baseExtensionGrid
      height: contentHeight
      anchors {
        top: parserSelector.bottom
        left: parent.left
        right: parent.right

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }

      Component {
        id: checkboxDelegateBase
        CheckBox {
          text: modelData
          onCheckedChanged: {
            if(checked) {
              button.enabled = true;
              baseExtensionsChecked.push(text);
            } else {
              var index = baseExtensionsChecked.indexOf(text);
              baseExtensionsChecked.splice(index, 1);
              if(baseExtensionsChecked.length == 0) {
                button.enabled = false;
              }
            }
          }
        }
      }
      delegate: checkboxDelegateBase
      cellHeight: 50
      interactive: false
    }


    Text {
      id: extensionText
      anchors {
        left: extensionGrid.left
        bottom: extensionGrid.top

        bottomMargin: marginHeight/5
      }
      text: "Extensions"
    }

    //choose further extensions
    GridView {
      id: extensionGrid
      height: contentHeight
      anchors {
        top: baseExtensionGrid.bottom
        left: parent.left
        right: parent.right

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }

      Component {
        id: checkboxDelegate
        CheckBox {
          text: modelData
          onCheckedChanged: {
            if(checked){
              extensionsChecked.push(text);
            } else {
              var index = extensionsChecked.indexOf(text);
              extensionsChecked.splice(index, 1);
            }
          }
        }
      }

      delegate: checkboxDelegate
      cellHeight: 50
      interactive: false
    }
}
