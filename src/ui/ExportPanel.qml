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
            text: "导出"
            color: "#cdd6f4"
            font.pixelSize: 16
            font.weight: Font.Bold
        }

        // 贴图通道选择 (阶段 1 仅为 UI 占位)
        Text { text: "贴图通道"; color: "#cdd6f4" }
        CheckBox { text: "漫反射 (Diffuse)"; checked: true; enabled: false }
        CheckBox { text: "法线 (Normal)"; checked: false }
        CheckBox { text: "粗糙度 (Roughness)"; checked: false }
        CheckBox { text: "金属度 (Metallic)"; checked: false }

        Item { Layout.fillHeight: true }

        // 依赖检查
        Button {
            text: "检查环境依赖"
            Layout.fillWidth: true
            onClicked: {
                if (bridge) {
                    let result = bridge.checkDependencies()
                    let msg = ""
                    for (let key in result) {
                        msg += key + ": " + (result[key] ? "已安装" : "未安装") + "\n"
                    }
                    depDialog.message = msg
                    depDialog.open()
                }
            }
        }

        Button {
            text: "打开输出目录"
            Layout.fillWidth: true
            enabled: bridge && bridge.pipelineState === "completed"
        }
    }

    Dialog {
        id: depDialog
        property string message: ""
        title: "环境依赖检查"
        modal: true
        standardButtons: Dialog.Ok
        Label {
            text: depDialog.message
            color: "#cdd6f4"
            font.family: "monospace"
            font.pixelSize: 12
        }
    }
}
