#pragma once
#include <Windows.h>
#include <cassert>
#include <format>
#include <stdexcept>

namespace Tuna {
    class AssertHelper {
    public:
        /// アサートによる条件チェック
        /// <param name="condition">チェックする条件</param>
        /// <param name="message">エラー時のメッセージ</param>
        static inline void Assert([[maybe_unused]] bool condition, [[maybe_unused]] const char *message) {
            assert((condition) && message);
        }

        /// nullptrチェック
        /// <param name="pointer">検証するポインタ</param>
        /// <param name="message">エラー時のメッセージ</param>
        template<typename T>
        static inline void AssertNotNull([[maybe_unused]] T *pointer, [[maybe_unused]] const char *message) {
            assert((pointer != nullptr) && message);
        }

        /// HRESULTの成否チェック
        /// <param name="hr">検証するHRESULT</param>
        /// <param name="message">エラー時のメッセージ</param>
        static inline void AssertHRESULT([[maybe_unused]] HRESULT hr, [[maybe_unused]] const char *message) {
            if (FAILED(hr)) {
                assert(false && message);
            }
        }

        /// runtime_errorによる例外送出
        /// <param name="hr">検証するHRESULT</param>
        /// <param name="message">エラー時のメッセージ</param>
        static inline void ThrowIfFailed([[maybe_unused]] HRESULT hr, [[maybe_unused]] const char *message) {
            if (FAILED(hr)) {
                throw std::runtime_error(
                    std::format("{} (HRESULT: 0x{:08X})", message, static_cast<unsigned int>(hr))
                );
            }
        }

        /// nullptrの場合に例外を送出
        /// <param name="pointer">検証するポインタ</param>
        /// <param name="message">エラー時のメッセージ</param>
        template<typename T>
        static inline void ThrowIfNull([[maybe_unused]] T *pointer, [[maybe_unused]] const char *message) {
            if (pointer == nullptr) {
                throw std::runtime_error(message);
            }
        }
    };
}
