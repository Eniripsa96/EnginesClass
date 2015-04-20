struct VertexOutput
{
	float4 initialPos	: SV_POSITION;
	float4 initialVel	: VELOCITY;
	float2 size			: SIZE;	// Storing age in y
	//float age : AGE;
	//unsigned int type : TYPE;
};

[maxvertexcount(4)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout PointStream<VertexOutput> output)
{
	// Initialize vertex to all the same values
	VertexOutput vert;
	vert.initialPos = input[0].initialPos - (input[0].initialVel * 0.0001f);
	vert.initialVel = input[0].initialVel;
	vert.size = input[0].size;
	
	// Increment height each time
	//vert.initialPos.y += 0.0001f;

	// Append :)
	output.Append(vert);
}