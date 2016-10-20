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
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import "Components"
import "Components/Menubar"
import "Components/Toolbar"

ApplicationWindow {
    id: window
    visible: true

    menuBar: Menubar{
        component: tabs
        main: window
    }
    toolBar: ToolbarMainWindow{
        id: toolbar
        tabView: tabView
    }

    TabView{
        anchors.fill: parent
        id: tabView

        Component.onCompleted: {
            createProject()
        }
    }

    function createProject() {
        tabView.addTab("Create new project...", tabs);
        tabView.currentIndex = tabView.count - 1;
    }

    function closeProject() {
        var currentTabIndex = tabView.currentIndex;
        tabView.removeTab(currentTabIndex);
        ui.removeProject(currentTabIndex);
    }

    /*Component for a project, instantiated by the TabView*/
    Component{
        id: tabs

        Item {
            id: placeholderItem
            anchors.fill: parent
            ProjectCreationScreen {
                anchors.fill: parent
                tab: parent.parent
                onButtonClicked: {
                    enabled = false;
                    visible = false;
                    ui.addProject(placeholderItem, projectComponent,
                      memorySize, architecture, baseExtensions, extensions, parser);
                }
            }
        }
    }

    //this component is instantiated by the addProject method
    Component{
        id: projectComponent
        Splitview{
            anchors.fill: parent
        }
    }
}
