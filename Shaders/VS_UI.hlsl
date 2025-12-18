cbuffer UI_Buffer : register(b3)
{
    float2 ui_pos;
    float2 ui_size;
}

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
    float2 ndc = input.pos * ui_size + ui_pos;
    o.pos = float4(ndc, 0.0f, 1.0f);
    o.uv = input.uv;
    return o;
}