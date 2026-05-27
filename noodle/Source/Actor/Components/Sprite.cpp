#include "../../NoodlePch.h"
#include "Sprite.h"
#include <fstream>

const eComponentId SpriteInterface::COMPONENT_ID = eComponentId::COMPONENT_SPRITE;

bool Sprite::Init(std::ifstream& bin)
{
	// TODO: Move this to resource manager and load on to GPU, caching resource pointer in the process

	//bin.read(reinterpret_cast<char*>(&m_Texture.resource), sizeof(eTextureFormat));
	//bin.read(reinterpret_cast<char*>(&m_Texture.width), sizeof(uint32));
	//bin.read(reinterpret_cast<char*>(&m_Texture.height), sizeof(uint32));
	//
	//uint32 pixelCount = 0;
	//bin.read(reinterpret_cast<char*>(&pixelCount), sizeof(uint32));
	//
	//std::vector<unsigned char> pixels;
	//pixels.resize(pixelCount);
	//bin.read(reinterpret_cast<char*>(&pixels), pixelCount);


	return true;
}
