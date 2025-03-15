#include "stdafx.h"
#include "CTextureManager.h"

// 싱글턴 인스턴스
CTextureManager* CTextureManager::s_instance = nullptr;

CTextureManager::CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context)
    : Device(device), Context(context) {
}

CTextureManager::~CTextureManager() {
    // 자원 해제
    for (auto& texture : m_textureMap) {
        if (texture.second) {
            texture.second->Release();
        }
    }
}

// 싱글턴 인스턴스 얻기
CTextureManager* CTextureManager::GetInstance(ID3D11Device* device, ID3D11DeviceContext* context) {
    if (s_instance == nullptr) {
        // 최초 호출 시 인스턴스 생성
        if (device == nullptr || context == nullptr) {
            throw std::runtime_error("Device and context must be provided during the first call.");
        }
        s_instance = new CTextureManager(device, context);
    }
    return s_instance;
}

// 텍스처 로드
void CTextureManager::LoadSetTexture(ID3D11DeviceContext* DeviceContext) {
    ScratchImage image;


    for (auto path : TexturePaths)
    {
        HRESULT hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
        if (FAILED(hr)) {
            return;
        }

        const TexMetadata& metadata = image.GetMetadata();

        ID3D11ShaderResourceView* textureView = nullptr;
        hr = CreateShaderResourceView(Device, image.GetImages(), image.GetImageCount(), metadata, &textureView);
        if (FAILED(hr)) {
            return;
        }

        m_textureMap[path] = textureView;

     
       BindTextureToShader(DeviceContext, textureView, slotCount++);  // 0번 슬롯에 텍스처 바인딩
    }
}

// 텍스처 바인딩 (동적으로 적용)
void CTextureManager::BindTexture(const std::wstring& textureName, UINT slot) {
    auto it = m_textureMap.find(textureName);
    if (it != m_textureMap.end()) {
        // 셰이더에 텍스처를 바인딩
        Context->PSSetShaderResources(slot, 1, &it->second);
    }
}


ID3D11ShaderResourceView* CTextureManager::GetTextureView(const std::wstring& filePath) {
    auto it = m_textureMap.find(filePath);
    if (it != m_textureMap.end()) {
        return it->second;  // 텍스처가 이미 로드되어 있으면 그 텍스처 뷰 반환
    }
    return nullptr;  // 텍스처가 로드되지 않았다면 nullptr 반환
}

// 셰이더에 텍스처 바인딩 함수
void CTextureManager::BindTextureToShader(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView, UINT slot) {
    context->PSSetShaderResources(slot, 1, &textureView);  // 픽셀 셰이더에 텍스처 바인딩
    context->PSSetSamplers(0, 1, &SamplerState);
}

void CTextureManager::CreateSamplerState(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  // 선형 필터링
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;  // 래핑 모드
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(&sampDesc, &SamplerState);
    if (FAILED(hr))
    {
        printf("Failed to create sampler state\n");
    }
}

