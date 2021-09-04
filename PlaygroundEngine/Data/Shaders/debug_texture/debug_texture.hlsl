cbuffer cb : register(b0)
{
    matrix worldViewProj;
}

Texture2D tex2D : register(t0);
SamplerState tex2DSampler : register(s0);

struct VertexInput
{
    float3 positionL : POSITION0;
    float3 normalL : NORMAL0;
    float2 texCoord : TEXCOORD0;
};

struct PixelInput
{
    float4 positionH: SV_POSITION;
    float2 texCoord : TEXCOORD0;
};

PixelInput vsMain(VertexInput input)
{
    PixelInput output;

    output.positionH = mul(worldViewProj, float4(input.positionL, 1.0f));
    output.texCoord = input.texCoord;

    return output;
}

float4 psMain(PixelInput input) : SV_Target
{
    float4 color = tex2D.Sample(tex2DSampler, input.texCoord);

    return float4(color.a / 100.0f, 0.0f, 0.0f, 1.0f);
}