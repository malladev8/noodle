#include "ShaderCooker.h"
#include <filesystem>

bool ShaderCooker::CookShaderByteCode(const std::string& sourcePath, const std::string& entry, const std::string& profile, const std::string& outputPath)
{
    std::filesystem::path dxcPath = std::filesystem::current_path().parent_path() / "noodle/Source/ThirdParty/dxc/dxc_2026_05_27/bin/x64/dxc.exe";
    std::string command = dxcPath.string() + " " + sourcePath + " -E " + entry + " -T " + profile + " -Fo " + outputPath;
    int result = system(command.c_str());
    return result == 0;
}
