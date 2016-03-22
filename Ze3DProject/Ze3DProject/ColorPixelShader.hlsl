Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D worldPosTexture: register(t3);
Texture2D shadowTexture: register(t4);
SamplerState pointSampler : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer MatrixBufferSimple
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	float4 camPos;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float4 color;
	float4 normal;
	float4 specColor;
	float4 worldPos;
	float4 outputColor;

	float lightIntensity = 0.0f;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float4 lightPos;

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.001f;

	color = colorTexture.Sample(pointSampler, input.tex);
	normal = normalTexture.Sample(pointSampler, input.tex);
	specColor = specularTexture.Sample(pointSampler, input.tex);
	worldPos = worldPosTexture.Sample(pointSampler, input.tex);

	float3 outVec = normalize(float3(25, 15, -6) - (worldPos).xyz);

	float3 refVec = normalize(reflect(-outVec, normal));	//Create the the reflection

	float3 viewDir = normalize(mul(camPos, worldMatrix) - worldPos).xyz;

	float specIntesity = saturate(dot(refVec, viewDir));
	float shineFactor = 5;
	float lightSpecular = 0.5;

	float4 specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0), 1.0f);

	lightPos = mul(worldPos, lightViewMatrix);
	lightPos = mul(lightPos, lightProjectionMatrix);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = (lightPos.x / lightPos.w) * 0.5f + 0.5f;
	projectTexCoord.y = (-lightPos.y / lightPos.w) * 0.5f + 0.5f;

	//If point outside shadowmap -> cast no shadow
	if ((saturate(projectTexCoord.x) != projectTexCoord.x) || (saturate(projectTexCoord.y) != projectTexCoord.y)) {
		lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
	}
	// Calculate the depth of the light.
	lightDepthValue = lightPos.z / lightPos.w;

	// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	depthValue = shadowTexture.Sample(shadowSampler, projectTexCoord).r;

	// Subtract the bias from the lightDepthValue.
	lightDepthValue = lightDepthValue - bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	// If the light is in front of the object then light the pixel, if not then shadow this pixel since an object (occluder) is casting a shadow on it.
	if (lightDepthValue < depthValue)
	{
		// Calculate the amount of light on this pixel.
		lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
	}

	outputColor = saturate(((color.rgba + specular.rgba) * lightIntensity * 0.8f) + ((color.rgba + specular.rgba) * 0.2f));

	return outputColor;
}