#include "DumpHelper.h"

#pragma comment(lib, "Dbghelp.lib")

#include <DbgHelp.h>
#include <strsafe.h>

namespace Tuna {
    void DumpHelper::Initialize() {
        // クラッシュ時にMiniDumpを生成する例外ハンドラを登録する
        SetUnhandledExceptionFilter(&DumpHelper::ExportDump);
    }

    LONG WINAPI DumpHelper::ExportDump(EXCEPTION_POINTERS *exception) {
        // Dumpファイル保存用ディレクトリを作成する
        CreateDirectoryW(L"./Dumps", nullptr);

        // 現在時刻を取得し、Dumpファイル名に利用する
        SYSTEMTIME time;
        GetLocalTime(&time);
        wchar_t filePath[MAX_PATH] = { 0 };

        // 日時を含むDumpファイル名を生成する
        StringCchPrintfW(
            filePath, MAX_PATH, L"./Dumps/%04u-%02u%02u-%02u%02u%02u.dmp",
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

        // Dump出力に必要な例外情報を設定する
        HANDLE dumpFileHandle = CreateFileW(
            filePath, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);

        if (dumpFileHandle != INVALID_HANDLE_VALUE) {
            // Dump出力に必要な例外情報を設定する
            MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{};

            minidumpInformation.ThreadId = GetCurrentThreadId();
            minidumpInformation.ExceptionPointers = exception;
            minidumpInformation.ClientPointers = TRUE;

            // クラッシュ解析用のMiniDumpを出力する
            MiniDumpWriteDump(
                GetCurrentProcess(), GetCurrentProcessId(),
                dumpFileHandle, MiniDumpNormal,
                &minidumpInformation, nullptr, nullptr);

            // ファイルハンドルを閉じる
            CloseHandle(dumpFileHandle);
        }

        // 例外を処理したものとして、プロセスを終了させる
        return EXCEPTION_EXECUTE_HANDLER;
    }
}
#include "DumpHelper.h"

#pragma comment(lib, "Dbghelp.lib")

#include <DbgHelp.h>
#include <strsafe.h>

namespace Tuna {
    void DumpHelper::Initialize() {
        // クラッシュ時にMiniDumpを生成する例外ハンドラを登録する
        SetUnhandledExceptionFilter(&DumpHelper::ExportDump);
    }

    LONG WINAPI DumpHelper::ExportDump(EXCEPTION_POINTERS *exception) {
        // Dumpファイル保存用ディレクトリを作成する
        CreateDirectoryW(L"./Dumps", nullptr);

        // 現在時刻を取得し、Dumpファイル名に利用する
        SYSTEMTIME time;
        GetLocalTime(&time);
        wchar_t filePath[MAX_PATH] = { 0 };

        // 日時を含むDumpファイル名を生成する
        StringCchPrintfW(
            filePath, MAX_PATH, L"./Dumps/%04u-%02u%02u-%02u%02u%02u.dmp",
            time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);

        // Dump出力に必要な例外情報を設定する
        HANDLE dumpFileHandle = CreateFileW(
            filePath, GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ, nullptr, CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, nullptr);

        if (dumpFileHandle != INVALID_HANDLE_VALUE) {
            // Dump出力に必要な例外情報を設定する
            MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{};

            minidumpInformation.ThreadId = GetCurrentThreadId();
            minidumpInformation.ExceptionPointers = exception;
            minidumpInformation.ClientPointers = TRUE;

            // クラッシュ解析用のMiniDumpを出力する
            MiniDumpWriteDump(
                GetCurrentProcess(), GetCurrentProcessId(),
                dumpFileHandle, MiniDumpNormal,
                &minidumpInformation, nullptr, nullptr);

            // ファイルハンドルを閉じる
            CloseHandle(dumpFileHandle);
        }

        // 例外を処理したものとして、プロセスを終了させる
        return EXCEPTION_EXECUTE_HANDLER;
    }
}
