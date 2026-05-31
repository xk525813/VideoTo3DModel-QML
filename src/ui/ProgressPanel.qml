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
                id: currentStageLabel
                text: root.bridge ? root.bridge.currentStage : ""
                color: "#6366F1"
                font.pixelSize: 12
                font.weight: Font.Medium
            }
        }

        ProgressBar {
            id: stageProgressBar
            Layout.fillWidth: true
            from: 0
            to: 1.0
            value: root.bridge ? root.bridge.stageProgress : 0
            background: Rectangle {
                implicitHeight: 8
                color: "#E2E8F0"
                radius: 4
            }
            contentItem: Rectangle {
                implicitHeight: 8
                color: "#6366F1"
                radius: 4
                width: stageProgressBar.visualPosition * stageProgressBar.width
            }
        }

        Text {
            id: stageStatusLabel
            text: root.bridge ? root.bridge.stageStatus : ""
            color: "#64748B"
            font.pixelSize: 12
            wrapMode: Text.Wrap
            Layout.preferredHeight: 18
        }

        Text {
            id: stageEtaLabel
            visible: root.bridge ? root.bridge.stageEta > 0 : false
            text: "预计剩余: " + Math.ceil((root.bridge && root.bridge.stageEta ? root.bridge.stageEta : 0) / 60) + " 分钟"
            color: "#F59E0B"
            font.pixelSize: 11
            Layout.preferredHeight: 14
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#F8FAFC"
            border.color: "#E2E8F0"
            radius: 6

            ListView {
                id: logListView
                anchors.fill: parent
                anchors.margins: 6
                model: ListModel { id: pipelineLogModel }
                delegate: Text {
                    text: modelData
                    color: "#475569"
                    font.pixelSize: 11
                    font.family: "monospace"
                    wrapMode: Text.Wrap
                    width: logListView.width - 12
                }
            }
        }
    }
}
