cbuffer FrameConstants : register(b0)
{
    float4x4 gView;
    float4x4 gProjection;
    float4x4 gViewProjection;
};

cbuffer ObjectConstants : register(b1)
{
    float4x4 gWorld;
    float4 gColor;
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
    output.pos = mul(float4(input.pos, 1.0f), mul(gWorld, gViewProjection));
    output.uv = input.uv;
    return output;
}