cbuffer TransformBuffer : register(b0)
{
    float4x4 wvp;
};

struct VSInput
{
    float3 pos : POSITION;
    float2 uv : UV;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.pos = mul(wvp, float4(input.pos, 1));
    output.uv = input.uv;
    return output;
}