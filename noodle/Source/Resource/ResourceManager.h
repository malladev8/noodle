#pragma once
#include "Core/NoodleCore.h"
#include "ResourceTypes.h"
#include <unordered_map>

class ResourceManager
{
public:
	std::shared_ptr<Texture> LoadTexture(std::ifstream& bin);

protected:
private:
	std::unordered_map<AssetId, std::weak_ptr<Texture>> m_TextureCache;
	// Will want to add separate chaches for meshes, shaders, etc. later on.
	// Asset registry?
};