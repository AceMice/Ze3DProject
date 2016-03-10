
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	float4 difColor;
	float4 specColor;
	bool hasTexture;
	bool hasNormMap;
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
	float3 viewDir : TEXCOORD1;
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
	
	//Store the normal for output
	output.normal = normalize(mul(input.normal, worldMatrix));

	output.viewDir = normalize(mul(cameraPos.xyz, worldMatrix) - output.worldPos.xyz);

	return output;
}