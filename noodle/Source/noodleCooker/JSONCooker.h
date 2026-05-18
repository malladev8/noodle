#pragma once
#include "../Core/NoodleCore.h"

enum class eJsonType
{
	ACTOR = 0,
	SCENE,
	COUNT
};

bool ConvertJsonToBinary(const char* jsonPath, const char* binPath, eJsonType type);