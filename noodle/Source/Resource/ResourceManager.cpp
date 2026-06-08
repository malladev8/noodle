#include "NoodlePch.h"
#include "ResourceManager.h"
#include "NoodleEngine.h"
#include "Rendering/IRenderer.h"
#include <fstream>

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::ifstream& bin)
{
	AssetId assetId = 0;
	bin.read(reinterpret_cast<char*>(&assetId), sizeof(AssetId));

	if (!m_TextureCache.contains(assetId) || m_TextureCache[assetId].expired())
	{
		// Create Texture
		std::shared_ptr<Texture> tex = std::make_shared<Texture>();
		bin.read(reinterpret_cast<char*>(&tex->format), sizeof(eTextureFormat));
		bin.read(reinterpret_cast<char*>(&tex->width), sizeof(uint32));
		bin.read(reinterpret_cast<char*>(&tex->height), sizeof(uint32));
		
		uint32 pixelCount = 0;
		bin.read(reinterpret_cast<char*>(&pixelCount), sizeof(uint32));
		
		std::vector<uint8> pixels;
		pixels.resize(pixelCount);
		bin.read(reinterpret_cast<char*>(pixels.data()), pixelCount);

		Engine::Get().GetContext().renderer->CreateTextureResources(*tex, pixels);

		m_TextureCache.insert({ assetId, tex });
		return tex;
	}
	else
	{
		return m_TextureCache[assetId].lock();
	}

	return nullptr;
}
