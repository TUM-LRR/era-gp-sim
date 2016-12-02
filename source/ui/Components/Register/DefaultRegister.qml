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


TextField {
    id: registerTextField

    font.pointSize: 13

    text: registerContent();

    // Fetches the register's content with the appropiate format from the model.
    function registerContent() {
        if (model === null) { return ""; }
        var registerContentString;
        switch (dataTypeFormatComboBox.currentText) {
        case "Binary":
            registerContentString = model.BinaryData;
            break;
        case "Hexadecimal":
            registerContentString = model.HexData;
            break;
        case "Decimal (Unsigned)":
            registerContentString = model.UnsignedDecData;
            break;
        case "Decimal (Signed)":
            registerContentString = model.SignedDecData;
            break;
        default:
            registerContentString = model.BinaryData;
            break;
        }
        return format(registerContentString);
    }


    // As some values need to be set manually (i.e. not using the model's data-method and the corresponding
    // roles), they also have to be updated manually whenever the model's data changed for the current index.
    Connections {
        target: registerModel
        // Data changed is emitted when some of the model's data has changed (e.g. refer to `updateContent`-
        // method).
        onDataChanged: {
            // Check if the current item's index is affected by the data change.
            if (topLeft <= styleData.index && styleData.index <= bottomRight) {
                text = Qt.binding(registerContent);
            }
        }
    }

    // Notify the model that the register's content was changed by the user.
    onEditingFinished: {
        registerModel.registerContentChanged(styleData.index, registerTextField.text, dataTypeFormatComboBox.currentText);
    }
    onAccepted: {
        registerModel.registerContentChanged(styleData.index, registerTextField.text, dataTypeFormatComboBox.currentText);
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_Tab) {
            event.accepted = true;
            registerTextField.focus = false;
        }
    }

    function format(registerContentString) {
        registerContentString = registerContentString.replace(/ /gi, '');
        if (dataTypeFormatComboBox.currentText === "Binary" || dataTypeFormatComboBox.currentText === "Hexadecimal") {
            var characterPerByte = (dataTypeFormatComboBox.currentText === "Hexadecimal") ? 2 : 8;
            // Insert new spaces
            for (var characterIndex = 2; characterIndex < registerContentString.length; characterIndex+=(characterPerByte+1)) {
                registerContentString = [registerContentString.slice(0, characterIndex), ' ', registerContentString.slice(characterIndex)].join('')
            }
        }
        return registerContentString;
    }

}
