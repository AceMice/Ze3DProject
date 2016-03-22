struct PSInput
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
	float depth;
	float4 outputColor;

	depth = input.depthPosition.z / input.depthPosition.w;

	outputColor = float4(input.depthPosition.x, input.depthPosition.y, depth, input.depthPosition.w);

	return outputColor;
}