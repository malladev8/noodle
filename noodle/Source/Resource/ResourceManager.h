#pragma once
#include "Core/NoodleCore.h"
#include "ResourceTypes.h"
#include <unordered_map>

class ResourceManager
{
public:
	std::shared_ptr<Material> LoadMaterial(std::ifstream& bin);
	std::shared_ptr<Shader> LoadShader(std::ifstream& bin);
	std::shared_ptr<Texture> LoadTexture(std::ifstream& bin);

protected:
private:
	std::unordered_map<AssetId, std::weak_ptr<Material>> m_MaterialCache;
	std::unordered_map<AssetId, std::weak_ptr<Shader>> m_ShaderCache;
	std::unordered_map<AssetId, std::weak_ptr<Texture>> m_TextureCache;
	// Will want to add separate chaches for meshes, shaders, etc. later on.
	// Asset registry?

	AssetId ReadAssetId(std::ifstream& bin);
};