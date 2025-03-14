cbuffer TransformBuffer : register(b0)
{
    row_major matrix WorldMatrix; // 4x4 변환 행렬
};
cbuffer PrimitiveBuffer : register(b1)
{
    uint Flags;
};

// 텍스처 및 샘플러 상태 추가
Texture2D tex : register(t0); // 텍스처를 t0 슬롯에 바인딩
SamplerState samplerState : register(s0); // 샘플러를 s0 슬롯에 바인딩

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
    
    // 정점 좌표를 4D 벡터로 변환 후 행렬 곱
    output.Position = mul(float4(input.Position.xyz, 1.f), WorldMatrix);
    
        
    // 텍스처 좌표를 그대로 출력으로 전달
    output.TexCoord = input.TexCoord;
    
    //output.Position.z *= -1;
    //output.Position.xyz /= output.Position.w;
    //output.Position = float4(input.Position, 1.0f);
    
    // 정점 색상을 그대로 전달
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
     // 텍스처에서 샘플링하여 색상을 반환
    float4 textureColor = tex.Sample(samplerState, input.TexCoord); // 텍스처 샘플링
    
    // 텍스처 샘플링된 색상과 전달된 색상 혼합
    return textureColor;// * input.Color; // 텍스처 색상에 기존 색상을 곱함
}
