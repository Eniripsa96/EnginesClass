struct VertexOutput
{
	float4 position		: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;	// Storing age in y
	//float age : AGE;
	//unsigned int type : TYPE;
};

[maxvertexcount(4)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout PointStream<VertexOutput> output)
{
	[unroll]
	for (int i = 0; i < 50; i++)
	{
		VertexOutput vert;
		vert.position = float4(0.0f, 0.0f, 0.0f, 1.0f);
		vert.initialVel = input[0].initialVel;
		vert.size = input[0].size;

		output.Append(vert);
	}
}