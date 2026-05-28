#include "NoodlePch.h"
#include "ResourceManager.h"
#include <fstream>

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::ifstream& bin)
{
	AssetId assetId = 0;
	bin.read(reinterpret_cast<char*>(&assetId), sizeof(AssetId));

	if (!m_TextureCache.contains(assetId) || m_TextureCache[assetId].expired())
	{
		// Create Texture
		Texture* tex = N_NEW Texture();
		bin.read(reinterpret_cast<char*>(&tex->format), sizeof(eTextureFormat));
		bin.read(reinterpret_cast<char*>(&tex->width), sizeof(uint32));
		bin.read(reinterpret_cast<char*>(&tex->height), sizeof(uint32));
		
		uint32 pixelCount = 0;
		bin.read(reinterpret_cast<char*>(&pixelCount), sizeof(uint32));
		
		std::vector<unsigned char> pixels;
		pixels.resize(pixelCount);
		bin.read(reinterpret_cast<char*>(pixels.data()), pixelCount);

		// TODO: Initialize GPU resource and cache in Texture::resource

		std::shared_ptr<Texture> texStrongRef = std::make_shared<Texture>(tex);
		m_TextureCache.insert({ assetId, texStrongRef });
		return texStrongRef;
	}
	else
	{
		return m_TextureCache[assetId].lock();
	}

	return nullptr;
}
