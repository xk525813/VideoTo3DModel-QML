import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import video3d.pipeline 1.0

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 1280
    height: 800
    title: "VideoTo3D"
    color: "#F0F4F8"

    palette {
        window: "#F0F4F8"
        windowText: "#1E293B"
        base: "#FFFFFF"
        text: "#1E293B"
        button: "#6366F1"
        buttonText: "#FFFFFF"
    }

    PipelineBridge {
        id: pipelineBridge

        onPipelineError: (title, message) => {
            pipelineErrorDialog.dialogTitle = title
            pipelineErrorDialog.dialogMessage = message
            pipelineErrorDialog.open()
        }
        onPipelineFinished: (success, message) => {
            pipelineFinishDialog.success = success
            pipelineFinishDialog.dialogMessage = message
            pipelineFinishDialog.open()
        }
    }

    footer: Rectangle {
        height: 32
        color: "#E2E8F0"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 8
            Text {
                text: "状态: " + pipelineBridge.pipelineState
                color: "#334155"
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }
            Text {
                text: "GPU: " + (pipelineBridge.hardwareProfile.hasGPU
                       ? pipelineBridge.hardwareProfile.gpuName
                       : "未检测到")
                color: pipelineBridge.hardwareProfile.hasGPU ? "#059669" : "#DC2626"
                font.pixelSize: 12
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        ColumnLayout {
            Layout.preferredWidth: 280
            Layout.minimumWidth: 240
            Layout.maximumWidth: 300
            Layout.fillHeight: true
            spacing: 10

            ImportPanel {
                id: importPanel
                Layout.fillWidth: true
                Layout.preferredHeight: 340
                bridge: pipelineBridge
                settingsPanel: settingsPanel
            }
            SettingsPanel {
                id: settingsPanel
                Layout.fillWidth: true
                Layout.fillHeight: true
                bridge: pipelineBridge
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            PreviewPanel {
                id: previewPanel
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ProgressPanel {
                id: progressPanel
                Layout.fillWidth: true
                Layout.preferredHeight: 130
                bridge: pipelineBridge
            }
        }

        ExportPanel {
            id: exportPanel
            Layout.preferredWidth: 240
            Layout.minimumWidth: 200
            Layout.maximumWidth: 260
            Layout.fillHeight: true
            bridge: pipelineBridge
        }
    }

    Dialog {
        id: pipelineErrorDialog
        property string dialogTitle: ""
        property string dialogMessage: ""
        title: pipelineErrorDialog.dialogTitle
        modal: true
        implicitWidth: 460
        leftPadding: 28
        rightPadding: 24
        topPadding: 24
        bottomPadding: 16
        footer: DialogButtonBox {
            alignment: Qt.AlignRight
            background: Rectangle { color: "transparent" }
            Button {
                text: "确定"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                flat: false
                implicitWidth: 80
                implicitHeight: 34
                background: Rectangle { color: "#EF4444"; radius: 6 }
                contentItem: Text {
                    text: parent.text; color: "#FFFFFF"; font.weight: Font.Medium
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
            }
        }

        background: Rectangle {
            radius: 12
            color: "#FFFFFF"
            border.color: "#FECACA"
            border.width: 1

            Rectangle {
                width: 4
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 1
                color: "#EF4444"
                radius: 2
            }
        }

        contentItem: ColumnLayout {
            spacing: 12

            RowLayout {
                spacing: 10
                Rectangle {
                    width: 32; height: 32
                    radius: 16
                    color: "#FEE2E2"
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "#EF4444"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                }
                Text {
                    text: pipelineErrorDialog.dialogTitle
                    color: "#991B1B"
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
            }

            Text {
                text: pipelineErrorDialog.dialogMessage
                wrapMode: Text.Wrap
                color: "#374151"
                font.pixelSize: 13
                lineHeight: 1.5
                Layout.fillWidth: true
            }
        }
    }

    Dialog {
        id: pipelineFinishDialog
        property bool success: false
        property string dialogMessage: ""
        title: ""
        modal: true
        implicitWidth: 440
        leftPadding: 28
        rightPadding: 24
        topPadding: 24
        bottomPadding: 16

        footer: DialogButtonBox {
            alignment: Qt.AlignRight
            background: Rectangle { color: "transparent" }
            Button {
                text: "确定"
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole
                flat: false
                implicitWidth: 80
                implicitHeight: 34
                background: Rectangle {
                    color: pipelineFinishDialog.success ? "#10B981" : "#EF4444"
                    radius: 6
                }
                contentItem: Text {
                    text: parent.text; color: "#FFFFFF"; font.weight: Font.Medium
                    horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
                }
            }
        }

        background: Rectangle {
            radius: 12
            color: "#FFFFFF"
            border.color: pipelineFinishDialog.success ? "#A7F3D0" : "#FECACA"
            border.width: 1

            Rectangle {
                width: 4
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 1
                color: pipelineFinishDialog.success ? "#10B981" : "#EF4444"
                radius: 2
            }
        }

        contentItem: ColumnLayout {
            spacing: 12

            RowLayout {
                spacing: 10
                Rectangle {
                    width: 32; height: 32
                    radius: 16
                    color: pipelineFinishDialog.success ? "#D1FAE5" : "#FEE2E2"
                    Text {
                        anchors.centerIn: parent
                        text: pipelineFinishDialog.success ? "✓" : "✕"
                        color: pipelineFinishDialog.success ? "#059669" : "#EF4444"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                }
                Text {
                    text: pipelineFinishDialog.success ? "处理完成" : "处理失败"
                    color: pipelineFinishDialog.success ? "#065F46" : "#991B1B"
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
            }

            Text {
                text: pipelineFinishDialog.dialogMessage
                wrapMode: Text.Wrap
                color: "#374151"
                font.pixelSize: 13
                lineHeight: 1.5
                Layout.fillWidth: true
            }
        }
    }
}
