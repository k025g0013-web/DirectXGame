#include "Logger.h"
#include "DumpHelper.h"

#include <Windows.h>

#include <filesystem>
#include <chrono>
#include <format>
#include <string>

namespace Tuna {
	// staticメンバー変数の定義
	std::ofstream Logger::stream_;
	std::mutex Logger::mutex_;

	void Logger::Initialize() {
		// ログファイルを生成する
		CreateLogFile();

		// クラッシュ時にMiniDumpを生成できるよう例外ハンドラを登録する
		DumpHelper::Initialize();
	}

	void Logger::Finalize() {
		// ログ出力時の排他制御
		std::lock_guard<std::mutex> lock(mutex_);

		if (stream_.is_open()) {
			// ログファイルを閉じる
			stream_.close();
		}
	}

	void Logger::Log(std::string_view message, LogLevel level) {
		// ログレベルを含めた出力文字列を作成する
		std::string logMessage = std::format("[{}] {}\n", GetLogLevelString(level), message);

		// ログ出力時の排他制御
		std::lock_guard<std::mutex> lock(mutex_);

		// ログファイルへ書き込む
		if (stream_.is_open()) {
			stream_ << logMessage;
		}

		// デバッグ出力ウィンドウにも表示する
		OutputDebugStringA(logMessage.c_str());
	}

    void Logger::CreateLogFile() {
        // ログ出力時の排他制御
        std::lock_guard<std::mutex> lock(mutex_);

        // ログ保存用ディレクトリを作成する
        std::filesystem::create_directories("logs");

        // 現在時刻を取得する
        auto now = std::chrono::system_clock::now();

        // 秒単位に変換する
        auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);

        // ローカルタイムへ変換する
        std::chrono::zoned_time localTime{
            std::chrono::current_zone(),
            nowSeconds
        };

        // 「YYYYMMDD_HHMMSS.log」の形式でファイル名を作成する
        std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
        std::string logFilePath = "logs/" + dateString + ".log";

        // ログファイルを開く
        stream_.open(logFilePath, std::ios::out);
    }

    const char *Logger::GetLogLevelString(LogLevel level) {
        switch (level) {
        case LogLevel::Info:
            return "INFO";
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        default:
            return "UNKNOWN";
        }
    }
}