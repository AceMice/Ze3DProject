cbuffer MatrixBufferSimple
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 lightPosition : TEXCOORD1;
};

PSInput main(VertexInput input)
{
	PSInput output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.lightPosition = mul(input.position, worldMatrix);
	output.lightPosition = mul(output.lightPosition, lightViewMatrix);
	output.lightPosition = mul(output.lightPosition, lightProjectionMatrix);

	//Store the uv for output
	output.tex = input.tex;

	return output;
}