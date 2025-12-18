Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samplerLinear : register(s0);
struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float2 TexCoords : TEXCOORD;
};
static const float3 LIGHT_DIRECTION = normalize(float3(0, -1, -1));
static const float3 LIGHT_COLOR = float3(1.0, 0.8, 1.0);
static const float3 AMBIENT_COLOR = float3(0.8, 0.8, 0.8);
float4 PS(PS_INPUT input) : SV_Target0
{
    float4 textureColor = tex.Sample(samplerLinear, input.TexCoords);
    
    float3 normal = normalize(input.Normal);
    
    float diffuseIntensity = max(0, dot(normal, -LIGHT_DIRECTION));
    
    float DIFFUSE_MULTIPLIER = 1.5;
    
    float3 ambient = AMBIENT_COLOR;
    float3 diffuse = LIGHT_COLOR * diffuseIntensity * DIFFUSE_MULTIPLIER;
    
    float3 finalColor = textureColor.rgb * (ambient + diffuse);
    
    return float4(finalColor, textureColor.a);
}

float4 main() : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
