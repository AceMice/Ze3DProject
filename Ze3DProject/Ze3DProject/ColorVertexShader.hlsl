cbuffer MatrixBufferSimple
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	float4 camPos;
	float4 lightPos;
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
};

PSInput main(VertexInput input)
{
	PSInput output;

	//Add homogencoordinates for proper matrix multiplication
	input.position.w = 1.0f;

	//Multiply the position with world-, view- and projectionmatrix
	//Save the world-pos of the vertex
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Store the uv for output
	output.tex = input.tex;

	return output;
}