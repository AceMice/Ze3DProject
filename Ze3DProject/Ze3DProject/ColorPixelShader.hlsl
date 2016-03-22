Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture: register(t3);
Texture2D shadowTexture: register(t4);
SamplerState pointSampler : register(s0);
SamplerState shadowSampler : register(s1);

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 lightPosition : TEXCOORD1;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 color;
	float4 normal;
	float4 specular;
	float4 depth;
	float4 outputColor;

	float lightIntensity = 0.0f;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.001f;

	color = colorTexture.Sample(pointSampler, input.tex);
	normal = normalTexture.Sample(pointSampler, input.tex);
	specular = specularTexture.Sample(pointSampler, input.tex);
	depth = depthTexture.Sample(pointSampler, input.tex);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = (input.lightPosition.x / input.lightPosition.w) * 0.5f + 0.5f;
	projectTexCoord.y = (-input.lightPosition.y / input.lightPosition.w) * 0.5f + 0.5f;

	// Calculate the depth of the light.
	lightDepthValue = input.lightPosition.z / input.lightPosition.w;

	// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	depthValue = shadowTexture.Sample(shadowSampler, projectTexCoord).r;

	// Subtract the bias from the lightDepthValue.
	lightDepthValue = lightDepthValue - bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
	if (lightDepthValue < depthValue)
	{
		// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(normal.xyz, depth.xyz));
	}

	outputColor = saturate(((color.rgba + specular.rgba) * lightIntensity * 0.8f) + ((color.rgba + specular.rgba) * 0.2f));

	return depthValue;
}