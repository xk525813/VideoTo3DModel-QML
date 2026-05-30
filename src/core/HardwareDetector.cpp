// /Share/GenerateModel/src/core/HardwareDetector.cpp
#include "HardwareDetector.h"

#ifdef Q_OS_LINUX
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QDebug>
#include <unistd.h>
#endif

HardwareProfile HardwareDetector::detect() {
    HardwareProfile p;

#ifdef Q_OS_LINUX
    // 检测 CPU 核心数
    p.cpuCores = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));

    // 检测系统内存
    long pages = sysconf(_SC_PHYS_PAGES);
    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pages > 0 && pageSize > 0)
        p.ramMB = static_cast<int>((pages * pageSize) / (1024 * 1024));

    // 检测 NVIDIA GPU
    QProcess nvidia;
    nvidia.start("nvidia-smi", {"--query-gpu=name,memory.total",
                                  "--format=csv,noheader,nounits"});
    nvidia.waitForFinished(5000);
    if (nvidia.exitCode() == 0) {
        QString output = nvidia.readAllStandardOutput().trimmed();
        QStringList parts = output.split(",");
        if (parts.size() >= 2) {
            p.hasGPU = true;
            p.gpuName = parts[0].trimmed();
            p.vramMB = parts[1].trimmed().toInt();
        }
    }
#endif

    return p;
}

PipelineStrategy HardwareDetector::determineStrategy(
    const HardwareProfile& profile,
    bool userGpuEnabled,
    const QString& userQuality)
{
    PipelineStrategy s;

    // 基础质量设定
    if (userQuality == "low") {
        s.maxFrameDimension = 1280;
        s.denseQuality = 1;
    } else if (userQuality == "high") {
        s.maxFrameDimension = 3840;
        s.denseQuality = 3;
    } else {
        s.maxFrameDimension = 1920;
        s.denseQuality = 2;
    }
    s.quality = userQuality;

    // GPU 启用判断
    s.useGPU = userGpuEnabled && profile.hasGPU;

    // GPU 降级阶梯 (4级)
    if (s.useGPU && profile.vramMB < 4000 && s.maxFrameDimension > 1920) {
        // Level 1: 显存不足 → 降分辨率
        s.maxFrameDimension = 1920;
    }
    if (s.useGPU && profile.vramMB < 2000) {
        // Level 2: 显存仍不足 → 降 MVS 质量
        s.denseQuality = qMin(s.denseQuality, 1);
    }
    if (s.useGPU && profile.vramMB < 1000) {
        // Level 3: 极低显存 → 回退 CPU
        s.useGPU = false;
    }

    // CPU 模式下的保守参数
    if (!s.useGPU) {
        s.maxFrameDimension = qMin(s.maxFrameDimension, 1920);
        s.denseQuality = qMin(s.denseQuality, 1);
    }

    return s;
}
