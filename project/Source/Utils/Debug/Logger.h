#pragma once

#include <fstream>
#include <mutex>
#include <string_view>

namespace Tuna {

    /// ログの種類
    enum class LogLevel {
        Info,       // 通常の情報
        Warning,    // 警告
        Error       // エラー
    };

    class Logger {
    public:
        // インスタンス生成禁止
        Logger() = delete;  // インストラクタ
        ~Logger() = delete; // デストラクタ
        
        // コピー禁止
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        // 初期化
        static void Initialize();
        // 終了
        static void Finalize();

        // ログを出力する
        // <param name="message">出力する文字列</param>
        // <param name="level">ログの種類</param>
        static void Log(std::string_view message, LogLevel level = LogLevel::Info);

        // ログファイルを生成する
        static void CreateLogFile();

    private:
        // ログレベルを文字列に変換する
        static const char *GetLogLevelString(LogLevel level);

    private:
        /* ログ管理
        --------------------*/
        // ログファイル出力ストリーム
        static std::ofstream stream_;
        // ログ出力時の排他制御
        static std::mutex mutex_;
    };
}
