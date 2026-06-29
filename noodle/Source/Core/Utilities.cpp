#include "NoodlePch.h"
#include "Utilities.h"
#include <fstream>

std::string path::ReadFilePath(std::ifstream& bin)
{
	uint8 pathLength;
	bin.read(reinterpret_cast<char*>(&pathLength), sizeof(uint8));
	std::string path;
	path.resize(pathLength);
	bin.read(reinterpret_cast<char*>(path.data()), pathLength);
	return path;
}

void path::WritePath(const std::string& path, std::ofstream& out)
{
	uint8 pathLength = (uint8)path.length();
	out.write(reinterpret_cast<const char*>(&pathLength), sizeof(uint8));
	out.write(path.data(), pathLength);
}

#if defined(WINDOWS)
std::string str::WideToUtf8(const std::wstring& wide)
{
	if (wide.empty())
	{
		return {};
	}
	int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), nullptr, 0, nullptr, nullptr);
	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(CP_UTF8, 0, wide.data(), (int)wide.size(), result.data(), sizeNeeded, nullptr, nullptr);
	return result;
}
#endif

uint32 align::Align256(uint32 size)
{
	return (size + 255) & ~255;
}
