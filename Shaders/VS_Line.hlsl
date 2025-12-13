cbuffer staticMeshBuffer : register(b1)
{
    float4x4 W;
    float4x4 VP;
};

struct VS_INPUT
{
    float4 pos : POSITION;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT o;
    o.pos = mul(input.pos, W);
    o.pos = mul(o.pos, VP);
    return o;
}
float4 main( float4 pos : POSITION ) : SV_POSITION
{
	return pos;
}