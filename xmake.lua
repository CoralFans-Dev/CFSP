add_rules("mode.debug", "mode.release")

add_repositories("liteldev-repo https://github.com/LiteLDev/xmake-repo.git")
add_repositories("coralfansdev-repo https://github.com/CoralFans-Dev/xmake-repo.git")

-- add_requires("levilamina x.x.x") for a specific version
-- add_requires("levilamina develop") to use develop version
-- please note that you should add bdslibrary yourself if using dev version
add_requires(
    "levilamina 1.2.0",
    "lua 5.4.7",
    "levibuildscript",
    "timewheel"
)
add_requires("boost", {configs = {all = true}})

if not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("CFSP") -- Change this to your mod name.
    add_rules("@levibuildscript/linkrule")
    add_cxflags(
        "/EHa",
        "/utf-8",
        "/W4",
        "/w44265",
        "/w44289",
        "/w44296",
        "/w45263",
        "/w44738",
        "/w45204"
    )
    add_defines("NOMINMAX", "UNICODE", "CFSPEXP")
    add_defines("VERSION=\"$(shell git describe --tags --abbrev=0 --always)\"")
    add_defines("COMMITID=\"$(shell git rev-parse HEAD)\"")
    add_files("src/**.cpp")
    add_headerfiles("src/(cfsp/**.h)")
    add_includedirs("src")
    add_packages(
        "levilamina",
        "boost",
        "levibuildscript",
        "lua",
        "timewheel"
    )
    add_shflags("/DELAYLOAD:bedrock_server.dll") -- To use symbols provided by SymbolProvider.
    set_exceptions("none") -- To avoid conflicts with /EHa.
    set_kind("shared")
    set_languages("c++20")
    set_symbols("debug")

    after_build(function (target)
        local mod_packer = import("scripts.after_build")

        local tag = os.iorun("git describe --tags --abbrev=0 --always")
        local major, minor, patch, suffix = tag:match("v(%d+)%.(%d+)%.(%d+)(.*)")
        if not major then
            print("Failed to parse version tag, using 0.0.0")
            major, minor, patch = 0, 0, 0
        end
        local mod_define = {
            modName = target:name(),
            modFile = path.filename(target:targetfile()),
            modVersion = major .. "." .. minor .. "." .. patch,
        }
        
        mod_packer.pack_mod(target,mod_define)
    end)
