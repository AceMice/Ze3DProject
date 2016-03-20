Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
SamplerState pointSampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main() : SV_TARGET
{
	float4 colors;
	float4 normals;
	float4 speculars;
	float4 outputColor;

	colors = colorTexture.Sample(pointSampler, input.tex);
	normals = normalTexture.Sample(pointSampler, input.tex);
	speculars = specularTexture.Sample(pointSampler, input.tex);

	outputColor = saturate(colors.rgb + speculars.rgb);

	return outputColor;
}