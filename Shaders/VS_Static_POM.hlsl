cbuffer staticMeshBuffer : register(b1)
{
    float4x4 W;
    float4x4 VP;
};

struct VS_INPUT
{
    float4 Pos : POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Pos_w : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};


PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    output.Pos_w = input.Pos;
    output.Pos = mul(input.Pos, W);
    output.Pos = mul(output.Pos, VP);
    output.Normal = mul(input.Normal, (float3x3) W);
    output.Tangent = mul(input.Tangent, (float3x3) W);
    output.TexCoords = input.TexCoords;
    return output;
}
