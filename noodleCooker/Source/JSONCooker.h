#pragma once
#include "Core/NoodleCore.h"

enum class eJsonType
{
	ACTOR = 0,
	SCENE,
	COUNT
};

bool ConvertJsonToBinary(eJsonType type, const char* applicationRoot, const char* assetName);