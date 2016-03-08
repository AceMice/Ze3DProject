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
	
	float3 outVec = (float4(0,5,-6,1) - input.worldPos).xyz;	//(0,0,-6,1)Position of light i worldspace, camera is at (0,0,-5)
	outVec = normalize(outVec);
	float value = saturate(dot(input.normal, outVec));

	//Calculate new color
	s.r = (0.8 * s.r * value) + (0.2 * s.r);
	s.g = (0.8 * s.g * value) + (0.2 * s.g);
	s.b = (0.8 * s.b * value) + (0.2 * s.b);

	return s;
}