
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 color;
	bool hasTexture;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	//float3 normal : NORMAL;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
};

GSInput main(VertexInput input)
{
	GSInput output;

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