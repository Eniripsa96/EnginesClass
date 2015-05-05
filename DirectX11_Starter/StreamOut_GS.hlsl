struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float3 color		: COLOR;
};

[maxvertexcount(1)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout PointStream<VertexOutput> output)
{
	// Initialize vertex to all the same values
	VertexOutput vert;
	vert.position = input[0].position - (input[0].initialVel * 0.001f);
	vert.initialVel = input[0].initialVel;
	vert.size = input[0].size;
	vert.color = input[0].color;
	
	// Increment height each time
	//vert.initialPos.y += 0.0001f;

	// Append :)
	output.Append(vert);
}