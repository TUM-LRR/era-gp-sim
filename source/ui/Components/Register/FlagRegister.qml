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


CheckBox {
    id: registerCheckBox

    Component.onCompleted: {
        checked = registerModel.contentStringForRegister(styleData.index, dataTypeFormatComboBox.currentIndex)
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
                checked = registerModel.contentStringForRegister(styleData.index, dataTypeFormatComboBox.currentIndex)
            }
        }
    }

    // Notify the model that the register's content was changed by the user.
    onCheckedChanged: {
        var registerContentString = (checked == true) ? "1" : "0";
        registerModel.registerContentChanged(styleData.index, registerContentString, dataTypeFormatComboBox.currentIndex);
    }
}
