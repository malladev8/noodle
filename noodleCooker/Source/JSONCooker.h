#pragma once
#include "Core/NoodleCore.h"

enum class eJsonType
{
	SCENE = 0,
	ACTOR,
	MATERIAL,
	SHADER,
	COUNT
};

bool ConvertJsonToBinary(eJsonType type, const char* applicationRoot, const char* assetName);