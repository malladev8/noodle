Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

cbuffer ObjectConstants : register(b1)
{
    float4x4 gWorld;
    float4 gColor;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    return texColor * gColor;
}