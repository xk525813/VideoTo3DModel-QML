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
            id: dropArea
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: dragHandler.containsDrag ? "#EEF2FF" : "#F8FAFC"
            border.color: dragHandler.containsDrag ? "#6366F1" : "#CBD5E1"
            border.width: dragHandler.containsDrag ? 2 : 1
            radius: 8

            DropArea {
                id: dragHandler
                anchors.fill: parent
                onDropped: (drop) => {
                    if (drop.urls.length > 0) {
                        let path = drop.urls[0].toString()
                        if (path.startsWith("file://"))
                            path = path.substring(7)
                        selectedFileLabel.text = path
                        selectedFileLabel.visible = true
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: selectedFileLabel.visible
                          ? selectedFileLabel.text.split("/").pop()
                          : "拖放视频文件到此处"
                    color: selectedFileLabel.visible ? "#059669" : "#94A3B8"
                    font.pixelSize: 13
                    elide: Text.ElideMiddle
                    width: parent.width - 16
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }

        Text {
            id: selectedFileLabel
            visible: false
        }

        // 选择文件按钮
        Button {
            id: browseBtn
            text: "选择文件..."
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            flat: false
            onClicked: fileDialog.open()

            background: Rectangle {
                color: browseBtn.hovered ? "#4F46E5" : "#6366F1"
                radius: 6
            }
            contentItem: Text {
                text: browseBtn.text
                color: "#FFFFFF"
                font.pixelSize: 13
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        FileDialog {
            id: fileDialog
            title: "选择视频文件"
            nameFilters: ["视频文件 (*.mp4 *.mov *.avi *.mkv)"]
            onAccepted: {
                let path = selectedFile.toString()
                if (path.startsWith("file://"))
                    path = path.substring(7)
                selectedFileLabel.text = path
                selectedFileLabel.visible = true
            }
        }

        Item { Layout.fillHeight: true }

        // 开始重建按钮
        Button {
            id: startBtn
            text: bridge && bridge.pipelineState === "running" ? "处理中..." : "开始重建"
            Layout.fillWidth: true
            Layout.preferredHeight: 42
            enabled: bridge && bridge.pipelineState !== "running" && selectedFileLabel.visible
            onClicked: {
                if (bridge && selectedFileLabel.visible) {
                    bridge.startPipeline(selectedFileLabel.text, {
                        outputDir: "",
                        gpuEnabled: true,
                        quality: "standard",
                        exportFormat: "glb"
                    })
                }
            }

            background: Rectangle {
                color: startBtn.enabled ? "#10B981" : "#CBD5E1"
                radius: 8
            }
            contentItem: Text {
                text: startBtn.text
                color: startBtn.enabled ? "#FFFFFF" : "#94A3B8"
                font.pixelSize: 14
                font.weight: Font.Bold
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
