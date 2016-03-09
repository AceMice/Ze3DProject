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
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 worldPos : POSITION;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 s;	//Texture color for the current pixel

	if (hasTexture) {
		s = shaderTexture.Sample(shaderSampler, input.tex).rgba;
	}
	else{
		s = difColor;
	}
	if (hasNormMap) {
		//Load normals from normal map
		float4 normalMap = shaderNormalMap.Sample(shaderSampler, input.tex);

		//Change range from 0 - 1 to -1 - 1
		normalMap = 1.0f - (normalMap * 2.0f);

		//Make sure the tangent and normal are completly orthogonal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

		//Create the bitangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "texture space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store it in input.normal
		input.normal = normalize(mul(normalMap, texSpace));
	}
	
	float3 outVec = (float4(0,5,-6,1) - input.worldPos).xyz;	//(0,0,-6,1)Position of light i worldspace, camera is at (0,0,-5)
	outVec = normalize(outVec);
	float value = saturate(dot(input.normal, outVec));

	//Calculate new color
	s.r = (0.8 * s.r * value) + (0.2 * s.r);
	s.g = (0.8 * s.g * value) + (0.2 * s.g);
	s.b = (0.8 * s.b * value) + (0.2 * s.b);

	return s;
}