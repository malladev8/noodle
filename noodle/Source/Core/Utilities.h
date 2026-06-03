#pragma once
#include <string>

namespace path
{
	std::string ReadFilePath(std::ifstream& bin);
	void WritePath(const std::string& path, std::ofstream& out);
}

namespace str
{
#if defined(WINDOWS)
	std::string WideToUtf8(const std::wstring& wide);
#endif
}