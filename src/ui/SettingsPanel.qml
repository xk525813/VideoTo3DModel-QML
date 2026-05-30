import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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
            text: "设置"
            color: "#cdd6f4"
            font.pixelSize: 16
            font.weight: Font.Bold
        }

        // GPU 开关
        RowLayout {
            Text { text: "GPU 加速"; color: "#cdd6f4"; Layout.fillWidth: true }
            Switch {
                id: gpuSwitch
                checked: true
                enabled: bridge ? bridge.hardwareProfile.hasGPU : false
            }
        }

        // 质量档位
        Text { text: "重建质量"; color: "#cdd6f4" }
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

        // 导出格式
        Text { text: "导出格式"; color: "#cdd6f4" }
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

        // 输出目录
        Text { text: "输出目录"; color: "#cdd6f4" }
        TextField {
            id: outputDirField
            Layout.fillWidth: true
            text: ""
            placeholderText: "默认: ~/VideoTo3D_Projects"
            color: "#cdd6f4"
            background: Rectangle {
                color: "#313244"
                radius: 4
            }
        }
    }
}
