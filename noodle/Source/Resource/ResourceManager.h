#pragma once
#include "Core/NoodleCore.h"
#include "ResourceTypes.h"
#include <unordered_map>

class IRenderer;

class ResourceManager
{
public:
	std::shared_ptr<Material> LoadMaterial(std::ifstream& bin, eShaderType type, IRenderer& renderer);
	std::shared_ptr<Shader> LoadShader(std::ifstream& bin, eShaderType type, IRenderer& renderer);
	std::shared_ptr<Texture> LoadTexture(std::ifstream& bin, IRenderer& renderer);
	std::shared_ptr<TextureAtlas> LoadAtlas(std::ifstream& bin);

protected:
private:
	std::unordered_map<AssetId, std::weak_ptr<Material>> m_MaterialCache;
	std::unordered_map<AssetId, std::weak_ptr<Shader>> m_ShaderCache;
	std::unordered_map<AssetId, std::weak_ptr<Texture>> m_TextureCache;
	std::unordered_map<AssetId, std::weak_ptr<TextureAtlas>> m_AtlasCache;
	// Asset registry?

	AssetId ReadAssetId(std::ifstream& bin);
};