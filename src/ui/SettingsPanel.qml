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

    // 暴露给 ImportPanel 读取的设置值
    property alias outputDir: outputDirField.text
    property alias gpuEnabled: gpuSwitch.checked
    property alias qualityChoice: qualityCombo.currentValue
    property alias exportFormat: formatCombo.currentValue

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
                id: gpuSwitch
                checked: true
                enabled: bridge ? bridge.hardwareProfile.hasGPU : false
            }
        }

        Text {
            text: "重建质量"
            color: "#334155"
            font.pixelSize: 13
        }
        ComboBox {
            id: qualityCombo
            Layout.fillWidth: true
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
            id: formatCombo
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: [
                { text: "GLB (推荐)", value: "glb" },
                { text: "glTF", value: "gltf" },
                { text: "OBJ", value: "obj" }
            ]
        }

        Text {
            text: "导出目录"
            color: "#334155"
            font.pixelSize: 13
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 6

            TextField {
                id: outputDirField
                Layout.fillWidth: true
                placeholderText: "默认: ~/VideoTo3D_Projects"
                color: "#1E293B"
                placeholderTextColor: "#94A3B8"
                background: Rectangle {
                    implicitHeight: 32
                    color: "#F1F5F9"
                    radius: 6
                    border.color: outputDirField.activeFocus ? "#6366F1" : "#CBD5E1"
                }
            }

            Button {
                id: browseDirBtn
                text: "..."
                implicitWidth: 36
                implicitHeight: 32
                flat: false
                onClicked: folderDialog.open()

                background: Rectangle {
                    color: browseDirBtn.hovered ? "#E2E8F0" : "#F1F5F9"
                    border.color: "#CBD5E1"
                    radius: 6
                }
                contentItem: Text {
                    text: browseDirBtn.text
                    color: "#475569"
                    font.weight: Font.Bold
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        FolderDialog {
            id: folderDialog
            title: "选择输出目录"
            currentFolder: outputDirField.text
                ? "file://" + outputDirField.text
                : StandardPaths.writableLocation(StandardPaths.HomeLocation)
            onAccepted: {
                let path = selectedFolder.toString()
                if (path.startsWith("file://"))
                    path = path.substring(7)
                outputDirField.text = path
            }
        }
    }
}
