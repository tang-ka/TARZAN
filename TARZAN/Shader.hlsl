cbuffer TransformBuffer : register(b0)
{
    row_major matrix WorldMatrix; // 4x4 ��ȯ ���
};
cbuffer PrimitiveBuffer : register(b1)
{
    uint Flags;
};

// �ؽ�ó �� ���÷� ���� �߰�
Texture2D tex : register(t0); // �ؽ�ó�� t0 ���Կ� ���ε�
SamplerState samplerState : register(s0); // ���÷��� s0 ���Կ� ���ε�

struct VS_INPUT
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // ���� ��ǥ�� 4D ���ͷ� ��ȯ �� ��� ��
    output.Position = mul(float4(input.Position.xyz, 1.f), WorldMatrix);
    
        
    // �ؽ�ó ��ǥ�� �״�� ������� ����
    output.TexCoord = input.TexCoord;
    
    //output.Position.z *= -1;
    //output.Position.xyz /= output.Position.w;
    //output.Position = float4(input.Position, 1.0f);
    
    // ���� ������ �״�� ����
    if (Flags == 0)
    {
        output.Color = input.Color;
    }
    else if (Flags & 1 << 0)
    {
        output.Color = float4(0.f, 0.f, 0.f, 0.f);
    }
    else if (Flags & 1 << 1)
    {
        output.Color = float4(input.Color.rgb + float3(0.1, 0.5, 0.5), 1);
    }
    //output.Color = float4(-output.Position.zzz, 1.0f);
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
     // �ؽ�ó���� ���ø��Ͽ� ������ ��ȯ
    float4 textureColor = tex.Sample(samplerState, input.TexCoord); // �ؽ�ó ���ø�
    
    // �ؽ�ó ���ø��� ����� ���޵� ���� ȥ��
    return textureColor;// * input.Color; // �ؽ�ó ���� ���� ������ ����
}
