Texture1D randomTex : register(t0);
SamplerState mySampler : register(s0);

cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 camPos;
	float4 age;
};

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
		vert.initialVel = randomTex.SampleLevel(mySampler, i / 40.0f + age.y * 500.0f, 0);	// Best option currently, looks random each emit
		//vert.initialVel = randomTex.SampleLevel(mySampler, i / 50.0f + age.y * 50.0f, 0);	// Decent but tends to be nearly the same every emit
		//vert.initialVel = randomTex.SampleLevel(mySampler, i / 50.0f, 0); // Decent but same every emit
		vert.initialVel.w = 1.0f;
		vert.size = input[0].size;
		vert.color = input[0].color;

		output.Append(vert);
	}
}