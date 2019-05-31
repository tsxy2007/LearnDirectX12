

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer Projection : register(b0)
{
	float4x4 GProjectionMatrix;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

PSInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
	PSInput result;

	result.position = mul(GProjectionMatrix, position);
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return g_texture.Sample(g_sampler, input.uv);
}
