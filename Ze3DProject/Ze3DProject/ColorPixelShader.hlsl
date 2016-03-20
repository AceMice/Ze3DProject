Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture: register(t3);
SamplerState pointSampler : register(s0);

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 color;
	float4 normal;
	float4 specular;
	float4 depth;
	float4 outputColor;

	color = colorTexture.Sample(pointSampler, input.tex);
	normal = normalTexture.Sample(pointSampler, input.tex);
	specular = specularTexture.Sample(pointSampler, input.tex);
	depth = depthTexture.Sample(pointSampler, input.tex);

	float lightIntensity = saturate(dot(normal.xyz, depth.xyz));

	outputColor = saturate(((color.rgba + specular.rgba) * lightIntensity * 0.8f) + ((color.rgba + specular.rgba) * 0.2f));

	return outputColor;
}