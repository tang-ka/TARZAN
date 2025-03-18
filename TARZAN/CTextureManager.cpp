#include "stdafx.h"
#include "CTextureManager.h"

// 싱글턴 인스턴스
CTextureManager* CTextureManager::s_instance = nullptr;

CTextureManager::CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context)
    : Device(device), Context(context)
{
}

CTextureManager::~CTextureManager() {
    // ComPtr가 자동으로 Release()하므로 m_textureMap의 값에 대해 수동 해제 불필요.
}

// 싱글턴 인스턴스 얻기
CTextureManager* CTextureManager::GetInstance() {
    assert(s_instance != nullptr);
    return s_instance;
}

void CTextureManager::SetDeviceAndContext(ID3D11Device* device, ID3D11DeviceContext* context) {
    if (!s_instance) {
        if (!device || !context) {
            throw std::runtime_error("Device and context must be provided when initializing.");
        }
        s_instance = new CTextureManager(device, context);
    }
}

// 텍스처 로드 (DDS 포맷)
void CTextureManager::LoadSetTexture() {
    ScratchImage image;

    for (auto path : TexturePaths)
    {
        HRESULT hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
        if (FAILED(hr)) {
            // 실패 시 로그 출력 후 continue 또는 return (필요에 따라)
            continue;
        }

        const TexMetadata& metadata = image.GetMetadata();

        ComPtr<ID3D11ShaderResourceView> textureView;
        hr = CreateShaderResourceView(Device, image.GetImages(), image.GetImageCount(), metadata, textureView.GetAddressOf());
        if (FAILED(hr)) {
            continue;
        }

        m_textureMap[path] = textureView;
    }
}

// 텍스처의 셰이더 리소스 뷰 반환
ID3D11ShaderResourceView* CTextureManager::GetTextureView(const std::wstring& filePath) {
    auto it = m_textureMap.find(filePath);
    if (it != m_textureMap.end()) {
        return it->second.Get();  // ComPtr에서 raw pointer 반환
    }
    return nullptr;
}

// 텍스처를 셰이더에 바인딩 (EObjectType에 따라)
void CTextureManager::BindTextureToShader(EObjectType type) {
    //std::wstring path = TexturePaths[(int)type];
    const std::wstring& path = TexturePaths[(int)type];

    // PSSetShaderResources는 ID3D11ShaderResourceView* 배열를 기대합니다.
    ID3D11ShaderResourceView* srv = GetTextureView(path);
    Context->PSSetShaderResources(0, 1, &srv);
    // SamplerState는 ComPtr 내부의 raw pointer로 바인딩합니다.
    ID3D11SamplerState* sampler = SamplerState.Get();
    Context->PSSetSamplers(0, 1, &sampler);
}

// 샘플러 생성 함수
void CTextureManager::CreateSamplerState() {
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  // 선형 필터링
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // 랩 모드
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = Device->CreateSamplerState(&sampDesc, SamplerState.GetAddressOf());
    if (FAILED(hr))
    {
        OutputDebugString(L"Failed to create sampler state\n");
    }
}
