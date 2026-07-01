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
    float2 uv : UV;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    
    //float4 worldPos = mul(gWorld, float4(input.pos, 1));
    //float4 viewPos = mul(gView, worldPos);
    //float4 clipPos = mul(gProjection, viewPos);
    //output.pos = clipPos;
    
    //output.pos = float4(input.pos.xy, 0, 1);
    
    output.pos = mul(gProjection, mul(gView, mul(gWorld, float4(input.pos, 1))));
    
    output.uv = input.uv;
    return output;
}