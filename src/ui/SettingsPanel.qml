import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

Rectangle {
    id: root
    clip: true
    color: "#FFFFFF"
    border.color: "#E2E8F0"
    border.width: 1
    radius: 10

    property var bridge: null

    readonly property bool isRunning: bridge && bridge.pipelineState === "running"

    property alias outputDir: outputDirectoryField.text
    property alias gpuEnabled: gpuToggleSwitch.checked
    property alias qualityChoice: qualityLevelCombo.currentValue
    property alias exportFormat: exportFormatCombo.currentValue

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 14
        spacing: 10

        Text {
            text: "设置"
            color: "#1E293B"
            font.pixelSize: 17
            font.weight: Font.Bold
        }

        RowLayout {
            Text {
                text: "GPU 加速"
                color: "#334155"
                Layout.fillWidth: true
                font.pixelSize: 13
            }
            Switch {
                id: gpuToggleSwitch
                checked: true
                enabled: (root.bridge ? root.bridge.hardwareProfile.hasGPU : false) && !root.isRunning
            }
        }

        Text {
            text: "重建质量"
            color: "#334155"
            font.pixelSize: 13
        }
        ComboBox {
            id: qualityLevelCombo
            Layout.fillWidth: true
            enabled: !root.isRunning
            textRole: "text"
            valueRole: "value"
            model: [
                { text: "低 (快速)", value: "low" },
                { text: "标准", value: "standard" },
                { text: "高 (精细)", value: "high" }
            ]
            currentIndex: 1
        }

        Text {
            text: "导出格式"
            color: "#334155"
            font.pixelSize: 13
        }
        ComboBox {
            id: exportFormatCombo
            Layout.fillWidth: true
            enabled: !root.isRunning
            textRole: "text"
            valueRole: "value"
            model: [
                { text: "GLB (推荐)", value: "glb" },
                { text: "glTF", value: "gltf" },
                { text: "OBJ", value: "obj" }
            ]
        }

        Text {
            text: "输出目录"
            color: "#334155"
            font.pixelSize: 13
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            TextField {
                id: outputDirectoryField
                Layout.fillWidth: true
                enabled: !root.isRunning
                readOnly: root.isRunning
                placeholderText: "默认: 运行目录/projects"
                color: "#1E293B"
                placeholderTextColor: "#94A3B8"
                background: Rectangle {
                    implicitHeight: 32
                    color: "#F1F5F9"
                    radius: 6
                    border.color: outputDirectoryField.activeFocus ? "#6366F1" : "#CBD5E1"
                }
            }

            Button {
                id: browseOutputDirBtn
                text: "..."
                implicitWidth: 36
                implicitHeight: 32
                enabled: !root.isRunning
                flat: false
                onClicked: outputDirDialog.open()

                background: Rectangle {
                    color: browseOutputDirBtn.hovered ? "#E2E8F0" : "#F1F5F9"
                    border.color: "#CBD5E1"
                    radius: 6
                }
                contentItem: Text {
                    text: browseOutputDirBtn.text
                    color: "#475569"
                    font.weight: Font.Bold
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        FolderDialog {
            id: outputDirDialog
            title: "选择输出目录"
            currentFolder: outputDirectoryField.text
                ? "file://" + outputDirectoryField.text
                : StandardPaths.writableLocation(StandardPaths.HomeLocation)
            onAccepted: {
                let path = selectedFolder.toString()
                if (path.startsWith("file://"))
                    path = path.substring(7)
                outputDirectoryField.text = path
            }
        }
    }
}
