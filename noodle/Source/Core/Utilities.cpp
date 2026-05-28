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
