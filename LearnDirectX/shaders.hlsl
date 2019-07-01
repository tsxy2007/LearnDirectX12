

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer Projection : register(b0)
{
	float4x4 GProjectionMatrix;
};

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float  cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

PSInput VSMain(float4 position : POSITION,float4 Color, float2 uv : TEXCOORD)
{
	PSInput result;

	result.position = mul(GProjectionMatrix, position);
	result.color = Color;
	//result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
	//return g_texture.Sample(g_sampler, input.uv);
}
