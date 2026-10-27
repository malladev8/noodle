#include "NoodlePch.h"
#include "D3D12DebugRenderer.h"
#include "D3D12Renderer.h"
#include "D3D12ResourceTypes.h"
#include "NoodleEngine.h"
#include <filesystem>
#include <fstream>

static const uint32 sBoxIndices[] =
{
    // Front face
    0, 1,
    1, 2,
    2, 3,
    3, 0,

    // Back face
    4, 5,
    5, 6,
    6, 7,
    7, 4,

    // Connect front and back faces
    0, 4,
    1, 5,
    2, 6,
    3, 7
};

bool D3D12DebugRenderer::Initialize(D3D12Renderer& renderer)
{
    m_Renderer = &renderer;

    ID3D12Device* device = renderer.GetDevice();

    // Create upload-heap vertex buffer.
    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC vertexBufferDesc = {};
    vertexBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    vertexBufferDesc.Width = sizeof(DebugVertex) * NumBoxCorners * MaxDebugAABBs;
    vertexBufferDesc.Height = 1;
    vertexBufferDesc.DepthOrArraySize = 1;
    vertexBufferDesc.MipLevels = 1;
    vertexBufferDesc.Format = DXGI_FORMAT_UNKNOWN;
    vertexBufferDesc.SampleDesc.Count = 1;
    vertexBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vertexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_BoxVertexBuffer));

    if (FAILED(hr))
    {
        N_LOG("Failed to create debug AABB vertex buffer. HR: %i", hr);
        return false;
    }

    // Create static index buffer. The same 24 indices are reused for each box.
    D3D12_RESOURCE_DESC indexBufferDesc = vertexBufferDesc;
    indexBufferDesc.Width = sizeof(sBoxIndices);

    hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &indexBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_BoxIndexBuffer));

    if (FAILED(hr))
    {
        N_LOG("Failed to create debug AABB index buffer. HR: %i", hr);
        return false;
    }

    // Initialize the static index buffer.
    void* indexData = nullptr;
    D3D12_RANGE readRange = { 0, 0 };

    hr = m_BoxIndexBuffer->Map(0, &readRange, &indexData);

    if (FAILED(hr))
    {
        N_LOG("Failed to map debug AABB index buffer. HR: %i", hr);
        return false;
    }

    memcpy(indexData, sBoxIndices, sizeof(sBoxIndices));
    m_BoxIndexBuffer->Unmap(0, nullptr);

    // Persistently map the dynamic vertex buffer.
    void* vertexData = nullptr;

    hr = m_BoxVertexBuffer->Map(0, &readRange, &vertexData);

    if (FAILED(hr))
    {
        N_LOG("Failed to map debug AABB vertex buffer. HR: %i", hr);
        return false;
    }

    m_MappedBoxVertices = static_cast<DebugVertex*>(vertexData);

    // Configure vertex buffer view.
    m_BoxVertexBufferView.BufferLocation = m_BoxVertexBuffer->GetGPUVirtualAddress();
    m_BoxVertexBufferView.StrideInBytes = sizeof(DebugVertex);
    m_BoxVertexBufferView.SizeInBytes = sizeof(DebugVertex) * NumBoxCorners * MaxDebugAABBs;

    // Configure index buffer view.
    m_BoxIndexBufferView.BufferLocation = m_BoxIndexBuffer->GetGPUVirtualAddress();
    m_BoxIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
    m_BoxIndexBufferView.SizeInBytes = sizeof(sBoxIndices);

    // Load Debug Shaders Directly
    std::filesystem::path projectRoot = std::filesystem::current_path();
    std::filesystem::path cookedAssetRoot = projectRoot / "Cooked";
    std::filesystem::path shaderPath = cookedAssetRoot / "Shader" / "DebugBoxShader.bin";
    std::ifstream bin(shaderPath, std::ios::binary);
    if (!bin.is_open())
    {
        N_LOG("Failed to open Shader binary file %s", shaderPath.string());
    }
    if (bin.is_open())
    {
        Engine::Get().GetContext().resourceManager.LoadShader(bin, eShaderType::DEBUG_SHADER, *m_Renderer);
    }
    bin.close();

    return true;
}

void D3D12DebugRenderer::SubmitAABB(const DebugAABBCommand& cmd)
{
    m_AABBCommands.push_back(cmd);
}

void D3D12DebugRenderer::CreateShaderResources(std::shared_ptr<struct Shader>& outShader, const CookedShaderData& shaderData)
{
    outShader = std::make_shared<D3D12Shader>();
    m_DebugShader = outShader;

    D3D12Shader* d3d12Shader = static_cast<D3D12Shader*>(m_DebugShader.get());

    // Shader bytecode supplied by the cooker.
    D3D12_SHADER_BYTECODE vsBytecode = {};
    vsBytecode.pShaderBytecode = shaderData.vsBytes.data();
    vsBytecode.BytecodeLength = shaderData.vsBytes.size();

    D3D12_SHADER_BYTECODE psBytecode = {};
    psBytecode.pShaderBytecode = shaderData.psBytes.data();
    psBytecode.BytecodeLength = shaderData.psBytes.size();

    // Root signature: only the frame constant buffer at b0 is required.
    CD3DX12_ROOT_PARAMETER rootParams[1];
    rootParams[0].InitAsConstantBufferView(0);

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(_countof(rootParams), rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serializedRootSig;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &serializedRootSig, &errorBlob);

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            N_LOG("%s", reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
        }

        N_ASSERT(false, "Failed to serialize debug renderer root signature.");
        return;
    }

    ID3D12Device* device = m_Renderer->GetDevice();

    hr = device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&d3d12Shader->rootSignature));

    if (FAILED(hr))
    {
        N_ASSERT(false, "Failed to create debug renderer root signature.");
        return;
    }

    // Input layout: float3 POSITION followed by float4 COLOR.
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
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32A32_FLOAT,
            0,
            12,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        }
    };

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
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    // No depth buffer is currently bound by the renderer.
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;

    hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&d3d12Shader->pso));

    N_ASSERT(SUCCEEDED(hr), "Failed to create debug renderer PSO.");
}

void D3D12DebugRenderer::Flush()
{
    // Draw Boxes
    if (m_AABBCommands.empty())
    {
        return;
    }

    N_ASSERT(m_Renderer != nullptr, "Debug renderer has no renderer.");
    N_ASSERT(m_DebugShader != nullptr, "Debug renderer shader has not been created.");

    if (m_Renderer == nullptr || m_DebugShader == nullptr)
    {
        m_AABBCommands.clear();
        return;
    }

    const uint32 numBoxes = static_cast<uint32>(std::min<size_t>(m_AABBCommands.size(), MaxDebugAABBs));

    // Upload the eight corners of each AABB into its own vertex-buffer region.
    for (uint32 boxIndex = 0; boxIndex < numBoxes; ++boxIndex)
    {
        const DebugAABBCommand& cmd = m_AABBCommands[boxIndex];

        const vec3 corners[NumBoxCorners] =
        {
            { cmd.aabb.min.x, cmd.aabb.min.y, cmd.aabb.min.z },
            { cmd.aabb.max.x, cmd.aabb.min.y, cmd.aabb.min.z },
            { cmd.aabb.max.x, cmd.aabb.max.y, cmd.aabb.min.z },
            { cmd.aabb.min.x, cmd.aabb.max.y, cmd.aabb.min.z },

            { cmd.aabb.min.x, cmd.aabb.min.y, cmd.aabb.max.z },
            { cmd.aabb.max.x, cmd.aabb.min.y, cmd.aabb.max.z },
            { cmd.aabb.max.x, cmd.aabb.max.y, cmd.aabb.max.z },
            { cmd.aabb.min.x, cmd.aabb.max.y, cmd.aabb.max.z }
        };

        const uint32 vertexOffset = boxIndex * NumBoxCorners;

        for (uint32 cornerIndex = 0; cornerIndex < NumBoxCorners; ++cornerIndex)
        {
            m_MappedBoxVertices[vertexOffset + cornerIndex].position = corners[cornerIndex];

            m_MappedBoxVertices[vertexOffset + cornerIndex].color = cmd.color;
        }
    }

    ID3D12GraphicsCommandList* cmdList = m_Renderer->GetCommandList();

    D3D12Shader* shader = static_cast<D3D12Shader*>(m_DebugShader.get());

    cmdList->SetPipelineState(shader->pso.Get());
    cmdList->SetGraphicsRootSignature(shader->rootSignature.Get());

    // Bind frame camera constants at b0.
    ID3D12Resource* frameConstantBuffer = m_Renderer->GetFrameConstantBuffer();

    cmdList->SetGraphicsRootConstantBufferView(0, frameConstantBuffer->GetGPUVirtualAddress());

    // Draw the same 12-edge index pattern for each box.
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    cmdList->IASetVertexBuffers(0, 1, &m_BoxVertexBufferView);
    cmdList->IASetIndexBuffer(&m_BoxIndexBufferView);

    for (uint32 boxIndex = 0; boxIndex < numBoxes; ++boxIndex)
    {
        const INT baseVertex = static_cast<INT>(boxIndex * NumBoxCorners);

        cmdList->DrawIndexedInstanced(NumBoxIndices, 1, 0, baseVertex, 0);
    }

    if (m_AABBCommands.size() > MaxDebugAABBs)
    {
        N_LOG("Debug renderer AABB capacity exceeded. Rendering %u of %u boxes.", numBoxes, static_cast<uint32>(m_AABBCommands.size()));
    }

    m_AABBCommands.clear();

    // Draw Other Shapes...
}
