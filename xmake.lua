-- Modern Command Framework (MCF) - xmake build configuration
-- Cross-compiled on Linux via clang-cl + lld-link + llvm-rc against the xwin
-- Windows SDK, built on libxse's CommonLibSF (runtime 1.16.244.0).
--
-- Linux cross-compile:
--   xmake f -p windows -a x64 --toolchain=clang-cl --sdk=$HOME/.vsxwin -m releasedbg -y
--   xmake build

-- Project-local module overrides: find_rc.lua makes xmake locate an rc compiler on
-- Linux (upstream find_rc is Windows-only). Must come before any target resolution.
add_moduledirs("modules")

-- CommonLibSF (+ nested commonlib-shared) built from source as a static-lib subproject.
-- This replaces the original setup, which linked prebuilt *.lib files that are not
-- present in the repo. spdlog (v1.16.0, std_format+wchar) is pulled by commonlib-shared
-- via xrepo and propagates here transitively.
includes("extern/commonlibsf")

set_project("ModernCommandFramework")
set_version("5.1.7")
set_license("MIT")
set_languages("c++23")
set_warnings("allextra")
set_encodings("utf-8")

add_rules("mode.debug", "mode.release", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")

target("ModernCommandFramework")
    set_kind("shared")
    add_defines("UNICODE", "_UNICODE")

    -- This plugin defines and exports its own SFSEPlugin_Version / SFSEPlugin_Load
    -- (src/Commands.cpp, src/main.cpp) plus the RegisterCommand API, so it deliberately
    -- does NOT use the commonlibsf.plugin rule -- that rule generates its own
    -- SFSEPlugin_Version, which would collide at link time.
    add_deps("commonlibsf")

    -- source files. spdlog_os_defs.cpp was a hand-rolled stub for a manually built
    -- spdlog; the real spdlog (from commonlib-shared via xrepo) already provides those
    -- symbols, so compiling it would cause duplicate-symbol link errors.
    add_files("src/*.cpp")
    remove_files("src/spdlog_os_defs.cpp")
    add_headerfiles("src/*.h")
    add_includedirs("src")

    -- precompiled header
    set_pcxxheader("src/PCH.h")

    -- explicit exports: the two SFSE entry points + the public command-registration API
    add_ldflags(
        "/EXPORT:SFSEPlugin_Version",
        "/EXPORT:SFSEPlugin_Load",
		"/EXPORT:RegisterCommand",
        "/EXPORT:RegisterCommandWithContext",
        { force = true }
    )

    -- Win32 libraries used by MCF (some also come transitively from commonlib-shared).
    add_syslinks(
        "user32", "kernel32", "shell32", "version", "ws2_32", "advapi32",
        "bcrypt", "ole32", "oleaut32", "dxgi", "d3dcompiler", "dbghelp", "d3d11", "psapi"
    )
target_end()
