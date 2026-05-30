// /Share/GenerateModel/tests/test_hardware_detector.cpp
#include <QtTest>
#include "HardwareDetector.h"

class TestHardwareDetector : public QObject {
    Q_OBJECT

private slots:
    void test_detect_returns_valid_profile() {
        HardwareProfile p = HardwareDetector::detect();

        QVERIFY(p.cpuCores > 0);
        QVERIFY(p.ramMB > 0);
        // GPU 可选 — 无 GPU 时 hasGPU=false, vramMB=0
        if (!p.hasGPU) {
            QCOMPARE(p.vramMB, 0);
        } else {
            QVERIFY(p.vramMB > 0);
            QVERIFY(!p.gpuName.isEmpty());
        }
    }

    void test_strategy_high_gpu_enabled_with_gpu() {
        HardwareProfile p;
        p.hasGPU = true;
        p.gpuName = "NVIDIA RTX 4090";
        p.vramMB = 24000;
        p.cpuCores = 16;
        p.ramMB = 64000;

        PipelineStrategy s = HardwareDetector::determineStrategy(p, true, "high");

        QVERIFY(s.useGPU);
        QCOMPARE(s.maxFrameDimension, 3840);
        QCOMPARE(s.denseQuality, 3);
    }

    void test_strategy_low_vram_falls_back_cpu() {
        HardwareProfile p;
        p.hasGPU = true;
        p.gpuName = "NVIDIA GT 710";
        p.vramMB = 512;
        p.cpuCores = 4;
        p.ramMB = 8000;

        PipelineStrategy s = HardwareDetector::determineStrategy(p, true, "standard");

        // vram < 1000 → GPU 回退
        QVERIFY(!s.useGPU);
        // CPU 模式下应自动降级
        QCOMPARE(s.maxFrameDimension, 1920);
    }

    void test_user_disables_gpu() {
        HardwareProfile p;
        p.hasGPU = true;
        p.gpuName = "NVIDIA RTX 4090";
        p.vramMB = 24000;
        p.cpuCores = 16;
        p.ramMB = 64000;

        PipelineStrategy s = HardwareDetector::determineStrategy(p, false, "high");

        // 用户禁用 GPU → 不使用
        QVERIFY(!s.useGPU);
    }

    void test_low_quality_settings() {
        HardwareProfile p;
        p.hasGPU = false;
        p.vramMB = 0;
        p.cpuCores = 4;
        p.ramMB = 8000;

        PipelineStrategy s = HardwareDetector::determineStrategy(p, false, "low");

        QVERIFY(!s.useGPU);
        QCOMPARE(s.maxFrameDimension, 1280);
        QCOMPARE(s.denseQuality, 1);
    }
};

QTEST_MAIN(TestHardwareDetector)
#include "test_hardware_detector.moc"
