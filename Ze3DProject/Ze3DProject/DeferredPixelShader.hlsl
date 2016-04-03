Texture2D shaderTexture;
Texture2D shaderNormalMap;
SamplerState shaderSampler;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float4 specColor;
	bool hasTexture;
	bool hasNormMap;
	bool picked;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 worldPos : POSITION;
};

struct PixelOutput
{
	float4 color : SV_Target0;
	float4 normal : SV_Target1;
	float4 specular : SV_Target2;
	float4 worldPos : SV_Target3;
};

PixelOutput main(PixelInput input) : SV_TARGET
{
	PixelOutput output;

	if (hasTexture) {
		output.color = shaderTexture.Sample(shaderSampler, input.tex).rgba;
	}
	else {
		output.color = difColor;
	}
	if (picked) { //Paint the model if picked
		output.color.r *= 0.25f;
		output.color.g *= 0.25f;
		output.color.b *= 1.5f;
	}
	if (hasNormMap) {
		//Load normals from normal map
		float4 normalMap = shaderNormalMap.Sample(shaderSampler, input.tex);

		//Change range from 0 - 1 to -1 - 1
		normalMap = (normalMap * 2.0f) - 1.0f;

		//Make sure the tangent and normal are completly orthogonal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

		//Create the bitangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "texture space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store it in input.normal
		output.normal = float4(normalize(mul(normalMap, texSpace)), 1.0f);
	}
	else {
		output.normal = float4(input.normal, 1.0f);
	}

	output.worldPos = input.worldPos;

	output.specular = specColor;

	return output;
}