Texture2D shaderTexture;
SamplerState shaderSampler;

cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 color;
	bool hasTexture;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

float4 main(PixelInput input) : SV_TARGET
{
	float3 s;	//Texture color for the current pixel

	if (hasTexture) {
		s = shaderTexture.Sample(shaderSampler, input.tex).rgb;
	}
	else{
		s = color;
	}
	
	//Ambient color
	float aRed = 0.2 * s.r;
	float aGreen = 0.2 * s.g;
	float aBlue = 0.2 * s.b;
	float3 ambient = float3(aRed, aGreen, aBlue);
	
	//Calculate Diffuse color
	float3 outVec = (float4(0,5,-6,1) - input.worldPos).xyz;	//(0,0,-6,1)Position of light i worldspace, camera is at (0,0,-5)
	outVec = normalize(outVec);
	float value = saturate(dot(input.normal, outVec));

	float dRed = (0.8 * s.r * value);
	float dGreen = (0.8 * s.g * value);
	float dBlue = (0.8 * s.b * value);
	float3 diffuse = float3(dRed, dGreen, dBlue);

	float3 result = ambient + diffuse;

	return float4(result, 1.0f);
}