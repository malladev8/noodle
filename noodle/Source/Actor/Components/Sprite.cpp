#include "NoodlePch.h"
#include "Sprite.h"
#include "EngineContext.h"
#include "Resource/ResourceManager.h"
#include <fstream>

const eComponentId SpriteInterface::COMPONENT_ID = eComponentId::COMPONENT_SPRITE;

bool Sprite::Init(std::ifstream& bin, EngineContext& engineContext)
{
	std::string texturePath = path::ReadFilePath(bin);

	std::ifstream textureBin(texturePath, std::ios::binary);
	if (!bin.is_open())
	{
		N_LOG("Failed to open Texture binary file %s", texturePath);
		return false;
	}

	m_Texture = engineContext.resourceManager.LoadTexture(textureBin);
	return true;
}
