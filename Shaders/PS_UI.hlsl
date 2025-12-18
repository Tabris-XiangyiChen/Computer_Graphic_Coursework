Texture2D tex : register(t0);
SamplerState samplerLinear : register(s0);

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 PS(PS_INPUT input) : SV_Target0
{
    float4 texColor = tex.Sample(samplerLinear, input.uv);
        
    if (texColor.a < 0.5)
    {
        discard;
    }
    return texColor;
}
