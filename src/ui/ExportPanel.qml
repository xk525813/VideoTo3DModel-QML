import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root
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
            text: "导出"
            color: "#1E293B"
            font.pixelSize: 17
            font.weight: Font.Bold
        }

        Text {
            text: "贴图通道"
            color: "#334155"
            font.pixelSize: 13
            font.weight: Font.Medium
        }
        CheckBox {
            text: "漫反射 (Diffuse)"
            checked: true
            enabled: false
        }
        CheckBox {
            text: "法线 (Normal)"
            checked: false
            enabled: false
        }
        CheckBox {
            text: "粗糙度 (Roughness)"
            checked: false
            enabled: false
        }
        CheckBox {
            text: "金属度 (Metallic)"
            checked: false
            enabled: false
        }

        Item { Layout.fillHeight: true }

        Button {
            id: checkDependenciesBtn
            text: "检查环境依赖"
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            flat: false
            onClicked: {
                if (root.bridge) {
                    let result = root.bridge.checkDependencies()
                    let msg = ""
                    for (let key in result) {
                        msg += key + ": " + (result[key] ? "已安装" : "未安装") + "\n"
                    }
                    dependencyCheckDialog.message = msg
                    dependencyCheckDialog.open()
                }
            }

            background: Rectangle {
                color: checkDependenciesBtn.hovered ? "#E2E8F0" : "#F1F5F9"
                border.color: "#CBD5E1"
                radius: 6
            }
            contentItem: Text {
                text: checkDependenciesBtn.text
                color: "#475569"
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }

        Button {
            id: openOutputDirBtn
            text: "打开输出目录"
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            enabled: root.bridge && root.bridge.pipelineState === "completed"

            background: Rectangle {
                color: openOutputDirBtn.enabled
                       ? (openOutputDirBtn.hovered ? "#4F46E5" : "#6366F1")
                       : "#CBD5E1"
                radius: 6
            }
            contentItem: Text {
                text: openOutputDirBtn.text
                color: openOutputDirBtn.enabled ? "#FFFFFF" : "#94A3B8"
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Dialog {
        id: dependencyCheckDialog
        property string message: ""
        title: ""
        modal: true
        implicitWidth: 440
        padding: 0
        standardButtons: Dialog.Ok

        background: Rectangle {
            radius: 12
            color: "#FFFFFF"
            border.color: "#C7D2FE"
            border.width: 1

            Rectangle {
                width: 4
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 1
                color: "#6366F1"
                radius: 2
            }
        }

        contentItem: ColumnLayout {
            spacing: 12
            topPadding: 24
            bottomPadding: 16
            leftPadding: 28
            rightPadding: 24

            RowLayout {
                spacing: 10
                Rectangle {
                    width: 32; height: 32
                    radius: 16
                    color: "#E0E7FF"
                    Text {
                        anchors.centerIn: parent
                        text: "?"
                        color: "#4F46E5"
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                }
                Text {
                    text: "环境依赖检查"
                    color: "#312E81"
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
            }

            ScrollView {
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(implicitHeight, 260)
                Text {
                    text: dependencyCheckDialog.message
                    color: "#374151"
                    font.family: "monospace"
                    font.pixelSize: 12
                    lineHeight: 1.7
                }
            }
        }
    }
}
