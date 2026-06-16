#pragma once
#include "Core/NoodleCore.h"
//#include "D3D12Common.h"

class D3D12SpriteRenderer
{
public:
	bool Initialize(class D3D12Renderer& renderer);
	void SubmitSprite(const struct SpriteRenderCommand& cmd);
	void CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const struct CookedShaderData& shaderData);
	void Flush();
protected:
private:
	D3D12Renderer* m_Renderer = nullptr;
};