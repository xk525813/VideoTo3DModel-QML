// /Share/GenerateModel/src/core/LogManager.cpp
#include "LogManager.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <QDebug>
#include <QDateTime>
#include <QFileInfo>

std::shared_ptr<spdlog::logger> LogManager::s_logger;

void LogManager::init(const QString& logDir)
{
    // 确保日志目录存在
    QDir().mkpath(logDir);

    const QString logFile = logDir + "/app.log";

    try {
        // 控制台 sink（带颜色）
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::trace);
        consoleSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        // 文件 sink（自动轮转：5MB × 3 个文件）
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile.toStdString(), 5 * 1024 * 1024, 3);
        fileSink->set_level(spdlog::level::trace);
        fileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

        // 组合 sink
        spdlog::sinks_init_list sinks = {consoleSink, fileSink};
        s_logger = std::make_shared<spdlog::logger>("VideoTo3D", sinks);
        s_logger->set_level(spdlog::level::trace);

        // 注册为默认日志器，以便全局使用
        spdlog::set_default_logger(s_logger);

        // 接管 Qt 的日志输出
        qInstallMessageHandler(qtMessageHandler);

        s_logger->info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        s_logger->info("LogManager 初始化完成");
        s_logger->info("  日志文件: {}", logFile.toStdString());
        s_logger->info("  控制台级别: trace");
        s_logger->info("  文件级别:   trace");
        s_logger->info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    }
    catch (const spdlog::spdlog_ex& ex) {
        qCritical() << "LogManager 初始化失败:" << ex.what();
    }
}

void LogManager::shutdown()
{
    if (s_logger) {
        s_logger->info("LogManager 关闭");
        s_logger->flush();
        spdlog::drop_all();
        s_logger.reset();
    }
}

// ─── QString 重载 ─────────────────────────────────────────────

void LogManager::trace(const QString& msg)
    { if (s_logger) s_logger->trace(msg.toStdString()); }
void LogManager::debug(const QString& msg)
    { if (s_logger) s_logger->debug(msg.toStdString()); }
void LogManager::info(const QString& msg)
    { if (s_logger) s_logger->info(msg.toStdString()); }
void LogManager::warn(const QString& msg)
    { if (s_logger) s_logger->warn(msg.toStdString()); }
void LogManager::error(const QString& msg)
    { if (s_logger) s_logger->error(msg.toStdString()); }
void LogManager::critical(const QString& msg)
    { if (s_logger) s_logger->critical(msg.toStdString()); }

// ─── Qt 消息桥接 ─────────────────────────────────────────────

void LogManager::qtMessageHandler(QtMsgType type,
                                   const QMessageLogContext& context,
                                   const QString& msg)
{
    if (!s_logger) return;

    QByteArray localMsg = msg.toLocal8Bit();
    const char* file = context.file ? context.file : "";
    const char* function = context.function ? context.function : "";

    switch (type) {
    case QtDebugMsg:
        s_logger->debug("[Qt] {}  ({}, {})", localMsg.constData(), file, context.line);
        break;
    case QtInfoMsg:
        s_logger->info("[Qt] {}", localMsg.constData());
        break;
    case QtWarningMsg:
        s_logger->warn("[Qt] {}  ({}, {})", localMsg.constData(), file, context.line);
        break;
    case QtCriticalMsg:
        s_logger->error("[Qt] {}  ({}, {})", localMsg.constData(), file, context.line);
        break;
    case QtFatalMsg:
        s_logger->critical("[Qt] FATAL: {}  ({}, {})", localMsg.constData(), file, context.line);
        break;
    }
}
