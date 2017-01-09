/* C++ Assembler Interpreter
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
 * along with this program. If not, see http://www.gnu.org/licenses/.*/

import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQuick.Controls.Styles 1.4
import Theme 1.0

Item {
    // Displays the registers in a tree-like structure.
    TreeView {
        id: registerTreeView

        // Set the TreeView's style.
        anchors.fill: parent
        backgroundVisible: false
        style: TreeViewStyle {
            transientScrollBars: true
        }
        alternatingRowColors: false
        headerVisible: false
        selectionMode: SelectionMode.NoSelection

        // Signal to format all registres at once.
        signal formatAllRegisters(string format)
        // When loading, each register uses the default format if no other
        // cached format is available.
        property var defaultFormat: "Binary"

        onFormatAllRegisters: {
            // Set default format to make newly loaded registers use this format.
            defaultFormat = format;
            // Clear cache to prevent default format from being overwritten.
            dataTypeFormatCache = ({});
        }

        // Map for associating the currently selected data type format (e.g. bin, hex etc.)
        // with the QModelIndex of the corresponding register. This is required in order to be
        // able to restore the last currently selected format after a delegate item has been
        // delete because it was scrolled out of the visible area. This property cannot be
        // part of the model, as the model can be assigned to multiple TreeViews.
        property variant dataTypeFormatCache: ({})

        // Uses the RegisterModel to populate the registers.
        model: registerModel


        itemDelegate: Item {
            id: treeViewItemDelegate

            // Delegates of some well-supported QML-views (such as ListView) have a property
            // for accessing the view the delegate is attached to from a delegate's sub-component
            // via ListView.view.
            // However treeView does not have such a property. In order to still be able
            // to access some of the TreeView's properties, we have to create a property
            // inside the delegate itself, which can then be used from within the delegate's
            // components.
            property TreeView attachedTreeView: registerTreeView

            // Register Title
            Label {
                id: registerTitleLabel
                anchors.left: registerContentItem.left
                anchors.top: parent.top
                height: 18
                text: (model !== null) ? model.Title : ""
                font.pointSize: 15
                font.weight: Font.Bold
                color: "#585858"
            }
            // The actual content of the register. Its appearance depends on the current
            // format type (e.g. hex/bin/dec uses TextField, flag uses checkbox).
            Loader {
                id: registerContentItem
                anchors.top: registerTitleLabel.bottom
                anchors.topMargin: 3
                anchors.left: parent.left
                anchors.right: dataTypeFormatComboBox.left
            }
            // The list of available data formats.
            ComboBox {
                id: dataTypeFormatComboBox
                anchors.verticalCenter: registerContentItem.verticalCenter
                anchors.right: parent.right
                width: 18
                height: registerContentItem.height
                property bool shouldCacheFormatIndex: false
                model: {
                    // Property bindings are magic; dark, dark magic. So how often this
                    // function is called depends on your version of Qt. The problem is, whenever
                    // the model is changed, the currentIndex of the ComboBox is reset to 0 (=Binary)
                    // Depending on what your version of Qt is, this might happen when you edit the register
                    // or just scroll through. So the current format needs to be cached and restored.
                    // We only want to cache a current index if it was changed by the user rather than automatically
                    // (e.g. through reset). In order to prevent us from caching index 0 after the model was changed,
                    // we set dataTypeFormatComboBox.shouldCacheFormatIndex to false which prevents caches until it's
                    // set to true again in the onModelChanged-signal.
                    dataTypeFormatComboBox.shouldCacheFormatIndex = false;
                    // Note: model.Type does not work...
                    var type = registerModel.data(styleData.index, 1);
                    switch (type) {
                    case "Integer":
                        return formatListInteger;
                    case "Float":
                        return formatListFloat;
                    case "Vector":
                        return formatListVector;
                    case "Flag":
                        return formatListFlag;
                    case "Link":
                        return formatListLink;
                    case "ProgramCounter":
                        return formatListProgramCounter;
                    }
                }

                onModelChanged: {
                    // Refer to property-binding to dataTypeFormatComboBox.model for further information.
                    _tryToRestoreCachedFormatIndex();
                    dataTypeFormatComboBox.shouldCacheFormatIndex = true;
                }

                Component.onCompleted: {
                    // Refer to property-binding to dataTypeFormatComboBox.model for further information.
                    _tryToRestoreCachedFormatIndex();
                    dataTypeFormatComboBox.shouldCacheFormatIndex = true;
                }

                onCurrentIndexChanged: {
                    // Certain registers require a different content item than a text field.
                    // They are loaded when such a data format is selected.
                    // For the content to be reloaded correctly, the source component has to be
                    // cleared.
                    registerContentItem.sourceComponent = undefined;
                    if (currentText == "Flag") {
                        registerContentItem.source = "FlagRegister.qml";
                    } else {
                        registerContentItem.source = "DefaultRegister.qml"
                    }
                    // Cache the new currently selected data type format, but only if the
                    // index change was not triggered upon the creation of the combo box
                    // (defaults to 0 at creation time) and only if the index is valid
                    // (i.e. row and column are positive).
                    if (dataTypeFormatComboBox.shouldCacheFormatIndex && styleData.index.row >= 0 && styleData.index.column >= 0) {
                        attachedTreeView.dataTypeFormatCache[styleData.index] = currentIndex;
                    }
                }

                function _tryToRestoreCachedFormatIndex() {
                    // Try to restore a cached selected data type format.
                    if (attachedTreeView.dataTypeFormatCache[styleData.index] !== undefined) {
                        dataTypeFormatComboBox.currentIndex = attachedTreeView.dataTypeFormatCache[styleData.index];
                    } else {    // If no cached format could be restored, load the default format.
                        var defaultFormatIndex = indexOfFormat(attachedTreeView.defaultFormat);
                        // If the default format is not available for this register's type (e.g. decimal for program counter),
                        // load the format at index 0.
                        if (defaultFormatIndex !== -1 && defaultFormatIndex !== undefined) {
                            dataTypeFormatComboBox.currentIndex = defaultFormatIndex;
                        }
                    }
                }

                style: ComboBoxStyle {
                    // Selection Indicator
                    label: Item {
                        id: comboBoxSelectionIndicator
                        // Triangle Up
                        Text {
                            id: triangleUp
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: parent.top
                            anchors.topMargin: -3
                            text: "\u25b2"
                            font.pointSize: 7
                        }
                        // Triangle Down
                        Text {
                            id: triangleDown
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.top: triangleUp.bottom
                            anchors.topMargin: 2
                            text: "\u25bc"
                            font.pointSize: 7
                        }
                    }
                    // Remove ComboBox-background
                    background: Rectangle {
                        visible: false
                    }
                }
            }

            Connections {
                target: registerTreeView
                // When all registers should be formatted, change the format of all registers currently
                // visible (and therefore loaded).
                onFormatAllRegisters: {
                    var defaultFormatIndex = indexOfFormat(attachedTreeView.defaultFormat);
                    if (defaultFormatIndex !== -1) {
                        dataTypeFormatComboBox.currentIndex = defaultFormatIndex;
                    }
                }
            }

            // Available data formats for each type of register.
            property var formatListInteger: ["Binary", "Hexadecimal", "Decimal (Unsigned)", "Decimal (Signed)"]
            property var formatListFloat: ["Binary", "Hexadecimal"]
            property var formatListVector: ["Binary", "Hexadecimal"]
            property var formatListFlag: ["Flag", "Binary"]
            property var formatListLink: ["Binary", "Hexadecimal"]
            property var formatListProgramCounter: ["Binary", "Hexadecimal"]

            // Searches for a given data format string and returns the corresponding index inside the
            // register's associated type's format list.
            function indexOfFormat(format) {
                switch (registerModel.data(styleData.index, 1)) {
                case "Integer":
                    return formatListInteger.indexOf(format);
                case "Float":
                    return formatListFloat.indexOf(format);
                case "Vector":
                    return formatListVector.indexOf(format);
                case "Flag":
                    return formatListFlag.indexOf(format);
                case "Link":
                    return formatListLink.indexOf(format);
                case "ProgramCounter":
                    return formatListProgramCounter.indexOf(format);
                }
            }
        }

        rowDelegate: Item {
            height: 50
        }

        TableViewColumn {
            title: "Register"
        }
    }

    // Context menu for formatting all registers at once.
    Menu {
        id: contextMenu

        MenuItem {
            text: "All registers to Binary"
            onTriggered: {
                registerTreeView.formatAllRegisters("Binary");
            }
        }
        MenuItem {
            text: "All registers to Hexadecimal"
            onTriggered: {
                registerTreeView.formatAllRegisters("Hexadecimal");
            }
        }
        MenuItem {
            text: "All registers to Decimal (Unsigned)"
            onTriggered: {
                registerTreeView.formatAllRegisters("Decimal (Unsigned)");
            }
        }
        MenuItem {
            text: "All registers to Decimal (Signed)"
            onTriggered: {
                registerTreeView.formatAllRegisters("Decimal (Signed)");
            }
        }
        MenuItem {
            text: "All registers to Flag"
            onTriggered: {
                registerTreeView.formatAllRegisters("Flag");
            }
        }
    }

    // MouseArea that provides a context menu on right-click.
    MouseArea {
        anchors.fill: parent

        acceptedButtons: Qt.RightButton
        onClicked: {
            contextMenu.popup();
        }
    }
}
