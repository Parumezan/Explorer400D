set_project("Explorer400D")

set_version("0.1.0")

add_rules("mode.debug", "mode.release")

includes("@builtin/xpack")

set_runtimes("stdc++_static")

add_requires("imgui docking", {configs = {glfw = true, opengl3 = true}, system = false})
add_requires("glfw", "glad", "implot-homemade", "imgui-file-dialog-homemade", "stb", "spdlog", "nlohmann_json", {system = false})
add_requires("libgphoto2", "libraw", "libcurl")

target("Explorer400D")
    set_kind("binary")
    set_languages("c++20")
    add_files("src/**.cpp")
    add_includedirs("src")
    add_packages("glfw", "glad", "imgui docking", "implot-homemade", "imgui-file-dialog-homemade", "libgphoto2", "libraw", "libcurl", "stb", "spdlog", "nlohmann_json")

    -- if is_plat("mingw") then
    --     add_ldflags("-static-libgcc", "-static-libstdc++", "-static")
    -- end

-- target("test_interface")
--     set_default(false)
--     set_kind("binary")
--     set_languages("c++20")
--     add_files("test/**.cpp")
--     add_packages("glfw", "glad", "imgui docking", "implot-homemade", "imgui-file-dialog-homemade", "libgphoto2", "libraw", "libcurl", "stb", "spdlog", "nlohmann_json")

xpack("Explorer400D")
    set_formats("nsis", "zip", "targz", "runself")
    set_title("Explorer400D")
    set_author("Pibe")
    set_description("Explorer400D installer")
    set_licensefile("LICENSE")
    add_targets("Explorer400D")
    add_sourcefiles("(src/**)")

package("implot-homemade")
    set_homepage("https://github.com/epezent/implot")
    set_description("Immediate Mode Plotting")
    set_license("MIT")

    add_urls("https://github.com/epezent/implot/archive/refs/tags/$(version).tar.gz",
             "https://github.com/epezent/implot.git")

    add_versions("v0.16", "961df327d8a756304d1b0a67316eebdb1111d13d559f0d3415114ec0eb30abd1")
    add_versions("v0.15", "3df87e67a1e28db86828059363d78972a298cd403ba1f5780c1040e03dfa2672")

    add_deps("imgui docking", {configs = {glfw = true, opengl3 = true}, system = false})

    on_install("windows", "linux", "macosx", "mingw", "android", "iphoneos", function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
            add_requires("imgui docking", {configs = {glfw = true, opengl3 = true}, system = false})
            add_rules("mode.release", "mode.debug")
            target("implot")
                set_kind("$(kind)")
                set_languages("c++11")
                add_files("*.cpp")
                add_headerfiles("*.h")
                add_packages("imgui")
                if is_plat("windows") and is_kind("shared") then
                    add_rules("utils.symbols.export_all", {export_classes = true})
                end
        ]])
        if package:config("shared") then
            configs.kind = "shared"
        end
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <implot.h>
            void test() {
                ImPlot::CreateContext();
                ImPlot::DestroyContext();
            }
        ]]}, {configs = {languages = "c++11"}}))
    end)

package("imgui-file-dialog-homemade")
    set_homepage("https://github.com/aiekick/ImGuiFileDialog")
    set_description("File Dialog for Dear ImGui")
    set_license("MIT")

    add_urls("https://github.com/aiekick/ImGuiFileDialog/archive/refs/tags/$(version).tar.gz",
             "https://github.com/aiekick/ImGuiFileDialog.git")

    add_versions("v0.6.7", "136e714965afaec2bac857bf46a653fdd74a0bf493e281682706c604113026b8")
    add_versions("v0.6.6", "75420f6eaf74fb1fa22042713f573858d8549366e7741baaf91128eb065b4b47")
    add_versions("v0.6.5", "3fac0f2cfc92b3f2c806e6743236467d0f691e54b1747a3955b82ef28b13e2fa")

    if is_plat("mingw") then
        add_configs("shared", {description = "Build shared binaries.", default = false, type = "boolean", readonly = true})
    end

    add_deps("imgui docking", {configs = {glfw = true, opengl3 = true}, system = false})
    if is_plat("windows") then
        add_deps("dirent")
    end

    on_install("windows", "linux", "macosx", "mingw", "android", function (package)
        local configs = {}
        io.writefile("xmake.lua", [[
            add_requires("imgui docking", {configs = {glfw = true, opengl3 = true}, system = false})
            if is_plat("windows") then
                add_requires("dirent")
                add_packages("dirent")
            end
            add_rules("mode.debug", "mode.release")
            target("imgui-file-dialog")
                set_kind("$(kind)")
                set_languages("c++11")
                add_files("ImGuiFileDialog.cpp")
                add_headerfiles("ImGuiFileDialog.h", "ImGuiFileDialogConfig.h")
                add_packages("imgui")
                if is_plat("windows") and is_kind("shared") then
                    add_rules("utils.symbols.export_all", {export_classes = true})
                end
        ]])
        import("package.tools.xmake").install(package, configs)
    end)

    on_test(function (package)
        assert(package:check_cxxsnippets({test = [[
            #include <ImGuiFileDialog.h>
            void test() {
                ImGuiFileDialog::Instance()->Close();
            }
        ]]}, {configs = {languages = "c++11"}}))
    end)