
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	//Add homogencoordinates for proper matrix multiplication
	input.position.w = 1.0f;

	//Multiply the position with world-, view- and projectionmatrix
	//Save the world-pos of the vertex
	output.position = output.worldPos = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Store the color for output
	output.tex = input.tex;

	return output;
}