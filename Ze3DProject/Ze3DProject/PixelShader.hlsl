Texture2D shaderTexture;
SamplerState shaderSampler;

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

float4 main(PSInput input) : SV_TARGET
{
	return shaderTexture.Sample(shaderSampler, input.tex);
}