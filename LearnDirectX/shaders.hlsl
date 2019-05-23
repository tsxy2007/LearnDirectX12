
struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);

SamplerState s_2D = sampler_state  // ����һ������������ 
{
	Texture = <tex0>;         // ָ�������������� 
	MinFilter = Linear;       // ������˷�ʽ 
	MagFilter = Linear;
	AddressU = Wrap;          // ����Ѱַģʽ 
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
