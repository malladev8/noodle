#include "NoodlePch.h"
#include "D3D12SpriteRenderer.h"
#include "D3D12Renderer.h"

bool D3D12SpriteRenderer::Initialize(D3D12Renderer& renderer)
{
	m_Renderer = &renderer;
	return true;
}

void D3D12SpriteRenderer::SubmitSprite(const SpriteRenderCommand& cmd)
{
}

void D3D12SpriteRenderer::CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const CookedShaderData& shaderData)
{
	outShader = std::make_shared<D3D12Shader>();
	D3D12Shader* d3d12Shader = static_cast<D3D12Shader*>(outShader.get());

	// Create Bytecode Structs
	D3D12_SHADER_BYTECODE vsBytecode = {};
	vsBytecode.pShaderBytecode = shaderData.vsBytes.data();
	vsBytecode.BytecodeLength = shaderData.vsBytes.size();

	D3D12_SHADER_BYTECODE psBytecode = {};
	psBytecode.pShaderBytecode = shaderData.psBytes.data();
	psBytecode.BytecodeLength = shaderData.psBytes.size();

	// Create Root Signature
	CD3DX12_DESCRIPTOR_RANGE texRange;
	texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0

	CD3DX12_ROOT_PARAMETER rootParams[2];
	rootParams[0].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL); // Texture descriptor table
	rootParams[1].InitAsConstantBufferView(0); // Transform constant buffer, b0

	CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR); // s0

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(_countof(rootParams), rootParams, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> serializedRootSig;
	ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);
	if (FAILED(hr))
	{
		N_LOG(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		N_ASSERT(false, "Failed to Serialize Root Signature.");
	}

	ID3D12Device* device = m_Renderer->GetDevice();

	hr = device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&d3d12Shader->rootSignature));
	N_ASSERT(SUCCEEDED(hr), "Failed to Create Root Signature");

	// Input Layout
	// This is the layout for the sprite shader currently.
	// TODO: Maybe need way to infer layout from shader
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			0,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		},

		{
			"UV",
			0,
			DXGI_FORMAT_R32G32_FLOAT,
			0,
			12,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			0
		}
	};

	// PSO desc
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout =
	{
		inputLayout,
		_countof(inputLayout)
	};
	psoDesc.pRootSignature = d3d12Shader->rootSignature.Get();
	psoDesc.VS = vsBytecode;
	psoDesc.PS = psBytecode;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = BufferCount; // Does this need to change to match our number of back buffers?
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;

	// Blend state
	D3D12_RENDER_TARGET_BLEND_DESC& blendDesc = psoDesc.BlendState.RenderTarget[0];
	blendDesc.BlendEnable = TRUE;
	blendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// Depth state
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	// Create PSO
	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&d3d12Shader->pso));
	N_ASSERT(SUCCEEDED(hr), "Failed to create PSO.");
}

void D3D12SpriteRenderer::Flush()
{
	// Convert queued render commands into GPU draw calls
}
