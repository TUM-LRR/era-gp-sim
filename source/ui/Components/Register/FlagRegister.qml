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
import QtGraphicalEffects 1.0


CheckBox {
    id: registerCheckBox

    checked: (model !== null) ? model.FlagData : false

    enabled: (model !== null) ? !model.IsConstant : true

    // As some values need to be set manually (i.e. not using the model's data-method and the corresponding
    // roles), they also have to be updated manually whenever the model's data changed for the current index.
    Connections {
        target: registerModel
        // Data changed is emitted when some of the model's data has changed (e.g. refer to `updateContent`-
        // method).
        onDataChanged: {
            // Check if the current item's index is affected by the data change.
            if (topLeft <= styleData.index && styleData.index <= bottomRight) {
                checked = Qt.binding(function() {return model.FlagData});
                registerCheckBox.style = styleChanged;
            }
        }
    }

    // Notify the model that the register's content was changed by the user.
    onCheckedChanged: {
        var registerContentString = (checked == true) ? "1" : "0";
        registerModel.registerContentChanged(styleData.index, registerContentString, dataTypeFormatComboBox.currentText);
    }

    style: whiteRectangle

    Component{
        id: styleChanged
        CheckBoxStyle{
            background:  Loader {
                id: loader
                focus: false
                sourceComponent: glowEffect
            }

            Component {
                id: glowEffect
                Item{
                    Rectangle{
                        id: rect
                        x: registerCheckBox.x; y: registerCheckBox.y
                        width: registerCheckBox.width; height: registerCheckBox.height
                        color: "white"

                    }
                    Glow {
                        x: rect.x; y: rect.y
                        width: rect.width; height: rect.height
                        source: rect
                        color: "#0080FF"
                        radius: 8
                        samples: 17
                        visible: true

                        NumberAnimation on spread {
                            from: 0; to: 0.5; duration: 1000
                            loops: 3
                            easing.type: Easing.InOutQuad
                            onStopped: {
                                registerCheckBox.style = whiteRectangle;
                            }
                        }
                    }
                }
            }
        }
    }


    Component {
        id: whiteRectangle
        CheckBoxStyle{
            background: Rectangle {
                x: registerCheckBox.x; y: registerCheckBox.y
                width: registerCheckBox.width; height: registerCheckBox.height
                color: "white"
            }
        }
    }
}
