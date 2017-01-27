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
import Theme 1.0

Rectangle {
  id: item

  color: Theme.input.keyInput.background

  Rectangle {
    id: focusRectangle

    property var marginsInactive: Theme.input.keyInput.inactive.margins
    property var marginsActive: Theme.input.keyInput.active.margins
    property var backgroundInactive: Theme.input.keyInput.inactive.background
    property var backgroundActive: Theme.input.keyInput.active.background
    property var defaultInfoText: "Click to start capturing key events"

    anchors.fill: parent
    anchors.margins: marginsInactive
    color: backgroundInactive

    TextEdit {
      id: infoText

      anchors.fill: parent
      readOnly: true
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      activeFocusOnPress: false; activeFocusOnTab: false
      wrapMode: TextEdit.Wrap
      color: Theme.input.keyInput.inactive.color

      text: focusRectangle.defaultInfoText
    }

    MouseArea {
      anchors.fill: parent
      cursorShape: Qt.PointingHandCursor
      onClicked: {
        focusRectangle.focus = !focusRectangle.focus;
        if (focusRectangle.focus) {
          focusRectangle.forceActiveFocus();
        }
      }
    }

    onFocusChanged: {
      if (focus) {
        gainFocusAnimation.start();
        infoText.text = "";
        infoText.font.bold = true;
        infoText.color = Theme.input.keyInput.active.color;
      } else {
        looseFocusAnimation.start();
        infoText.text = focusRectangle.defaultInfoText;
        infoText.font.bold = false;
        infoText.color = Theme.input.keyInput.inactive.color;
      }
    }

    Keys.onPressed: {
      event.accepted = true;
      infoText.text = inputKeyMod.getKeyDescription(event.key);
      inputKeyMod.keyPressed(event.key);
    }

    ParallelAnimation {
      id: looseFocusAnimation
      running: false
      PropertyAnimation {
        id: looseFocusMarginAnimation
        target: focusRectangle
        property: "anchors.margins"
        duration: Theme.input.keyInput.inactive.animation.marginAnimation.duration
        to: focusRectangle.marginsInactive
        easing.type: Easing.OutExpo
      }
      PropertyAnimation {
        id: looseFocusColorAnimation
        target: focusRectangle
        property: "color"
        duration: Theme.input.keyInput.inactive.animation.colorAnimation.duration
        to: focusRectangle.backgroundInactive
        easing.type: Easing.OutExpo
      }
    }

    ParallelAnimation {
      id: gainFocusAnimation
      running: false
      PropertyAnimation {
        id: gainFocusMarginAnimation
        target: focusRectangle
        property: "anchors.margins"
        duration: Theme.input.keyInput.active.animation.marginAnimation.duration
        to: focusRectangle.marginsActive
        easing.type: Easing.OutExpo
      }
      PropertyAnimation {
        id: gainFocusColorAnimation
        target: focusRectangle
        property: "color"
        duration: Theme.input.keyInput.active.animation.colorAnimation.duration
        to: focusRectangle.backgroundActive
        easing.type: Easing.OutExpo
      }
    }
  }

  function settingsButtonPressed() {
    settingsWindowIB.show();
  }

  KeyInputSettings {
    id: settingsWindowIB
  }
}
