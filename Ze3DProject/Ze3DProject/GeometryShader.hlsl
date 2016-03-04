
struct GSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 worldPos : POSITION;
	float3 normal : NORMAL;
	float3 viewDir : TEXCOORD1;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	float4 worldPos : POSITION;
	float3 viewDir : TEXCOORD1;
};

[maxvertexcount(3)]
void main( triangle GSInput input[3], inout TriangleStream< PSInput > output
)
{
	PSInput element;
	/*float3 normal;

	float3 edge0 = input[1].position.xyz - input[0].position.xyz;
	float3 edge1 = input[2].position.xyz - input[0].position.xyz;
	normal = cross(edge0, edge1);
	normal = normalize(normal);*/
	element.viewDir = input[0].viewDir;
	for (uint i = 0; i < 3; i++)
	{
		element.position = input[i].position;
		element.tex = input[i].tex;
		element.worldPos = input[i].worldPos;
		element.normal = input[i].normal;

		output.Append(element);
	}
}