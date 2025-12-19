cbuffer PerFrameBuffer : register(b2)
{
    float time;
    float3 cam_pos;
};

Texture2D tex : register(t0);
Texture2D normalsTexture : register(t1);
Texture2D rmaxtex : register(t2);
SamplerState samplerLinear : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 Pos_w : TEXCOORD1;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};
static const float3 LIGHT_DIRECTION = normalize(float3(0, -1, -1));
static const float3 LIGHT_COLOR = float3(1.0, 0.8, 1.0);
static const float3 AMBIENT_COLOR = float3(1.0, 1.0, 1.0);

static const int NUM_STEPS = 24;
static const float HEIGHT_SCALE = 0.05f;


float4 PS(PS_INPUT input) : SV_Target0
{
    float3 normal = normalize(input.Normal);
    float3 tangent = normalize(input.Tangent);
    float3 binormal = normalize(cross(normal, tangent));
    float3x3 TBN = float3x3(tangent, binormal, normal);

    float3 viewDirWorld = normalize(cam_pos - input.Pos_w.xyz);
    float3 viewDirTS = mul(viewDirWorld, transpose(TBN));

    float3 scaledViewDir = viewDirTS * HEIGHT_SCALE;
    float stepSize = 1.0 / NUM_STEPS;
    
    float2 currentTexCoords = input.TexCoords;
    float currentHeight = 0;
    for (int i = 0; i < NUM_STEPS; i++)
    {
        float sampledHeight = normalsTexture.Sample(samplerLinear, currentTexCoords).a;
        if (sampledHeight > currentHeight)
            break;
        currentTexCoords -= scaledViewDir.xy * stepSize;
        currentHeight += stepSize;
    }


    float4 albedo = tex.Sample(samplerLinear, currentTexCoords);
    if (albedo.a < 0.5)
    {
        discard;
    }

    float3 mapNormal = normalsTexture.Sample(samplerLinear, currentTexCoords).xyz;
    float3 tangentNormal = normalize(mapNormal * 2.0 - 1.0);
    float3 worldNormal = normalize(mul(tangentNormal, TBN));

    float diffuseIntensity = max(0, dot(worldNormal, -LIGHT_DIRECTION));
    float3 diffuse = LIGHT_COLOR * diffuseIntensity * 1.5;
    float3 ambient = AMBIENT_COLOR;

    float3 finalColor = albedo.rgb * (ambient + diffuse);
    return float4(finalColor, albedo.a);
}