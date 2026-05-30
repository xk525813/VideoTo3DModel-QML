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
    color: "#1e1e2e"

    PipelineBridge {
        id: bridge

        onPipelineError: (title, message) => {
            errorDialog.title = title
            errorDialog.message = message
            errorDialog.open()
        }
        onPipelineFinished: (success, message) => {
            finishDialog.success = success
            finishDialog.message = message
            finishDialog.open()
        }
    }

    // 状态栏
    footer: Rectangle {
        height: 32
        color: "#181825"
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            Text {
                text: "状态: " + bridge.pipelineState
                color: "#cdd6f4"
                font.pixelSize: 12
            }
            Item { Layout.fillWidth: true }
            Text {
                text: "GPU: " + (bridge.hardwareProfile.hasGPU
                       ? bridge.hardwareProfile.gpuName
                       : "未检测到")
                color: bridge.hardwareProfile.hasGPU ? "#a6e3a1" : "#f38ba8"
                font.pixelSize: 12
            }
        }
    }

    // 主布局
    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // 左侧: 导入 + 设置
        ColumnLayout {
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            spacing: 8

            ImportPanel { id: importPanel; bridge: bridge }
            SettingsPanel { id: settingsPanel; bridge: bridge }
        }

        // 中央: 进度面板
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            // 预览占位 (Task 15 替换)
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#11111b"
                border.color: "#313244"
                border.width: 1
                radius: 8

                Text {
                    anchors.centerIn: parent
                    text: "3D 模型预览区域"
                    color: "#585b70"
                    font.pixelSize: 14
                }
            }

            ProgressPanel {
                Layout.fillWidth: true
                Layout.preferredHeight: 120
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
        property string title: ""
        property string message: ""
        title: errorDialog.title
        modal: true
        standardButtons: Dialog.Ok
        Label {
            text: errorDialog.message
            wrapMode: Text.Wrap
            color: "#cdd6f4"
        }
    }

    // 完成弹窗
    Dialog {
        id: finishDialog
        property bool success: false
        property string message: ""
        title: finishDialog.success ? "完成" : "失败"
        modal: true
        standardButtons: Dialog.Ok
        Label {
            text: finishDialog.message
            wrapMode: Text.Wrap
            color: "#cdd6f4"
        }
    }
}
