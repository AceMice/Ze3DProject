struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 norm : NORMAL;
	float4 worldPos : POSITION;
};

[maxvertexcount(3)]
void main(
	triangle GSInput input[3] : SV_POSITION, 
	inout TriangleStream< PSInput > output
)
{
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}