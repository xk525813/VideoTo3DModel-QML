import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#1e1e2e"
    border.color: "#313244"
    border.width: 1
    radius: 8

    property var bridge: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 4

        RowLayout {
            Text {
                text: "进度"
                color: "#cdd6f4"
                font.pixelSize: 14
                font.weight: Font.Bold
            }
            Item { Layout.fillWidth: true }
            Text {
                text: bridge ? bridge.currentStage : ""
                color: "#a6e3a1"
                font.pixelSize: 12
            }
        }

        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: 1.0
            value: bridge ? bridge.stageProgress : 0
            background: Rectangle {
                implicitHeight: 8
                color: "#313244"
                radius: 4
            }
            contentItem: Rectangle {
                implicitHeight: 8
                color: "#89b4fa"
                radius: 4
                width: parent.visualPosition * parent.width
            }
        }

        Text {
            text: bridge ? bridge.stageStatus : ""
            color: "#a6adc8"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            visible: bridge ? bridge.stageEta > 0 : false
            text: "预计剩余: " + Math.ceil((bridge && bridge.stageEta ? bridge.stageEta : 0) / 60) + " 分钟"
            color: "#f9e2af"
            font.pixelSize: 11
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#11111b"
            radius: 4

            ListView {
                id: logView
                anchors.fill: parent
                anchors.margins: 4
                model: logModel
                delegate: Text {
                    text: modelData
                    color: "#a6adc8"
                    font.pixelSize: 11
                    wrapMode: Text.Wrap
                }

                property ListModel logModel: ListModel {}
            }
        }
    }
}
