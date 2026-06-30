#pragma once
#include "Core/NoodleCore.h"
#include "D3D12Common.h"
#include "Rendering/RenderCommands.h"
#include <queue>

constexpr uint32 MaxSprites = 1024;

class D3D12SpriteRenderer
{
public:
	bool Initialize(class D3D12Renderer& renderer);
	void SubmitSprite(const struct SpriteRenderCommand& cmd);
	void CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const struct CookedShaderData& shaderData);
	void Flush();

protected:

private:
	D3D12_VERTEX_BUFFER_VIEW m_QuadVertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_QuadIndexBufferView = {};

	D3D12Renderer* m_Renderer = nullptr;
	ComPtr<ID3D12Resource> m_QuadVertexBuffer = nullptr;
	ComPtr<ID3D12Resource> m_QuadIndexBuffer = nullptr;
	ComPtr<ID3D12Resource> m_SpriteConstantBuffer = nullptr;
	uint8* m_MappedSpriteConstants = nullptr;

	std::queue<SpriteRenderCommand>m_SpriteCommands;
};