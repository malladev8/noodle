#include "NoodlePch.h"
#include "ResourceManager.h"
#include "NoodleEngine.h"
#include "Rendering/IRenderer.h"
#include <fstream>

std::shared_ptr<Material> ResourceManager::LoadMaterial(std::ifstream& bin)
{
	AssetId assetId = ReadAssetId(bin);

	if (!m_MaterialCache.contains(assetId) || m_MaterialCache[assetId].expired())
	{
		std::shared_ptr<Material> material = std::make_shared<Material>();

		// Shader
		std::string shaderPath = path::ReadFilePath(bin);
		std::ifstream shaderBin(shaderPath, std::ios::binary);
		N_ASSERT(shaderBin.is_open(), "Failed to open Shader binary file.");
		material->shader = LoadShader(shaderBin);
		shaderBin.close();

		// Diffuse Texture
		std::string diffuseTexturePath = path::ReadFilePath(bin);
		std::ifstream diffuseTextureBin(diffuseTexturePath, std::ios::binary);
		N_ASSERT(diffuseTextureBin.is_open(), "Failed to open Texture binary file.");
		material->diffuseTexture = LoadTexture(diffuseTextureBin);
		diffuseTextureBin.close();

		// Texture Layout
		uint32 numRows, numColumns;
		N_ASSERT(bin.read(reinterpret_cast<char*>(&numRows), sizeof(numRows)), "Failed to read number of rows for texture layout");
		N_ASSERT(bin.read(reinterpret_cast<char*>(&numColumns), sizeof(numColumns)), "Failed to read number of columns for texture layout");
		material->diffuseTextureLayout.numRows = numRows;
		material->diffuseTextureLayout.numColumns = numColumns;
		material->diffuseTextureLayout.tileWidth = material->diffuseTexture->width / numColumns;
		material->diffuseTextureLayout.tileHeight = material->diffuseTexture->height / numRows;

		m_MaterialCache.insert({ assetId, material });
		return material;
	}
	else
	{
		return m_MaterialCache[assetId].lock();
	}

	return nullptr;
}

std::shared_ptr<Shader> ResourceManager::LoadShader(std::ifstream& bin)
{
	AssetId assetId = ReadAssetId(bin);

	if (!m_ShaderCache.contains(assetId) || m_ShaderCache[assetId].expired())
	{
		CookedShaderData shaderData;
		uint32 vsSize = 0;
		uint32 psSize = 0;
		N_ASSERT(bin.read(reinterpret_cast<char*>(&vsSize), sizeof(vsSize)), "Failed to read shader size.");
		N_ASSERT(bin.read(reinterpret_cast<char*>(&psSize), sizeof(psSize)), "Failed to read shader size.");

		shaderData.vsBytes.resize(vsSize);
		shaderData.psBytes.resize(psSize);
		N_ASSERT(bin.read(reinterpret_cast<char*>(shaderData.vsBytes.data()), vsSize), "Failed to read shader bytecode.");
		N_ASSERT(bin.read(reinterpret_cast<char*>(shaderData.psBytes.data()), psSize), "Failed to read shader bytecode.");

		std::shared_ptr<Shader> shader = nullptr;
		Engine::Get().GetContext().renderer->CreateShaderResources(shader, shaderData);

		m_ShaderCache.insert({ assetId, shader });
		return shader;
	}
	else
	{
		return m_ShaderCache[assetId].lock();
	}

	return nullptr;
}

std::shared_ptr<Texture> ResourceManager::LoadTexture(std::ifstream& bin)
{
	AssetId assetId = ReadAssetId(bin);

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

AssetId ResourceManager::ReadAssetId(std::ifstream& bin)
{
	AssetId assetId = 0;
	bin.read(reinterpret_cast<char*>(&assetId), sizeof(AssetId));
	return assetId;
}
