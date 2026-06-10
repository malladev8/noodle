#include "ShaderCooker.h"

bool ShaderCooker::CookShaderByteCode(const std::string& sourcePath, const std::string& entry, const std::string& profile, const std::string& outputPath)
{
    std::string command = "$(SolutionDir)noodle/Source/ThirdParty/dxc/dxc_2026_05_27/bin/x64/dxc.exe " + sourcePath + " -E " + entry + " -T " + profile + " -Fo " + outputPath;
    system(command.c_str());
	return true;
}
