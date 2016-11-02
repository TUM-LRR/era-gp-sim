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
    property var main
    property var component
    id: menubar
    Menu{
        title: "File"
        MenuItem{
            text: "Open..."
            onTriggered: {
                console.info("Open triggerd");
            }
        }
        MenuItem{
            text: "New..."
            onTriggered: {
                console.info("New triggerd");
                main.createProject();
            }
        }
        MenuItem{
            text: "Save"
            onTriggered: {
                console.info("Save triggerd");
            }
        }
        MenuItem{
            text: "Save as..."
            onTriggered: {
                console.info("Save as triggerd");
            }
        }
        MenuItem{
            text: "Snapshot"
            onTriggered: {
                console.info("Snapshot triggerd");
            }
        }
        MenuItem{
            text: "Delete"
            onTriggered: {
                console.info("Delete Triggerd");
                main.closeProject();
            }
        }
    }

    Menu{
        title: "Settings"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Open Settings triggerd")
            }
        }
    }

    Menu{
        title: "Help"
        MenuItem{
            text: "Open"
            onTriggered: {
                console.info("Help open triggerd")
            }
        }
    }

}
