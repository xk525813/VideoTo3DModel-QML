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

    // 主布局 — 占位: 后续任务替换为实际面板
    Rectangle {
        anchors.fill: parent
        anchors.margins: 16
        color: "transparent"

        ColumnLayout {
            anchors.centerIn: parent
            spacing: 12

            Text {
                text: "VideoTo3D"
                color: "#cdd6f4"
                font.pixelSize: 28
                font.weight: Font.Bold
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: "视频转 3D 模型工具 v" + _appVersion
                color: "#6c7086"
                font.pixelSize: 14
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: bridge.pipelineState === "idle" ? "就绪 — 拖放视频开始" : bridge.pipelineState
                color: "#a6adc8"
                font.pixelSize: 13
                Layout.alignment: Qt.AlignHCenter
            }
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
