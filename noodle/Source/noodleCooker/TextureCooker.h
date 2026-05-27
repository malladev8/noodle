#pragma once
#include "../Core/NoodleCore.h"
#include "../Resource/ResourceTypes.h"

#include <fstream>

class TextureCooker
{
public:
	bool CookTexture(const char* assetPath, std::ofstream& out, AssetId assetId);
protected:
private:
};