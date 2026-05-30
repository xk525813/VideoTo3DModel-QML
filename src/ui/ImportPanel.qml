import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

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
        spacing: 8

        Text {
            text: "导入"
            color: "#cdd6f4"
            font.pixelSize: 16
            font.weight: Font.Bold
        }

        Rectangle {
            id: dropArea
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            color: "#181825"
            border.color: dragHandler.containsDrag ? "#89b4fa" : "#45475a"
            border.width: dragHandler.containsDrag ? 3 : 2
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
                    color: selectedFileLabel.visible ? "#a6e3a1" : "#6c7086"
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

        Button {
            text: "选择文件..."
            Layout.fillWidth: true
            onClicked: fileDialog.open()
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

        Button {
            text: bridge && bridge.pipelineState === "running" ? "处理中..." : "开始重建"
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            enabled: bridge && bridge.pipelineState !== "running" && selectedFileLabel.visible
            palette.button: "#89b4fa"
            palette.buttonText: "#1e1e2e"

            onClicked: {
                if (bridge && selectedFileLabel.visible) {
                    let settings = {
                        outputDir: "",
                        gpuEnabled: true,
                        quality: "standard",
                        exportFormat: "glb"
                    }
                    bridge.startPipeline(selectedFileLabel.text, settings)
                }
            }
        }
    }
}
