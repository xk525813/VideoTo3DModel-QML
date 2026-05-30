import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#11111b"
    border.color: "#313244"
    border.width: 1
    radius: 8

    ColumnLayout {
        anchors.centerIn: parent
        spacing: 12

        Text {
            text: "3D 预览"
            color: "#6c7086"
            font.pixelSize: 20
            Layout.alignment: Qt.AlignHCenter
        }
        Text {
            text: "处理完成后将在此处显示模型预览"
            color: "#585b70"
            font.pixelSize: 13
            Layout.alignment: Qt.AlignHCenter
        }

        // OpenGL 预览区域 — 阶段1占位
        Rectangle {
            width: 300
            height: 200
            color: "#181825"
            border.color: "#313244"
            border.width: 1
            radius: 4
            Layout.alignment: Qt.AlignHCenter

            Text {
                anchors.centerIn: parent
                text: "预览区域"
                color: "#45475a"
                font.pixelSize: 14
            }
        }
    }
}
