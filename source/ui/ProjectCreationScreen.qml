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

  signal createProject(string projectName,
                       var memorySize,
                       string architecture,
                       var optionName,
                       string parser);

    property real marginWidth: width*0.1;
    property real marginHeight: height*0.1;

    property var main;

    property var baseExtensionsChecked: [];
    property var extensionsChecked: [];

    /////////////////
    // Project Name
    /////////////////

    ProjectCreationSection {
      id: projectName
      anchors.topMargin: Theme.createProject.marginTop
      TextField {
        id: projectNameField
        focus: true
        anchors.fill: parent
        width: Theme.createProject.width
        validator: RegExpValidator { regExp: /[\w ]+/ }
        placeholderText: "Give your project a name ..."
        style: TextFieldStyle {
          font.pixelSize: Theme.createProject.section.fontSize
        }
      }
    }

    /////////////////
    // Architecture
    /////////////////

    ProjectCreationSection {
      id: architecture
      anchors.top: projectName.bottom
      anchors.topMargin: Theme.createProject.marginTop

      Text {
        id: architectureLabel
        font.pixelSize: Theme.createProject.section.fontSize
        text: "Architecture:"
        anchors {
          left: parent.left
          leftMargin: Theme.createProject.section.margin
          verticalCenter: parent.verticalCenter
        }
      }

      ComboBox {
        id: architectureSelector
        model: ui.getArchitectures();
        anchors {
          right: parent.right
          rightMargin: Theme.createProject.section.margin
          left: architectureLabel.right
          leftMargin: Theme.createProject.section.margin
          verticalCenter: parent.verticalCenter
        }
        onCurrentIndexChanged: {
          var currentArchitecture = model[currentIndex];
          versionSelector.model = ui.getOptionNames(currentArchitecture);
          syntaxSelector.model = ui.getParsers(currentArchitecture);
        }
        style: ComboBoxStyle {
          font.pixelSize: Theme.createProject.section.fontSize
        }
      }
    }

    /////////////////
    // Memory Size
    /////////////////

    ProjectCreationSection {
      id: memorySize
      anchors.top: architecture.bottom

      Text {
        id: memorySizeLabel
        font.pixelSize: Theme.createProject.section.fontSize
        text: "Memory:"
        anchors {
          left: parent.left
          verticalCenter: parent.verticalCenter
          leftMargin: Theme.createProject.section.margin
        }
      }

      SpinBox {
        id: memorySizeSelector
        anchors {
          right: parent.right
          verticalCenter: parent.verticalCenter
          rightMargin: Theme.createProject.section.margin
        }
        value: 1024
        minimumValue: 4
        maximumValue: (1 << 20)
        stepSize: 2
        suffix: "B"
      }
    }

    ////////////
    // Syntax
    ////////////

    ProjectCreationSection {
      id: syntax
      anchors.top: memorySize.bottom

      Text {
        id: syntaxLabel
        font.pixelSize: Theme.createProject.section.fontSize
        text: "Syntax:"
        anchors {
          left: parent.left
          verticalCenter: parent.verticalCenter
          leftMargin: Theme.createProject.section.margin
        }
      }

      ComboBox {
        id: syntaxSelector
        anchors {
          right: parent.right
          rightMargin: Theme.createProject.section.margin
          verticalCenter: parent.verticalCenter
        }
        style: ComboBoxStyle {
          font.pixelSize: Theme.createProject.section.fontSize
        }
      }
    }

    ////////////
    // Version
    ////////////

    ProjectCreationSection {
      id: version
      anchors.top: syntax.bottom

      Text {
        id: versionLabel
        font.pixelSize: Theme.createProject.section.fontSize
        text: "Version:"
        anchors {
          left: parent.left
          verticalCenter: parent.verticalCenter
          leftMargin: Theme.createProject.section.margin
        }
      }

      ComboBox {
        id: versionSelector
        anchors {
          right: parent.right
          rightMargin: Theme.createProject.section.margin
          left: versionLabel.right
          leftMargin: Theme.createProject.section.margin
          verticalCenter: parent.verticalCenter
        }
        style: ComboBoxStyle {
          font.pixelSize: Theme.createProject.section.fontSize
        }
      }
    }

    /////////////////////////////
    // Project Creation Button
    /////////////////////////////

    Button {
      id: button
      anchors {
        top: version.bottom
        horizontalCenter: parent.horizontalCenter
        topMargin: Theme.createProject.button.marginTop
      }

      MouseArea {
        anchors.fill: button
        cursorShape: Qt.PointingHandCursor
        onClicked: {
          root.createProject(
            projectName.text,
            memorySizeSelector.value,
            architectureSelector.currentText,
            versionSelector.currentText,
            syntaxSelector.currentText
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
          implicitWidth: Theme.createProject.button.width
          implicitHeight: Theme.button.height
          border.width: Theme.button.border.width
          border.color: Theme.button.border.color
          radius: Theme.button.radius
          color: Theme.createProject.button.backgroundColor
        }
      }
    }
  }
