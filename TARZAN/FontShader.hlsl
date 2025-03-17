// 텍스처 좌표와 월드 좌표를 처리하는 구조체
struct VS_INPUT
{
    float3 position : POSITION; // 3D 위치
    float2 texCoord : TEXCOORD; // 텍스처 좌표
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION; // 변환된 최종 위치
    float2 texCoord : TEXCOORD; // 변환된 텍스처 좌표
};

// 카메라의 View 행렬과 프로젝션 행렬
cbuffer ViewProjection : register(b0)
{
    matrix view;
    matrix projection;
};

// 월드 좌표를 처리하는 함수
VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // 텍스트는 항상 카메라를 향하게 해야 하므로, 카메라 방향에 맞춰 회전시킴
    float4x4 world = mul(view, projection); // 월드 행렬

    // 텍스트의 위치 계산 (카메라를 바라보게)
    output.position = mul(float4(input.position, 1.0f), world);
    
    // 텍스처 좌표 전달
    output.texCoord = input.texCoord;

    return output;
}

// 텍스처와 텍스처 좌표를 처리하는 구조체
sampler2D fontSampler : register(s0);
Texture2D fontTexture : register(t0);

// 픽셀 색상을 처리하는 함수
float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    // 텍스처에서 픽셀 값을 가져옴
    float4 color = fontTexture.Sample(fontSampler, texCoord);

    // 텍스트 아틀라스에서 문자에 해당하는 부분을 가져오기
    if (color.a < 0.1)
        discard; // 알파값이 낮으면 텍스트를 보이지 않게 함 (투명한 부분을 버림)

    return color; // 색상 반환
}
