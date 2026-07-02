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
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    output.pos = mul(gViewProjection, mul(gWorld, float4(input.pos, 1)));
    output.uv = input.uv;
    return output;
}