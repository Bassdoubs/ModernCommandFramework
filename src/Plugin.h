#pragma once
// Content-based guard in addition to #pragma once: with the clang-cl PCH the
// header is reached both through the precompiled blob and a -FIPCH.h textual
// parse under two different path identities (src/PCH.h vs the generated copy),
// which defeats #pragma once and double-defines these. The macro guard dedups
// regardless of path.
#ifndef MCF_PLUGIN_H
#define MCF_PLUGIN_H
#include <string_view>

using namespace std::literals;

namespace Plugin {
    inline constexpr auto NAME = "ModernCommandFramework"sv;
    // Log shows "v5.1.7"; build is 5.1.7 (exec-thunk name resolve, ExecuteHook OFF).
    inline constexpr auto Version = 5ULL;
}
#endif  // MCF_PLUGIN_H