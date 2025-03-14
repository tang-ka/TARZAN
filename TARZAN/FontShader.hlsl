// �ؽ�ó ��ǥ�� ���� ��ǥ�� ó���ϴ� ����ü
struct VS_INPUT
{
    float3 position : POSITION; // 3D ��ġ
    float2 texCoord : TEXCOORD; // �ؽ�ó ��ǥ
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // ��ȯ�� ���� ��ġ
    float2 texCoord : TEXCOORD; // ��ȯ�� �ؽ�ó ��ǥ
};

// ī�޶��� View ��İ� �������� ���
cbuffer ViewProjection : register(b0)
{
    matrix view;
    matrix projection;
};

// ���� ��ǥ�� ó���ϴ� �Լ�
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // �ؽ�Ʈ�� �׻� ī�޶� ���ϰ� �ؾ� �ϹǷ�, ī�޶� ���⿡ ���� ȸ����Ŵ
    float4x4 world = mul(view, projection); // ���� ���

    // �ؽ�Ʈ�� ��ġ ��� (ī�޶� �ٶ󺸰�)
    output.position = mul(float4(input.position, 1.0f), world);
    
    // �ؽ�ó ��ǥ ����
    output.texCoord = input.texCoord;

    return output;
}

// �ؽ�ó�� �ؽ�ó ��ǥ�� ó���ϴ� ����ü
sampler2D fontSampler : register(s0);
Texture2D fontTexture : register(t0);

// �ȼ� ������ ó���ϴ� �Լ�
float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    // �ؽ�ó���� �ȼ� ���� ������
    float4 color = fontTexture.Sample(fontSampler, texCoord);

    // �ؽ�Ʈ ��Ʋ�󽺿��� ���ڿ� �ش��ϴ� �κ��� ��������
    if (color.a < 0.1)
        discard; // ���İ��� ������ �ؽ�Ʈ�� ������ �ʰ� �� (������ �κ��� ����)

    return color; // ���� ��ȯ
}
