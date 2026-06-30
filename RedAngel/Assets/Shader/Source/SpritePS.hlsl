Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    //return gTexture.Sample(gSampler, input.uv);
    return float4(1, 0, 1, 1);
}