Texture2D shaderTexture;
SamplerState shaderSampler;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float4 specColor;
	bool hasTexture;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
	float3 viewDir : TEXCOORD1;
};

float4 main(PixelInput input) : SV_TARGET
{
	float3 s;	//Texture color for the current pixel
	float ambientStr = 0.2;
	float diffuseStr = 0.8;
	float shineFactor = 5;
	float lightSpecular = 0.5;

	if (hasTexture) {
		s = shaderTexture.Sample(shaderSampler, input.tex).rgb;
	}
	else{
		s = difColor;
	}
	
	//Ambient color
	float3 ambient = float3(ambientStr * s.r, ambientStr * s.g, ambientStr * s.b);
	
	
	float3 outVec = normalize(float4(0, 5, -6, 1) - (input.worldPos).xyz);	//lightVec towards the object
	
	//Specular
	float3 refVec = normalize(reflect(outVec, input.normal));	//Create the the reflection
	float3 specular = specColor * lightSpecular * pow( dot( refVec, normalize(input.viewDir) ), shineFactor);
	
	//Calculate Diffuse color
	float value = saturate(dot(input.normal, outVec));
	float3 diffuse = float3(diffuseStr * s.r * value, diffuseStr * s.g * value, diffuseStr * s.b * value);

	float3 result = ambient + diffuse + specular;

	return float4(result, 1.0f);
}