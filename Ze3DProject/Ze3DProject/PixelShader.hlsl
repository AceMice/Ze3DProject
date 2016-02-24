Texture2D shaderTexture;
SamplerState shaderSampler;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 color;
	bool hasTexture;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
	if (hasTexture) {
		return shaderTexture.Sample(shaderSampler, input.tex);
	}

	return color;
}