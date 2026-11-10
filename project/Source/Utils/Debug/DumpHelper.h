#pragma once
#include <Windows.h>

namespace Tuna {
    class DumpHelper {
    public:
        // インスタンス生成禁止
        DumpHelper() = delete;
        ~DumpHelper() = delete;

        // 例外ハンドラを登録する
        static void Initialize();

    private:
        // MiniDumpを生成する
        // <param name="exception">発生した例外の情報</param>
        static LONG WINAPI ExportDump(EXCEPTION_POINTERS *exception);
    };
}