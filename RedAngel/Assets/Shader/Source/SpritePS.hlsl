Texture2D DiffuseTexture : register(t0);
SamplerState LinearSampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    return DiffuseTexture.Sample(LinearSampler, input.uv);
}