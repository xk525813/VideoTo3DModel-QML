// /Share/GenerateModel/src/core/LogManager.h
#pragma once

#include <QString>
#include <QDir>
#include <memory>
#include <spdlog/spdlog.h>

class LogManager {
public:
    static void init(const QString& logDir);
    static void shutdown();

    // ── 带源码位置（原生 spdlog::source_loc）────────────────
    //
    // 用法:
    //   LogManager::info (spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__}, "msg={}", val);
    //   LogManager::error(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__}, "err={}", e);

    template<typename... Args>
    static void trace(spdlog::source_loc loc, const char* fmt, Args&&... args);
    template<typename... Args>
    static void debug(spdlog::source_loc loc, const char* fmt, Args&&... args);
    template<typename... Args>
    static void info (spdlog::source_loc loc, const char* fmt, Args&&... args);
    template<typename... Args>
    static void warn (spdlog::source_loc loc, const char* fmt, Args&&... args);
    template<typename... Args>
    static void error(spdlog::source_loc loc, const char* fmt, Args&&... args);
    template<typename... Args>
    static void critical(spdlog::source_loc loc, const char* fmt, Args&&... args);

    // ── 格式字符串（无源码位置，简单调用用）─────────────────

    template<typename... Args>
    static void trace(const char* fmt, Args&&... args);
    template<typename... Args>
    static void debug(const char* fmt, Args&&... args);
    template<typename... Args>
    static void info (const char* fmt, Args&&... args);
    template<typename... Args>
    static void warn (const char* fmt, Args&&... args);
    template<typename... Args>
    static void error(const char* fmt, Args&&... args);
    template<typename... Args>
    static void critical(const char* fmt, Args&&... args);

    // ── 简单字符串（无源码位置）─────────────────────────────

    static void trace(const QString& msg);
    static void debug(const QString& msg);
    static void info (const QString& msg);
    static void warn (const QString& msg);
    static void error(const QString& msg);
    static void critical(const QString& msg);

    /// Qt 消息桥接
    static void qtMessageHandler(QtMsgType type,
                                 const QMessageLogContext& context,
                                 const QString& msg);

    static std::shared_ptr<spdlog::logger> s_logger;
};

// ─── 模板实现 ─────────────────────────────────────────────

template<typename... Args>
void LogManager::trace(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::trace, fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::debug(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::debug, fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::info(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::info, fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::warn(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::warn, fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::error(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::err, fmt, std::forward<Args>(args)...);
}
template<typename... Args>
void LogManager::critical(spdlog::source_loc loc, const char* fmt, Args&&... args) {
    if (s_logger) s_logger->log(loc, spdlog::level::critical, fmt, std::forward<Args>(args)...);
}

// ─── 无源码位置的格式重载 ─────────────────────────

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
