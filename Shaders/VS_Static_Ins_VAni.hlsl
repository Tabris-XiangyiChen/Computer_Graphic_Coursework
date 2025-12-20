cbuffer PerFrameBuffer : register(b2)
{
float time;
float3 cam_pos;
};

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
    float4x4 World : WORLD;
};


struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};

PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output;
    

    float heightFactor = input.Pos.y;

    float wind = 0.0;
    wind += sin(time * 1.5 + input.Pos.x * 3.0) * 0.15;
    wind += sin(time * 2.0 + input.Pos.z * 2.0) * 0.1;

    float3 windOffset = float3(wind * 0.3, 0.0, wind * 0.2) * heightFactor;

    float4 worldPos = mul(float4(input.Pos.xyz + windOffset, 1.0), input.World);
    output.Pos = mul(worldPos, VP);
    
    float3 windNormal = normalize(input.Normal + float3(wind * 0.1, 0.0, 0.0));
    output.Normal = windNormal;
    
    output.Tangent = input.Tangent;
    output.TexCoords = input.TexCoords;
    
    return output;
}

float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}