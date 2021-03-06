Texture1D randomTex : register(t0);
SamplerState mySampler : register(s0);

cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 camPos;
	float4 spawnPos;
	float4 misc;
};

struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

[maxvertexcount(75)]
void main(point VertexOutput input[1], inout PointStream<VertexOutput> output)
{
	VertexOutput vert;
	float div = misc.z * 0.8f;
	float offset = misc.y * (misc.z * 10.0f);
	int max = (int)(misc.z);

	//[unroll]
	for (int i = 0; i < max; i++)
	{
		vert.position = spawnPos;
		vert.initialVel = randomTex.SampleLevel(mySampler, i / div + offset, 0);	// Best option currently, looks random each emit
		//vert.initialVel = randomTex.SampleLevel(mySampler, i / 50.0f + age.y * 50.0f, 0);	// Decent but tends to be nearly the same every emit
		//vert.initialVel = randomTex.SampleLevel(mySampler, i / 50.0f, 0); // Decent but same every emit
		vert.initialVel.w = 1.0f;
		vert.size = input[0].size;
		vert.color = input[0].color;

		output.Append(vert);
	}
}