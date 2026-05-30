// /Share/GenerateModel/src/core/HardwareDetector.h
#pragma once

#include <QString>
#include <QJsonObject>

struct HardwareProfile {
    bool hasGPU = false;
    QString gpuName;
    int vramMB = 0;           // 专用显存，0 表示无 GPU
    int cpuCores = 1;
    int ramMB = 0;            // 系统总内存

    QJsonObject toJson() const {
        return {
            {"hasGPU", hasGPU},
            {"gpuName", gpuName},
            {"vramMB", vramMB},
            {"cpuCores", cpuCores},
            {"ramMB", ramMB}
        };
    }
};

struct PipelineStrategy {
    QString quality;           // "low" | "standard" | "high"
    bool useGPU;
    int maxFrameDimension;     // 最大帧边长 (如 1920)
    int denseQuality;          // 1=低, 2=标准, 3=高 (传给 OpenMVS)
};

class HardwareDetector {
public:
    static HardwareProfile detect();

    /// 根据硬件配置 + 用户偏好生成管线策略
    static PipelineStrategy determineStrategy(const HardwareProfile& profile,
                                              bool userGpuEnabled,
                                              const QString& userQuality);
};
