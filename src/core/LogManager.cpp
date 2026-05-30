// /Share/GenerateModel/src/core/LogManager.cpp
#include "LogManager.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/pattern_formatter.h>

#include <QDebug>
#include <QDir>

std::shared_ptr<spdlog::logger> LogManager::s_logger;

// ─── 自定义 %* 标志：等级名补 '#' 到固定宽度 ──────────────────

class LevelPaddedFlag : public spdlog::custom_flag_formatter {
public:
    void format(const spdlog::details::log_msg& msg,
                const std::tm&, spdlog::memory_buf_t& dest) override
    {
        static const char* names[] = {"TRACE", "DEBUG", "INFO ",
                                       "WARN ", "ERROR", "CRIT "};
        int idx = static_cast<int>(msg.level);
        if (idx < 0 || idx > 5) idx = 2;
        dest.append(names[idx], names[idx] + 5);
    }

    std::unique_ptr<custom_flag_formatter> clone() const override
    {
        return spdlog::details::make_unique<LevelPaddedFlag>();
    }
};

// ─── 初始化 ─────────────────────────────────────────────────

void LogManager::init(const QString& logDir)
{
    QDir().mkpath(logDir);
    const QString logFile = logDir + "/app.log";

    try {
        // ── 控制台 sink（带颜色）─────────────────────
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::trace);

        // ── 文件 sink（自动轮转 5MB×3）────────────────
        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            logFile.toStdString(), 5 * 1024 * 1024, 3);
        fileSink->set_level(spdlog::level::trace);

        // ── 创建日志器 ──────────────────────────────
        spdlog::sinks_init_list sinks = {consoleSink, fileSink};
        s_logger = std::make_shared<spdlog::logger>("VideoTo3D", sinks);
        s_logger->set_level(spdlog::level::trace);

        // ── 注册自定义 %* 标志并设置格式 ──────────────
        // 格式: [yyyy-MM-dd HH:mm:ss][文件名 函数名:行号][LEVEL#] 消息
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<LevelPaddedFlag>('*');
        formatter->set_pattern("[%Y-%m-%d %H:%M:%S][%s %!:%#][%*] %v");
        s_logger->set_formatter(std::move(formatter));

        spdlog::set_default_logger(s_logger);
        qInstallMessageHandler(qtMessageHandler);

        s_logger->info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        s_logger->info("LogManager 初始化完成，日志文件: {}", logFile.toStdString());
        s_logger->info("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");

    } catch (const spdlog::spdlog_ex& ex) {
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
void LogManager::info (const QString& msg)
    { if (s_logger) s_logger->info(msg.toStdString()); }
void LogManager::warn (const QString& msg)
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

    QByteArray local = msg.toLocal8Bit();
    spdlog::source_loc loc{
        context.file     ? context.file     : __FILE__,
        context.line     ? context.line     : __LINE__,
        context.function ? context.function : __FUNCTION__
    };

    switch (type) {
    case QtDebugMsg:    s_logger->log(loc, spdlog::level::debug,    "[Qt] {}", local.constData()); break;
    case QtInfoMsg:     s_logger->log(loc, spdlog::level::info,     "[Qt] {}", local.constData()); break;
    case QtWarningMsg:  s_logger->log(loc, spdlog::level::warn,     "[Qt] {}", local.constData()); break;
    case QtCriticalMsg: s_logger->log(loc, spdlog::level::err,      "[Qt] {}", local.constData()); break;
    case QtFatalMsg:    s_logger->log(loc, spdlog::level::critical, "[Qt] {}", local.constData()); break;
    }
}
