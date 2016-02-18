Texture2D shaderTexture;
SamplerState shaderSampler;

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 worldPos : POSITION;
};

float4 main(PSInput input) : SV_TARGET
{
	return shaderTexture.Sample(shaderSampler, input.tex);
}