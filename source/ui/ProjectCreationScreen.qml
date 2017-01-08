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
import QtQuick.Controls.Styles 1.4
import QtGraphicalEffects 1.0
import "Components/Common"
import Theme 1.0

/////////////////////////////
// Project Creation Screen
/////////////////////////////

Item {
  id: root

  signal buttonClicked(var memorySize,
    string architecture, var optionName, string parser);

    property var tab;

    property real marginWidth: width*0.1;
    property real marginHeight: height*0.1;

    property var main;

    property var baseExtensionsChecked: [];
    property var extensionsChecked: [];

    /////////////////////////////
    // Project Creation Button
    /////////////////////////////

    Button {
      id: button
      anchors {
        top: optionSelector.bottom
        horizontalCenter: parent.horizontalCenter
      }
      MouseArea {
        anchors.fill: button
        cursorShape: Qt.PointingHandCursor
        onClicked: {
          tab.title = textInputName.text;
          root.buttonClicked(
            memorySizeSelector.getCurrentValue(),
            architectureSelector.currentText,
            optionSelector.currentText,
            parserSelector.currentText
          );
        }
      }

      style: ButtonStyle {
        label: Item {
          anchors.centerIn: parent
          Text {
            anchors.centerIn: parent
            color: Theme.button.color
            font.pixelSize: Theme.button.fontSize
            font.capitalization: Font.AllUppercase
            text: "Create project"
          }
        }
        background: Rectangle {
          implicitWidth: Theme.createProject.add.width
          implicitHeight: Theme.button.height
          border.width: Theme.button.border.width
          border.color: Theme.button.border.color
          radius: Theme.button.radius
          color: Theme.createProject.add.backgroundColor
        }
      }
    }

    TextField {
      id: textInputName
      focus: true
      style: TextFieldStyle {
        background: Rectangle {
          radius: 2
          width: 100
          implicitWidth: 100
          implicitHeight: 24
          border.color: "#333"
          border.width: 1
        }
      }

      anchors {
        top: parent.top
        horizontalCenter: parent.horizontalCenter
      }

      placeholderText: "Give your project a name ..."
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
        top: textInputName.bottom
        left: parent.horizontalCenter
        right: parent.right

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }
      _value: 1024
      _minValue: 4
      _maxValue: 1048576
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
        optionSelector.model = ui.getOptionNames(currentArchitecture);
        parserSelector.model = ui.getParsers(currentArchitecture);
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
        top: memorySizeSelector.bottom
        left: parent.horizontalCenter
        right: parent.right

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }
    }

    Text {
      id: optionSelectorText
      anchors {
        left: optionSelector.left
        bottom: optionSelector.top
      }
      text: "Select a version."
    }

    //select a formula for the architecture
    ComboBox {
      id: optionSelector
      anchors {
        top: architectureSelector.bottom
        left: parent.left
        right: parent.horizontalCenter

        topMargin: marginHeight
        bottomMargin: 0
        leftMargin: marginWidth
        rightMargin: marginWidth
      }
    }
  }
