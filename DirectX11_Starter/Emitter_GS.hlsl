Texture1D randomTex : register(t0);
SamplerState mySampler : register(s0);

struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

[maxvertexcount(4)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout PointStream<VertexOutput> output)
{
	VertexOutput vert;
	//float2 uv = float2(age.y * 1000.0f, (1.0f - age.y) * 1000.0f);
	//float4 v = randomTex.SampleLevel(mySampler, uv, 0);
	//float2 uv;

	[unroll]
	for (int i = 0; i < 50; i++)
	{
		vert.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
		
		//uv = float2(i * 1000.f, i + 2);
		//vert.initialVel = input[0].initialVel;
		//vert.initialVel = float4(v.x, v.y, v.z, 1.0f);
		vert.initialVel = randomTex.SampleLevel(mySampler, i / 500.0f, 0);
		vert.initialVel.w = 1.0f;

		vert.size = input[0].size;
		vert.color = input[0].color;
		//vert.color = float4(v);

		output.Append(vert);
	}
}