struct PSInput
{
	float4 position : SV_POSITION;
};

float4 main(PSInput input) : SV_TARGET
{
	return float4(0.0f, 0.0f, 0.0f, 0.0f);
}