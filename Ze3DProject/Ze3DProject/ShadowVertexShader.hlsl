
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 lightPos;
};

struct VertexInput
{
	float4 position : POSITION;
};

struct PSInput
{
	float4 position : SV_POSITION;
	/*float4 depthPosition : TEXCOORD0;*/
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

	//// Store the position value in a second input value for depth value calculations.
	//output.depthPosition = output.position;

	return output;
}