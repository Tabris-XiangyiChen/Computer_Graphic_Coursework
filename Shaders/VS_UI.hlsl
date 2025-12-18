struct VS_INPUT
{
float2 pos : POSITION;
float2 uv : TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT o;
    o.pos = float4(input.pos.xy, 0.0f, 1.0f);
    o.uv = input.uv;
    return o;
}