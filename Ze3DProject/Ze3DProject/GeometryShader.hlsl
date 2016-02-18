struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
};

[maxvertexcount(3)]
void main( triangle GSInput input[3], inout TriangleStream< PSInput > output
)
{
	GSOutput element;
	float3 normal;

	float3 edge0 = input[1].pos.xyz - input[0].pos.xyz;
	float3 edge1 = input[2].pos.xyz - input[0].pos.xyz;
	normal = cross(edge0, edge1);

	for (uint i = 0; i < 3; i++)
	{
		element.position = input[i].position;
		element.tex = input[i].tex;
		element.worldPos = input[i].worldPos;
		element.normal = normal;

		output.Append(element);
	}
}