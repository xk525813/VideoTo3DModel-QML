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
        standardButtons: Dialog.Ok
        implicitWidth: 420
        Label {
            text: pipelineErrorDialog.dialogMessage
            wrapMode: Text.Wrap
            color: "#1E293B"
        }
    }

    Dialog {
        id: pipelineFinishDialog
        property bool success: false
        property string dialogMessage: ""
        title: pipelineFinishDialog.success ? "完成" : "失败"
        modal: true
        standardButtons: Dialog.Ok
        implicitWidth: 380
        Label {
            text: pipelineFinishDialog.dialogMessage
            wrapMode: Text.Wrap
            color: "#1E293B"
        }
    }
}
