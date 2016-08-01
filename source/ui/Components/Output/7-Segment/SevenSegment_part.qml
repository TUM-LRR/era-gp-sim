import QtQuick 2.0
import QtQuick.Dialogs 1.0

Item {
    id: light
    height: 100
    width: 10
    x: 0
    y: 0
    property color color: "blue"

    Rectangle {
        id: ending1
        transform: Rotation { origin.x: ending1.width/2; origin.y: ending1.height/2; angle: 45}
        width: light.width/Math.sqrt(2)
        height: ending1.width
        color: light.color
        x: 0.5*(light.width - ending1.width)
        y: 0.5*(light.width - ending1.width)
    }
    Rectangle {
        id: ending2
        transform: Rotation { origin.x: ending2.width/2; origin.y: ending2.height/2; angle: 45}
        width: ending1.width
        height: ending1.height
        color: light.color
        x: 0.5*(light.width - ending1.width)
        y: light.height - light.width + 0.5*(light.width - ending1.width)
    }
    Rectangle {
        id: middle
        width: light.width
        height: light.height - light.width
        color: light.color
        x: 0
        y: 0.5*light.width
    }
}
