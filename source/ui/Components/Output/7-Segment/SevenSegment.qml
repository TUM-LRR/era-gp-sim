
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

/*
  a 7-segment digit for displaying numbers:
   _   _
  |_| |_|
  |_| |_|

*/

import QtQuick 2.0
import QtQuick.Window 2.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4

Item {
    id: sevenSegmentOutputItem

    // Index of output item relative to all available output outems.
    // Required to identify this output item when communicating with
    // the model.
    property var outputItemIndex: 1

    signal settingsButtonPressed()

    // Color definitions
    property var digitsInactiveColor: "#9B9B9B"
    property var digitsActiveColor: "#0080FF"

    // Connect the output item to signals that the model might send.
    Connections {
        target: outputComponent
        // Send when the memory changes (at any address).
        onMemoryChanged: {
            var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
            var numberOfDigits = outputComponent.getOutputItems()[outputItemIndex]["numberOfDigits"];
            // Check if the memory address that was changed (at least partly) belongs to
            // the output item's source space.
            if ((address+length) >= _baseAddress && (address <= (_baseAddress+(numberOfDigits)))) {
                updateContent(_baseAddress);
            }
        }
        // Sent when any item's settings where updated.
        onOutputItemSettingsChanged: {
            updateContent(outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]);
            settingsWindow.updateSettings();
        }
    }

    // Update the output item's content (there may already be some initial values in memory).
    Component.onCompleted: {
        updateContent(outputComponent.getOutputItems()[outputItemIndex]["baseAddress"]);
    }

    // Called from outside by the output tab view to signal that the settings button for the current
    // output item was pressed.
    onSettingsButtonPressed: {
        sevenSegmentSettingsWindow.show();
    }


    // ListView for seven segment digits.
    ListView {
        id: digitsListView

        anchors.centerIn: parent
        height: parent.height - 10
        spacing: 10
        width: (height / 2) * count + (spacing) * (count - 1)

        orientation: ListView.Horizontal
        interactive: false

        model: ListModel {
            id: sevenSegmentDigitsModel
        }

        delegate: sevenSegmentDigit
    }

    // Delegate for the ListView displaying each digit.
    Component {
        id: sevenSegmentDigit

        SevenSegmentDigit {
            height: parent.height
            width: height / 2
            thickness: height / 10

            color: "#00000000"  // Background Color
            digitActiveColor: digitsActiveColor
            digitInactiveColor: digitsInactiveColor
            textColor: "black"

            segment0Activated: segment0
            segment1Activated: segment1
            segment2Activated: segment2
            segment3Activated: segment3
            segment4Activated: segment4
            segment5Activated: segment5
            segment6Activated: segment6

            onSegmentPressed: {
                var segmentIdentifier = "segment" + segmentIndex;
                sevenSegmentDigitsModel.setProperty(index, segmentIdentifier, !sevenSegmentDigitsModel.get(index)[segmentIdentifier]);
                var memoryContent = [];
                for (var digitIndex = (sevenSegmentDigitsModel.count -1); digitIndex >= 0; --digitIndex) {
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment0"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment1"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment2"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment3"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment4"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment5"]);
                    memoryContent.push(sevenSegmentDigitsModel.get(digitIndex)["segment6"]);
                    memoryContent.push(false);  // Fill one byte.
                }
                var _baseAddress = outputComponent.getOutputItems()[outputItemIndex]["baseAddress"];
                outputComponent.putMemoryValue(_baseAddress, memoryContent);
            }
        }
    }



    // Updates the content of the output model depending on the value in memory.
    function updateContent(_baseAddress) {
        _updateSevenSegmentDigitsModel();

        var content = outputComponent.getMemoryContent(_baseAddress, sevenSegmentDigitsModel.count);
        for (var digitIndex = 0; digitIndex < sevenSegmentDigitsModel.count; ++digitIndex) {
            // Iterate memory bytewise from right to left, so the rightmost digit represents the first byte in memory.
            var digitInContentIndex = sevenSegmentDigitsModel.count -1 - digitIndex;
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment0", content[digitInContentIndex*8+0]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment1", content[digitInContentIndex*8+1]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment2", content[digitInContentIndex*8+2]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment3", content[digitInContentIndex*8+3]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment4", content[digitInContentIndex*8+4]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment5", content[digitInContentIndex*8+5]);
            sevenSegmentDigitsModel.setProperty(digitIndex, "segment6", content[digitInContentIndex*8+6]);
        }
    }

    // Updates the sevenSegmentDigitsModel to correspond to the output item's settings values.
    function _updateSevenSegmentDigitsModel() {
        var numberOfDigits = outputComponent.getOutputItems()[outputItemIndex]["numberOfDigits"];
        if (numberOfDigits < 0) {
            numberOfDigits = 0;
        }
        while (sevenSegmentDigitsModel.count < numberOfDigits) {
            sevenSegmentDigitsModel.insert(0, { segment0: false, segment1: false, segment2: false, segment3: false , segment4: false, segment5: false, segment6: false });
        }
        while (sevenSegmentDigitsModel.count > numberOfDigits) {
            sevenSegmentDigitsModel.remove(0, 1);
        }
    }


    SevenSegmentSettingsWindow {
        id: sevenSegmentSettingsWindow
    }

}
