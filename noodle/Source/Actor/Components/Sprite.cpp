#include "../../NoodlePch.h"
#include "Sprite.h"
#include <fstream>

const eComponentId SpriteInterface::COMPONENT_ID = eComponentId::COMPONENT_SPRITE;

bool Sprite::Init(std::ifstream& bin)
{
	std::string texturePath = path::ReadFilePath(bin);

	// TODO: use resource manager to load / get pointer to texture

	return true;
}
