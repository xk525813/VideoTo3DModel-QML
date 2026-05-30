import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import video3d.pipeline 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 1280
    height: 800
    title: "VideoTo3D"
    color: "#F0F4F8"

    PipelineBridge {
        id: bridge

        onPipelineError: (title, message) => {
            errorDialog.dialogTitle = title
            errorDialog.dialogMessage = message
            errorDialog.open()
        }
        onPipelineFinished: (success, message) => {
            finishDialog.success = success
            finishDialog.dialogMessage = message
            finishDialog.open()
        }
    }

    // 状态栏
    footer: Rectangle {
        height: 32
        color: "#E2E8F0"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            Text {
                text: "状态: " + bridge.pipelineState
                color: "#334155"
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }
            Text {
                text: "GPU: " + (bridge.hardwareProfile.hasGPU
                       ? bridge.hardwareProfile.gpuName
                       : "未检测到")
                color: bridge.hardwareProfile.hasGPU ? "#059669" : "#DC2626"
                font.pixelSize: 12
            }
        }
    }

    // 主布局
    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        // 左侧: 导入 + 设置
        ColumnLayout {
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            spacing: 10

            ImportPanel { id: importPanel; bridge: bridge }
            SettingsPanel { id: settingsPanel; bridge: bridge }
        }

        // 中央: 预览 + 进度
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 10

            PreviewPanel {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            ProgressPanel {
                Layout.fillWidth: true
                Layout.preferredHeight: 130
                bridge: bridge
            }
        }

        // 右侧: 导出
        ExportPanel {
            Layout.preferredWidth: 240
            Layout.fillHeight: true
            bridge: bridge
        }
    }

    // 错误弹窗
    Dialog {
        id: errorDialog
        property string dialogTitle: ""
        property string dialogMessage: ""
        title: errorDialog.dialogTitle
        modal: true
        standardButtons: Dialog.Ok
        implicitWidth: 420
        Label {
            text: errorDialog.dialogMessage
            wrapMode: Text.Wrap
            color: "#1E293B"
        }
    }

    // 完成弹窗
    Dialog {
        id: finishDialog
        property bool success: false
        property string dialogMessage: ""
        title: finishDialog.success ? "完成" : "失败"
        modal: true
        standardButtons: Dialog.Ok
        implicitWidth: 380
        Label {
            text: finishDialog.dialogMessage
            wrapMode: Text.Wrap
            color: "#1E293B"
        }
    }
}
