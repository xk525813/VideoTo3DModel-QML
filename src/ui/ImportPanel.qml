import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root
    clip: true
    color: "#FFFFFF"
    border.color: "#E2E8F0"
    border.width: 1
    radius: 10

    property var bridge: null
    property var settingsPanel: null
    property string selectedFilePath: ""
    property bool fileSelected: false

    Layout.fillWidth: true
    Layout.preferredWidth: 280
    Layout.maximumWidth: 300

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            text: "导入"
            color: "#1E293B"
            font.pixelSize: 17
            font.weight: Font.Bold
        }

        Rectangle {
            id: dropZone
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: fileDropArea.containsDrag ? "#EEF2FF" : "#F8FAFC"
            border.color: fileDropArea.containsDrag ? "#6366F1" : "#CBD5E1"
            border.width: fileDropArea.containsDrag ? 2 : 1
            radius: 8

            DropArea {
                id: fileDropArea
                anchors.fill: parent
                onDropped: (drop) => {
                    if (drop.urls.length > 0) {
                        let path = drop.urls[0].toString()
                        if (path.startsWith("file://"))
                            path = path.substring(7)
                        root.selectedFilePath = path
                        root.fileSelected = true
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: root.fileSelected
                          ? root.selectedFilePath.split("/").pop()
                          : "拖放视频文件到此处"
                    color: root.fileSelected ? "#059669" : "#94A3B8"
                    font.pixelSize: 13
                    elide: Text.ElideMiddle
                    width: parent.width - 16
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        Button {
            id: fileBrowseBtn
            text: "选择文件..."
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            flat: false
            onClicked: fileBrowseDialog.open()

            background: Rectangle {
                color: fileBrowseBtn.hovered ? "#4F46E5" : "#6366F1"
                radius: 6
            }
            contentItem: Text {
                text: fileBrowseBtn.text
                color: "#FFFFFF"
                font.pixelSize: 13
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        FileDialog {
            id: fileBrowseDialog
            title: "选择视频文件"
            nameFilters: ["视频文件 (*.mp4 *.mov *.avi *.mkv)"]
            onAccepted: {
                let path = selectedFile.toString()
                if (path.startsWith("file://"))
                    path = path.substring(7)
                root.selectedFilePath = path
                root.fileSelected = true
            }
        }

        Item { Layout.fillHeight: true }

        Button {
            id: startPipelineBtn
            text: bridge && bridge.pipelineState === "running" ? "处理中..." : "开始重建"
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            enabled: bridge && bridge.pipelineState !== "running" && root.fileSelected
            onClicked: {
                if (bridge && root.fileSelected && root.settingsPanel) {
                    bridge.startPipeline(root.selectedFilePath, {
                        outputDir: root.settingsPanel.outputDir,
                        gpuEnabled: root.settingsPanel.gpuEnabled,
                        quality: root.settingsPanel.qualityChoice,
                        exportFormat: root.settingsPanel.exportFormat
                    })
                }
            }

            background: Rectangle {
                color: startPipelineBtn.enabled ? "#10B981" : "#CBD5E1"
                radius: 8
            }
            contentItem: Text {
                text: startPipelineBtn.text
                color: startPipelineBtn.enabled ? "#FFFFFF" : "#94A3B8"
                font.pixelSize: 14
                font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
