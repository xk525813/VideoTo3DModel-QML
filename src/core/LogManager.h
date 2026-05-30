// /Share/GenerateModel/src/core/LogManager.h
#pragma once

#include <QString>
#include <QDir>
#include <memory>

// 前向声明 spdlog 类型，避免头文件污染
namespace spdlog {
class logger;
}

class LogManager {
public:
    /// 初始化日志系统（应用启动时调用一次）
    /// @param logDir  日志文件存放目录
    static void init(const QString& logDir);

    /// 关闭日志系统（应用退出前调用）
    static void shutdown();

    // ── 等级日志（带格式字符串）───────────────────────────
    // 用法: LogManager::info("管线启动, project={}", dir.toStdString());

    template<typename... Args>
    static void trace(const char* fmt, Args&&... args);
    template<typename... Args>
    static void debug(const char* fmt, Args&&... args);
    template<typename... Args>
    static void info(const char* fmt, Args&&... args);
    template<typename... Args>
    static void warn(const char* fmt, Args&&... args);
    template<typename... Args>
    static void error(const char* fmt, Args&&... args);
    template<typename... Args>
    static void critical(const char* fmt, Args&&... args);

    // ── 等级日志（纯字符串，无格式化）─────────────────────

    static void trace(const QString& msg);
    static void debug(const QString& msg);
    static void info(const QString& msg);
    static void warn(const QString& msg);
    static void error(const QString& msg);
    static void critical(const QString& msg);

    /// Qt 消息处理器（桥接 qDebug/qWarning 到 spdlog）
    static void qtMessageHandler(QtMsgType type,
                                  const QMessageLogContext& context,
                                  const QString& msg);

    static std::shared_ptr<spdlog::logger> s_logger;
};

// ─── 模板实现 ─────────────────────────────────────────────

#include <spdlog/spdlog.h>

template<typename... Args>
void LogManager::trace(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->trace(fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::debug(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->debug(fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::info(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->info(fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::warn(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->warn(fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::error(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->error(fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::critical(const char* fmt, Args&&... args) {
    if (s_logger) s_logger->critical(fmt, std::forward<Args>(args)...);
}
