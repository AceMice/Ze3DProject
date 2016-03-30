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
	float4 lightPos;
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
	float4 positionLight;

	// Set the bias value for fixing the floating point precision issues.
	float bias = 0.00001f;

	color = colorTexture.Sample(pointSampler, input.tex);
	normal = normalTexture.Sample(pointSampler, input.tex);
	specColor = specularTexture.Sample(pointSampler, input.tex);
	worldPos = worldPosTexture.Sample(pointSampler, input.tex);

	float3 outVec = normalize(lightPos.xyz - (worldPos).xyz);

	float3 refVec = normalize(reflect(-outVec, normal));	//Create the the reflection

	float3 viewDir = normalize(camPos - worldPos).xyz;

	float specIntesity = saturate(dot(refVec, viewDir));
	float shineFactor = 5.0f;
	float lightSpecular = 0.65f;

	float4 specular = float4(specColor.rgb * lightSpecular * max(pow(specIntesity, shineFactor), 0.0f), 1.0f);

	positionLight = mul(worldPos, lightViewMatrix);
	positionLight = mul(positionLight, lightProjectionMatrix);

	// Calculate the projected texture coordinates.
	projectTexCoord.x = (positionLight.x / positionLight.w) * 0.5f + 0.5f;
	projectTexCoord.y = (-positionLight.y / positionLight.w) * 0.5f + 0.5f;

	//If point outside shadowmap -> cast no shadow
	if ((saturate(projectTexCoord.x) != projectTexCoord.x) || (saturate(projectTexCoord.y) != projectTexCoord.y)) {
		lightIntensity = saturate(dot(normal.xyz, outVec.xyz));
	}
	// Calculate the depth of the light.
	lightDepthValue = positionLight.z / positionLight.w;

	// Sample the shadow map depth value from the depth texture using the sampler at the projected texture coordinate location.
	/*float shadowEpsilon = 0.01f;
	float dx = 1.0f / 800.f;
	float s0 = (shadowTexture.Sample(shadowSampler, projectTexCoord).r + shadowEpsilon < lightDepthValue) ? 0.0f : 1.0f;
	float s1 = (shadowTexture.Sample(shadowSampler, projectTexCoord + float2(dx, 0.0f)).r + shadowEpsilon < lightDepthValue) ? 0.0f : 1.0f;
	float s2 = (shadowTexture.Sample(shadowSampler, projectTexCoord + float2(0.0f, dx)).r + shadowEpsilon < lightDepthValue) ? 0.0f : 1.0f;
	float s3 = (shadowTexture.Sample(shadowSampler, projectTexCoord + float2(dx, dx)).r + shadowEpsilon < lightDepthValue) ? 0.0f : 1.0f;*/
	depthValue = shadowTexture.Sample(shadowSampler, projectTexCoord).r;

	////Transform shadow map uv coord to texel space
	//float2 texelPos = projectTexCoord * 800.f;

	////Determine lerp amount, (340.3, 200.1) -> (0.3, 0.1)
	//float2 lerps = frac(texelPos);

	//float shadowCoeff = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerps.x), lerps.y);

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