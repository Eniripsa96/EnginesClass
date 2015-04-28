// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct PSInput
{
	float4 position		: SV_POSITION;
	float2 uv			: TEXCOORD0;
	float3 color		: COLOR;
};

// Entry point for this pixel shader
float4 main(PSInput input) : SV_TARGET
{
	// Return the color
	return float4(input.color, 1.0f);
}
