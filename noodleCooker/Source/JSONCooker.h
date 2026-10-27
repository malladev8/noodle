#pragma once
#include "Core/NoodleCore.h"

enum class eJsonType
{
	SCENE = 0,
	MATERIAL,
	SHADER,
	ATLAS,
	COUNT
};

bool ConvertJsonToBinary(eJsonType type, const char* assetRoot, const char* cookedRoot, const char* assetName);