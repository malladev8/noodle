cbuffer FrameConstants : register(b0)
{
    float4x4 gView;
    float4x4 gProjection;
    float4x4 gViewProjection;
};

struct VSInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 worldPosition = float4(input.pos, 1.0f);
    output.pos = mul(gProjection, mul(gView, worldPosition));
    output.color = input.color;
    return output;
}