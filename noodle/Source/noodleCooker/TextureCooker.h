#pragma once
#include "../Core/NoodleCore.h"
#include <fstream>

class TextureCooker
{
public:
	bool CookTexture(const char* assetPath, std::ofstream& out);
protected:
private:
};