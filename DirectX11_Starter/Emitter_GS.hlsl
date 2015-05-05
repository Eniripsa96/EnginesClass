Texture1D randomTex : register(t0);
SamplerState mySampler : register(s0);

struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

[maxvertexcount(50)]
void main(point VertexOutput input[1], inout PointStream<VertexOutput> output)
{
	VertexOutput vert;

	[unroll]
	for (int i = 0; i < 50; i++)
	{
		vert.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
		vert.initialVel = randomTex.SampleLevel(mySampler, i / 500.0f, 0);
		vert.initialVel.w = 1.0f;
		vert.size = input[0].size;
		vert.color = input[0].color;

		output.Append(vert);
	}
}