import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#FFFFFF"
    border.color: "#E2E8F0"
    border.width: 1
    radius: 10

    property var bridge: null

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 6

        RowLayout {
            Text {
                text: "进度"
                color: "#1E293B"
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            Item { Layout.fillWidth: true }
            Text {
                text: bridge ? bridge.currentStage : ""
                color: "#6366F1"
                font.pixelSize: 12
                font.weight: Font.Medium
            }
        }

        ProgressBar {
            Layout.fillWidth: true
            from: 0
            to: 1.0
            value: bridge ? bridge.stageProgress : 0
            background: Rectangle {
                implicitHeight: 8
                color: "#E2E8F0"
                radius: 4
            }
            contentItem: Rectangle {
                implicitHeight: 8
                color: "#6366F1"
                radius: 4
                width: parent.visualPosition * parent.width
            }
        }

        Text {
            text: bridge ? bridge.stageStatus : ""
            color: "#64748B"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            visible: bridge ? bridge.stageEta > 0 : false
            text: "预计剩余: " + Math.ceil((bridge && bridge.stageEta ? bridge.stageEta : 0) / 60) + " 分钟"
            color: "#F59E0B"
            font.pixelSize: 11
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#F8FAFC"
            border.color: "#E2E8F0"
            radius: 6

            ListView {
                id: logView
                anchors.fill: parent
                anchors.margins: 6
                model: logModel
                delegate: Text {
                    text: modelData
                    color: "#475569"
                    font.pixelSize: 11
                    wrapMode: Text.Wrap
                    font.family: "monospace"
                }

                property ListModel logModel: ListModel {}
            }
        }
    }
}
