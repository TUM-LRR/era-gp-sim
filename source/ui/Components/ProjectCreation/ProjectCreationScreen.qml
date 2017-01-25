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
import Theme 1.0
import "Sections"

/////////////////////////////
// Project Creation Screen
/////////////////////////////

Item {
  id: root

  signal createProject(string projectName,
                       var memorySize,
                       string architecture,
                       var optionName,
                       string parser)

    ProjectName {
      id: projectName
      onChange: button.enabled = projectName.text.length > 0
    }

    ProjectArchitecture {
      id: architecture
      anchors.top: projectName.bottom
    }

    ProjectMemorySize {
      id: memorySize
      anchors.top: architecture.bottom
    }

    ProjectSyntax {
      id: syntax
      anchors.top: memorySize.bottom
      Connections {
        target: architecture
        onSelectionMade: syntax.model = ui.getParsers(selection)
      }
    }

    ProjectArchitectureVersion {
      id: version
      anchors.top: syntax.bottom
      Connections {
        target: architecture
        onSelectionMade: version.model = ui.getOptionNames(selection)
      }
    }

    ProjectCreationButton {
      id: button
      anchors.top: version.bottom
      onClicked: {
        root.createProject(
          projectName.text,
          memorySize.selection,
          architecture.selection,
          version.selection,
          syntax.selection
        );
      }
    }

    Keys.onPressed: {
      // == on purpose (to coerce types)
      if (event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
        if (button.enabled) button.clicked();
      }
    }
  }
