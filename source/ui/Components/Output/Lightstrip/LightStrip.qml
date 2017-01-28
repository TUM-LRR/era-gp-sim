import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.0
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import Theme 1.0

import "../"
import "../../Common"

Rectangle {
  // Color definitions
  property var lightstripInactiveColor: {
    return Theme.output.lightStrip.strip.inactive.background;
  }

  property var lightstripDefaultActiveColor: {
    return Theme.output.lightStrip.strip.active.background;
  }

  // Index of output item relative to all available output outems.
  // Required to identify this output item when communicating with
  // the model.
  property var outputItemIndex: 0

  // Called by Output.qml (i.e. component wrapper) when it receives
  // the signal that component settings icon was pressed.
  signal settingsButtonPressed()

  color: Theme.output.lightStrip.background

  ListModel {
    id: lightstripModel
  }

  // Update the output item's content (there may already be some initial values
  // in memory).
  Component.onCompleted: {
    var output = outputComponent.getOutputItem(outputItemIndex);
    updateContent(output["baseAddress"]);
  }

  // Connect the output item to signals that the model might send.
  Connections {
    target: outputComponent
    // Send when the memory changes (at any address).
    onMemoryChanged: {
      var output = outputComponent.getOutputItem(outputItemIndex);
      var _baseAddress = output["baseAddress"];
      // Check if the memory address that was changed (at least partly) belongs
      // to the output item's source space.
      if ((address + length) >= _baseAddress &&
          (address <= (_baseAddress+(lightstripModel.count + 7)/8))) {
        updateContent(_baseAddress);
      }
    }
    // Send when any item's settings where updated.
    onOutputItemSettingsChanged: {
      var output = outputComponent.getOutputItem(outputItemIndex);
      updateContent(output["baseAddress"]);
      lightstripSettingsWindow.updateSettings();
    }
  }

  // Called from outside by the output tab view to signal that the settings
  // button for the current output item was pressed.
  onSettingsButtonPressed: lightstripSettingsWindow.show();

  // Updates the content of the output model depending on the value in memory.
  function updateContent(_baseAddress) {
    _updatelightstripModel();
    var content = outputComponent.getMemoryContent(
      _baseAddress,
      (lightstripModel.count + 7) / 8
    );

    for (var bitIndex = 0;
         bitIndex < content.length && bitIndex < lightstripModel.count;
        ++bitIndex) {
      lightstripModel.setProperty(bitIndex, "active", content[bitIndex]);
    }
  }

  // Updates the lightstripModel to correspond to
  // the output item's settings values.
  function _updatelightstripModel() {
    var numberOfStrips =
      outputComponent.getOutputItem(outputItemIndex)["numberOfStrips"];
    if (numberOfStrips < 0) {
      numberOfStrips = 0
    }

    while (lightstripModel.count < numberOfStrips) {
      lightstripModel.append({
        activeColor: lightstripDefaultActiveColor,
        inactiveColor: lightstripInactiveColor,
        active: false
      });
    }

    while (lightstripModel.count > numberOfStrips) {
      lightstripModel.remove(lightstripModel.count - 1, 1)
    }
  }

  SystemPalette {
    id: systemColorPalette
    colorGroup: SystemPalette.Active
  }

  // Background/Border of the lightstrip.
  Rectangle {
    id: lightstripBackground
    color: "#00000000"

    // The lightstrip is rastered in strips. The amount of strips can be set via
    // the current theme and alters the amount of raster-strips according to the
    // following formula: (rasterGranularity * numberOfStrips + 1). Each spacing
    // between strips uses one raster-strip of space. Each lightstrip uses the
    // remaining space (e.g. if rasterGranularity is 5, the lightstrip each uses
    // 4 and the spacing each uses 1). e.g. 5 in the example above.
    property var rasterGranularity: {
      return Theme.output.lightStrip.strip.rasterGranularity;
    }

    // e.g. 4 in the example above.
    property var stripWidthParts: rasterGranularity - 1;

    // Each theme can set a maximum width for a lightstrip. The following
    // formula calculates whether using the full space of the
    // lightstripBackground would make the lightstrips exceed the maximum width.
    // In this case the lightstrip width using the maximum width is returned.
    property var lightstripWidth: {
      var normalWidth = parent.width - 2 * Theme.output.lightStrip.margin;
      var stripWidth = (stripWidthParts * normalWidth)
                    / (rasterGranularity * lightstripModel.count + 1);
      if (stripWidth > Theme.output.lightStrip.strip.maximumWidth) {
        var result = rasterGranularity * lightstripModel.count + 1;
        result /= stripWidthParts;
        result *= Theme.output.lightStrip.strip.maximumWidth;
        return result;
      } else {
        return normalWidth;
      }
    }

    anchors.verticalCenter: parent.verticalCenter
    anchors.horizontalCenter: parent.horizontalCenter
    radius: Theme.output.lightStrip.cornerRadius
    border.color: lightstripInactiveColor
    border.width: Theme.output.lightStrip.borderWidth
    width: lightstripWidth
    height: {
      return Math.min(
        parent.height - 2 * Theme.output.lightStrip.margin,
        Theme.output.lightStrip.maximumHeight
      );
    }

    // ListView for lights.
    ListView {
      spacing: {
        return lightstripBackground.width /
          (lightstripBackground.rasterGranularity * lightstripModel.count + 1);
      }
      anchors.fill: parent
      anchors.margins: spacing

      orientation: ListView.Horizontal
      interactive: false

      model: lightstripModel
      delegate: light
    }

    // Single light.
    Component {
      id: light
      Rectangle {
        id: rect

        color: (active == true) ? activeColor : inactiveColor
        height: parent.height
        anchors.verticalCenter: parent.center
        width: {
          var result = lightstripBackground.stripWidthParts;
          result *= lightstripBackground.width;
          result /= (lightstripBackground.rasterGranularity
                     * lightstripModel.count + 1);
          return result;
        }
        border.color: systemColorPalette.highlight
        border.width: 0
        radius: Theme.output.lightStrip.strip.cornerRadius

        MouseArea {
          anchors.fill: parent
          hoverEnabled: true
          cursorShape: Qt.PointingHandCursor
          acceptedButtons: Qt.LeftButton | Qt.RightButton
          onClicked: {
            // Show change-color-dialog on right-click.
            if(mouse.button === Qt.RightButton){
              var newObject = colorChooserComponent.createObject(light);
            } else {
              // Set light active on left-click.
              var active = !lightstripModel.get(index).active;
              lightstripModel.setProperty(index, "active", active);
              var memoryContent = [];
              for (var lightIndex = 0;
                   lightIndex < lightstripModel.count;
                   ++lightIndex) {
                memoryContent.push(lightstripModel.get(lightIndex).active);
              }
              var output = outputComponent.getOutputItem(outputItemIndex);
              var _baseAddress = output["baseAddress"];
              outputComponent.putMemoryValue(_baseAddress, memoryContent);
            }
          }
          onExited: rect.border.width = 0
          onEntered: {
            rect.border.width =
              Theme.output.lightStrip.strip.hovered.borderWidth
          }

          Component {
            id: colorChooserComponent
            ColorChooser {
              id: colorDialog
              currentColor: lightstripModel.get(index).activeColor
              onAccepted: {
                var color = colorDialog.color.toString();
                lightstripModel.setProperty(index, "activeColor", color);
                colorDialog.close();
              }
            }
          }
        }
      }
    }
  }

  LightstripSettingsWindow {
    id: lightstripSettingsWindow
  }
}
