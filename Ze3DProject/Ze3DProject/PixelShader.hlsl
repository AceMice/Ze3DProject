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
	float4 cameraPos;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 worldPos : POSITION;
	float3 viewDir : TEXCOORD1;
};

float4 main(PixelInput input) : SV_TARGET
{

	float4 s;	//Texture color for the current pixel
	float ambientStr = 0.2;
	float diffuseStr = 0.8;
	float shineFactor = 5;
	float lightSpecular = 0.5;
	float3 specular = float3(0.0f, 0.0f, 0.0f);


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
		normalMap = (normalMap * 2.0f) - 1.0f;

		//Make sure the tangent and normal are completly orthogonal
		input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);

		//Create the bitangent
		float3 biTangent = cross(input.normal, input.tangent);

		//Create the "texture space"
		float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);

		//Convert normal from normal map to texture space and store it in input.normal
		input.normal = normalize(mul(normalMap, texSpace));
	}
	
	//Ambient color
	float3 ambient = float3(ambientStr * s.r, ambientStr * s.g, ambientStr * s.b);
	
	float3 outVec = normalize(float3(0, 5, -60) - (input.worldPos).xyz);	//lightVec towards the object
	
	//Specular
	float3 refVec = normalize(reflect(outVec, input.normal));	//Create the the reflection
	float lightIntesity = saturate(dot(refVec, input.viewDir));
	
	specular = specColor.rgb * lightSpecular * max(pow(lightIntesity, shineFactor), 0);
	
	
	
	//Calculate Diffuse color
	//float3 outVec = normalize(-inVec);	//Reversing the vector to point outwards
	float value = saturate(dot(input.normal, outVec));
	float3 diffuse = float3(diffuseStr * s.r * value, diffuseStr * s.g * value, diffuseStr * s.b * value);

	float3 result = saturate(ambient + diffuse + (value * specular));

	return float4(result, s.a);
}