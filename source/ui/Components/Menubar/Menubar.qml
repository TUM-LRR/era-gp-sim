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


import QtQuick 2.0
import QtQuick.Controls 1.4

MenuBar {
    /*Adding or deleting components in the tab-View*/
    property TabView tabView
    property var component
    id: menubar
    Menu{
        title: "File"
        MenuItem{
            text: "Open..."
            onTriggered: {
                console.info("Open triggerd");
                ui.open("PopUp ergaenzen");
            }
        }
        MenuItem{
            text: "New..."
            onTriggered: {
                console.info("New triggerd");
                tabView.addTab("test", component);
                ui.new("risc5", "popup ergaenzen");
            }
        }
        MenuItem{
            text: "Save"
            onTriggered: {
                console.info("Save triggerd");
                ui.save(tabView.currentIndex);
            }
        }
        MenuItem{
            text: "Save as..."
            onTriggered: {
                console.info("Save as triggerd");
                ui.saveAs("popup ergaenzen", tabView.currentIndex);
            }
        }
        MenuItem{
            text: "Snapshot"
            onTriggered: {
                console.info("Snapshot triggerd");
                ui.snapshot("popup für name", tabView.currentIndex)
            }
        }
        MenuItem{
            text: "Close"
            onTriggered: {
                console.info("Delete Triggerd");
                var index=tabView.currentIndex;
                tabView.removeTab(index);
                ui.closeProject(index);
            }
        }
    }

    Menu{
        title: "Settings"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Open Settings triggerd")
                ui.settings();
            }
        }
    }

    Menu{
        title: "Help"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Help open triggerd")
                ui.help();
            }
        }
    }

}

