cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 camPos;
	float4 age;
};

struct VertexInput
{
	float3 position		: POSITION;
	float3 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.position = float4(input.position, 1.0f);
	output.initialVel = float4(input.initialVel, 1.0f);
	output.size = input.size;
	output.color = input.color;

	output.position = output.position - (output.initialVel * (age.z - age.x));

	return output;
}