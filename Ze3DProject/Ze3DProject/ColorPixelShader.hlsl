Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
SamplerState pointSampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 colors;
	float4 normals;
	float4 speculars;
	float4 outputColor;

	colors = colorTexture.Sample(pointSampler, input.tex);
	normals = normalTexture.Sample(pointSampler, input.tex);
	speculars = specularTexture.Sample(pointSampler, input.tex);

	float3 lightDir = float3(0.0f, 0.4f, -1.0f);

	float lightIntensity = saturate(dot(normals.xyz, lightDir));

	outputColor = saturate((colors.rgba + speculars.rgba) * lightIntensity);

	return outputColor;
}