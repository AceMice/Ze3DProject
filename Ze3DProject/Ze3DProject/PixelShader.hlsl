Texture2D shaderTexture;
SamplerState shaderSampler;

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
	//return shaderTexture.Sample(shaderSampler, input.tex);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}