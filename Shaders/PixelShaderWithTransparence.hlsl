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
float4 PS(PS_INPUT input) : SV_Target0
{
    float4 albedo;
    albedo = tex.Sample(samplerLinear, input.TexCoords);
    float4 albedo2;
    albedo2 = tex2.Sample(samplerLinear, input.TexCoords);

    if (albedo2.a < 0.5)
    {
        discard;
    }
    return float4(albedo.rgb, 1.0);
}

float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}