#pragma once
#include "Core/NoodleCore.h"
#include "D3D12Common.h"
#include "Rendering/RenderCommands.h"
#include <vector>

static constexpr uint32 MaxDebugAABBs = 1024;
static constexpr uint32 NumBoxCorners = 8;
static constexpr uint32 NumBoxIndices = 24;

class D3D12DebugRenderer
{
public:
	bool Initialize(class D3D12Renderer& renderer);
	void SubmitAABB(const DebugAABBCommand& cmd);
	void CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const struct CookedShaderData& shaderData);
	void Flush();

protected:

private:

	D3D12_VERTEX_BUFFER_VIEW m_BoxVertexBufferView = {};
	D3D12_INDEX_BUFFER_VIEW m_BoxIndexBufferView = {};

	D3D12Renderer* m_Renderer = nullptr;
	ComPtr<ID3D12Resource> m_BoxVertexBuffer = nullptr;
	ComPtr<ID3D12Resource> m_BoxIndexBuffer = nullptr;
	DebugVertex* m_MappedBoxVertices = nullptr;
	std::shared_ptr<Shader> m_DebugShader;

	std::vector<DebugAABBCommand> m_AABBCommands;
};