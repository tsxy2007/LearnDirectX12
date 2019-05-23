
struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);

SamplerState s_2D = sampler_state  // 声明一个采样器对象 
{
	Texture = <tex0>;         // 指定被采样的纹理 
	MinFilter = Linear;       // 纹理过滤方式 
	MagFilter = Linear;
	AddressU = Wrap;          // 纹理寻址模式 
	AddressV = Wrap;
};
PSInput VSMain(float4 position : POSITION, float2 uv : TEXCOORD)
{
	PSInput result;

	result.position = position;
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	return g_texture.Sample(s_2D,input.uv);
}
