
Texture2D myTexture : register(t0);
SamplerState mySampler: register(s0);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct PSInput
{
	float4 position		: SV_POSITION;
	//float4 initialPos	: VELOCITY;
	float2 uv			: TEXCOORD0;
	float3 color		: COLOR;
};

// Entry point for this pixel shader
float4 main(PSInput input) : SV_TARGET
{
	// Return color sampled from texture
	//return myTexture.Sample(mySampler, input.uv);
	return float4(input.color, 1.0f);
}
