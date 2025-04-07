set_project("CuteHttpServer")
set_xmakever("2.9.8")

set_languages("c++20")

add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate")

set_warnings("allextra")
set_defaultmode("debug")

add_requires("catch2")
add_packages("catch2")

includes("3rd-party/CuteMuduo")

includes("CuteHttpServer")
-- includes("tests")
