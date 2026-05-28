#pragma once
#include <string>

namespace path
{
	std::string ReadFilePath(std::ifstream& bin);
	void WritePath(const std::string& path, std::ofstream& out);
}