
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
	float3 tangent : TANGENT;
	float4 worldPos : POSITION;
};

[maxvertexcount(3)]
void main( triangle GSInput input[3], inout TriangleStream< PSInput > output)
{
	bool cull = true;
	float3 normal;

	

	float3 edge0 = input[1].position.xyz - input[0].position.xyz;
	float3 edge1 = input[2].position.xyz - input[0].position.xyz;

	normal = normalize(cross(edge0, edge1));

	for (uint i = 0; i < 3; i++)
	{
		if (dot(input[i].normal, input[i].viewDir) >= -0.1f) {
			cull = false;
		}
	}


	if (true) {
		PSInput element;
		float3 tangent;
		
		float2 texEdge0 = input[1].tex - input[0].tex;
		float2 texEdge1 = input[2].tex - input[0].tex;

		tangent.x = (texEdge0.y * edge0.x - texEdge1.y * edge1.x) * (1.0f / (texEdge0.x * texEdge1.y - texEdge1.x * texEdge0.y));
		tangent.y = (texEdge0.y * edge0.y - texEdge1.y * edge1.y) * (1.0f / (texEdge0.x * texEdge1.y - texEdge1.x * texEdge0.y));
		tangent.z = (texEdge0.y * edge0.z - texEdge1.y * edge1.z) * (1.0f / (texEdge0.x * texEdge1.y - texEdge1.x * texEdge0.y));

		tangent = normalize(tangent);



		for (uint i = 0; i < 3; i++)
		{
			//element.position = float4(input[i].position.xyz + normal, input[i].position.w); //Wanted to see if I could make it explode, dissapeared..
			element.position = input[i].position;
			element.tex = input[i].tex;
			element.worldPos = input[i].worldPos;
			element.normal = input[i].normal;
			if (input[i].normal.x == 0 && input[i].normal.y == 0 && input[i].normal.z == 0) {
				element.normal = normal;
			}
			element.tangent = tangent;

			output.Append(element);
		}
	}
}