
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float4 specColor;
	bool hasTexture;
	bool hasNormMap;
	//  padding 8 bytes
	float4 cameraPos;
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float3 viewDir : NORMAL1;
};

GSInput main(VertexInput input)
{
	GSInput output;
	float4 viewPos;

	//Add homogencoordinates for proper matrix multiplication
	input.position.w = 1.0f;

	//Multiply the position with world-, view- and projectionmatrix
	//Save the world-pos of the vertex
	output.position = output.worldPos = mul(input.position, worldMatrix);
	output.position = viewPos = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//Store the color for output
	output.tex = input.tex;

	//Store the normal for output
	output.normal = normalize(mul(input.normal, worldMatrix));

	output.viewDir = normalize(cameraPos.xyz - output.worldPos.xyz);

	return output;
}