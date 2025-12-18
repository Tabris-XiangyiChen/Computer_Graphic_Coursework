Texture2D uiTex : register(t0);
SamplerState samp : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    return uiTex.Sample(samp, input.uv);
}
