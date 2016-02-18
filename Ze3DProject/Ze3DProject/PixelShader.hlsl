Texture2D shaderTexture;
SamplerState shaderSampler;

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

float4 main(PSInput input) : SV_TARGET
{
	float3 s = shaderTexture.Sample(shaderSampler, input.tex).rgb;	//Texture color for the current pixel
																//Calculate angle 															//Calculate angle 
	float3 outVec = (float4(0,0,-6,1) - input.worldPos).xyz;	//(0,0,-6,1)Position of light i worldspace, camera is at (0,0,-5)
	normalize(outVec);
	float value = saturate(dot(input.normal, outVec));

	//Calculate new color
	s.r = (0.8 * s.r * value) + (0.2 * s.r);
	s.g = (0.8 * s.g * value) + (0.2 * s.g);
	s.b = (0.8 * s.b * value) + (0.2 * s.b);

	return float4(s, 1.0f);
	//return shaderTexture.Sample(shaderSampler, input.tex);
}