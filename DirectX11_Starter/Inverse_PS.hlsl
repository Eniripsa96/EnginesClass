
Texture2D myTexture : register(t0);
Texture2D shadowMap : register(t1);
SamplerState mySampler : register(s0);
SamplerState shadowSampler : register(s1);

// Defines the input to this pixel shader
// - Should match the output of our corresponding vertex shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal		: TEXCOORD2;
	float2 uv			: TEXCOORD0;
	//float4 lightPos     : TEXCOORD1;
	float4 lightDir     : TEXCOORD3;
	float4 color        : COLOR;
};

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	//input.lightPos.xyz /= input.lightPos.w;
	//float shade = 1.0f;

	//input.lightPos.y = -input.lightPos.y;
	//input.lightPos.xy = input.lightPos.xy / 2 + 0.5f;

	//float depth = shadowMap.Sample(shadowSampler, input.lightPos.xy).r;
	//if (depth < input.lightPos.z - 0.0001f) {
	//	shade = 0.0f;
	//}

	// Return color sampled from texture
	float nDotL = dot(normalize(input.normal), -normalize((float3)input.lightDir));
	//float3 result = myTexture.Sample(mySampler, input.uv).rgb * (1 - input.lightDir.w + shade * input.lightDir.w * nDotL) * input.color.rgb;
	float3 result = myTexture.Sample(mySampler, input.uv).rgb * (1 - input.lightDir.w * input.lightDir.w * nDotL) * input.color.rgb;
	result = saturate(result);
	return float4(
		1 - result.r,
		1 - result.g,
		1 - result.b,
		input.color.a
	);
}