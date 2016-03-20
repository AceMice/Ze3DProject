

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PSInput main(VertexInput input)
{
	PSInput output;

	output.position = input.position

	//Store the color for output
	output.tex = input.tex;

	return output;
}