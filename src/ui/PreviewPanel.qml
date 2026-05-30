import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#FFFFFF"
    border.color: "#E2E8F0"
    border.width: 1
    radius: 10

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "3D 预览"
            color: "#94A3B8"
            font.pixelSize: 20
            font.weight: Font.Bold
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            text: "处理完成后将在此处显示模型预览"
            color: "#CBD5E1"
            font.pixelSize: 13
            Layout.alignment: Qt.AlignHCenter
        }

        Rectangle {
            width: 300
            height: 200
            color: "#F1F5F9"
            border.color: "#E2E8F0"
            border.width: 1
            radius: 8
            Layout.alignment: Qt.AlignHCenter

            Text {
                anchors.centerIn: parent
                text: "预览区域"
                color: "#CBD5E1"
                font.pixelSize: 14
            }
        }
    }
}
