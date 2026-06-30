#include "NoodlePch.h"
#include "D3D12SpriteRenderer.h"
#include "D3D12Renderer.h"
#include "D3D12ResourceTypes.h"

static const SpriteVertex sQuadVerts[]
{
	{{ 0.0f, -0.5f, -0.5f}, {0,1}}, // bottom left
	{{ 0.0f,  0.5f, -0.5f}, {1,1}}, // bottom right
	{{ 0.0f, -0.5f,  0.5f}, {1,0}}, // top left
	{{ 0.0f,  0.5f,  0.5f}, {0,0}}  // top right
};

static const uint32 sQuadIndices[]
{
	0, 1, 2, // bl, br, tl
	2, 1, 3  // tl, br, tr
};

bool D3D12SpriteRenderer::Initialize(D3D12Renderer& renderer)
{
	m_Renderer = &renderer;
	ID3D12Device* device = renderer.GetDevice();

	// Create quad vertex buffer
	// Common property helper structure for an upload heap
	D3D12_HEAP_PROPERTIES heapProps = {};
	heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	// Define vertex buffer resource description
	D3D12_RESOURCE_DESC vertexBufferDesc = {};
	vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexBufferDesc.Width = sizeof(sQuadVerts);
	vertexBufferDesc.Height = 1;
	vertexBufferDesc.DepthOrArraySize = 1;
	vertexBufferDesc.MipLevels = 1;
	vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
	vertexBufferDesc.SampleDesc.Count = 1;
	vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// Create Vertex Buffer Resource
	HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_QuadVertexBuffer));
	if (FAILED(hr))
	{
		N_LOG("Failed to create sprite quad vertex buffer resource. hr: %i", hr);
		return false;
	}

	// Define index buffer resource description
	D3D12_RESOURCE_DESC indexBufferDesc = vertexBufferDesc; // Copy structure layout
	indexBufferDesc.Width = sizeof(sQuadIndices);

	// Create Index Buffer Resource
	hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &indexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_QuadIndexBuffer));
	if (FAILED(hr))
	{
		N_LOG("Failed to create sprite quad index buffer resource. hr: %i", hr);
		return false;
	}

	// Copy memory from CPU to GPU
	void* pVertexDataBegin = nullptr;
	D3D12_RANGE readRange = { 0, 0 }; // We do not intend to read this resource from the CPU

	// Map and copy vertex data
	m_QuadVertexBuffer->Map(0, &readRange, &pVertexDataBegin);
	memcpy(pVertexDataBegin, sQuadVerts, sizeof(sQuadVerts));
	m_QuadVertexBuffer->Unmap(0, nullptr);

	void* pIndexDataBegin = nullptr;
	// Map and copy index data
	m_QuadIndexBuffer->Map(0, &readRange, &pIndexDataBegin);
	memcpy(pIndexDataBegin, sQuadIndices, sizeof(sQuadIndices));
	m_QuadIndexBuffer->Unmap(0, nullptr);

	// Configure buffer views
	// Initialize the Vertex Buffer View (VBV)
	m_QuadVertexBufferView.BufferLocation = m_QuadVertexBuffer->GetGPUVirtualAddress();
	m_QuadVertexBufferView.StrideInBytes = sizeof(SpriteVertex);
	m_QuadVertexBufferView.SizeInBytes = sizeof(sQuadVerts);

	// Initialize the Index Buffer View (IBV)
	m_QuadIndexBufferView.BufferLocation = m_QuadIndexBuffer->GetGPUVirtualAddress();
	m_QuadIndexBufferView.Format = DXGI_FORMAT_R32_UINT; // Match uint32_t indices array
	m_QuadIndexBufferView.SizeInBytes = sizeof(sQuadIndices);

	// Initialize Sprite Constant Buffer
	uint32 cbSize = align::Align256(sizeof(FrameConstants)); // D3D12 requires all constant buffers to be 256 byte-aligned
	cbSize *= MaxSprites;

	D3D12_HEAP_PROPERTIES spriteCbHeapProps = {};
	spriteCbHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC spriteCbDesc = {};
	spriteCbDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	spriteCbDesc.Width = cbSize;
	spriteCbDesc.Height = 1;
	spriteCbDesc.DepthOrArraySize = 1;
	spriteCbDesc.MipLevels = 1;
	spriteCbDesc.SampleDesc.Count = 1;
	spriteCbDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = device->CreateCommittedResource(&spriteCbHeapProps, D3D12_HEAP_FLAG_NONE, &spriteCbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_SpriteConstantBuffer));
	if (FAILED(hr))
	{
		N_LOG("Failed to create committed resource for Sprite Constant Buffer. HR: %i", hr);
		return false;
	}

	D3D12_RANGE spriteCbReadRange = { 0, 0 };
	// Common in D3D12 to leave upload CBs mapped forever like this
	m_SpriteConstantBuffer->Map(0, &spriteCbReadRange, reinterpret_cast<void**>(&m_MappedSpriteConstants));

	return true;
}

void D3D12SpriteRenderer::SubmitSprite(const SpriteRenderCommand& cmd)
{
	m_SpriteCommands.push(cmd);
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

	CD3DX12_ROOT_PARAMETER rootParams[(uint32)eRootParamIndex::COUNT];
	rootParams[(uint32)eRootParamIndex::FRAME_CB].InitAsConstantBufferView((uint32)eRootParamIndex::FRAME_CB); // Frame constant buffer, b0
	rootParams[(uint32)eRootParamIndex::OBJECT_CB].InitAsConstantBufferView((uint32)eRootParamIndex::OBJECT_CB); // Object constant buffer, b1
	rootParams[(uint32)eRootParamIndex::SRV].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL); // SRV t0

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
	ID3D12GraphicsCommandList* cmdList = m_Renderer->GetCommandList();

	// Set Descriptor Heaps
	ID3D12DescriptorHeap* heaps[] =
	{
		m_Renderer->GetSrvHeap()
	};
	cmdList->SetDescriptorHeaps(1, heaps);

	// Bind VB and IB
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &m_QuadVertexBufferView);
	cmdList->IASetIndexBuffer(&m_QuadIndexBufferView);

	// Convert queued render commands into GPU draw calls for each sprite
	const uint32 alignedSize = align::Align256(sizeof(SpriteConstants));
	uint32 spriteIndex = 0;
	ID3D12Resource* frameConstantBuffer = m_Renderer->GetFrameConstantBuffer();

	while (!m_SpriteCommands.empty())
	{
		const SpriteRenderCommand& spriteCmd = m_SpriteCommands.front();
		const uint32 offset = spriteIndex * alignedSize;
		N_ASSERT(cmdList != nullptr, "cmdList cannot be null");

		// Bind PSO
		D3D12Shader* shader = static_cast<D3D12Shader*>(spriteCmd.material->shader.get());
		cmdList->SetPipelineState(shader->pso.Get());
		cmdList->SetGraphicsRootSignature(shader->rootSignature.Get());

		// Frame Constant Buffer
		cmdList->SetGraphicsRootConstantBufferView((uint32)eRootParamIndex::FRAME_CB, frameConstantBuffer->GetGPUVirtualAddress());

		// Map sprite constants
		SpriteConstants spriteConstants;
		spriteConstants.world = spriteCmd.world;
		spriteConstants.color = spriteCmd.color;
		memcpy(m_MappedSpriteConstants + offset, &spriteConstants, sizeof(spriteConstants));

		// Bind sprite constant buffer
		D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_SpriteConstantBuffer->GetGPUVirtualAddress() + offset;
		cmdList->SetGraphicsRootConstantBufferView((uint32)eRootParamIndex::OBJECT_CB, gpuAddress);

		// Bind Texture
		D3D12TextureResource* texture = static_cast<D3D12TextureResource*>(spriteCmd.material->diffuseTexture->resource.get());
		cmdList->SetGraphicsRootDescriptorTable((uint32)eRootParamIndex::SRV, texture->srvGpuHandle);

		// Draw
		cmdList->DrawIndexedInstanced(6 /*quad indices*/, 1 /*instance count*/, 0, 0, 0);

		m_SpriteCommands.pop();
		++spriteIndex;
	}
}
