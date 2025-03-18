#include "stdafx.h"
#include "CTextureManager.h"

// �̱��� �ν��Ͻ�
CTextureManager* CTextureManager::s_instance = nullptr;

CTextureManager::CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context)
    : Device(device), Context(context)
{
}

CTextureManager::~CTextureManager() {
    // ComPtr�� �ڵ����� Release()�ϹǷ� m_textureMap�� ���� ���� ���� ���� ���ʿ�.
}

// �̱��� �ν��Ͻ� ���
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

// �ؽ�ó �ε� (DDS ����)
void CTextureManager::LoadSetTexture() {
    ScratchImage image;

    for (auto path : TexturePaths)
    {
        HRESULT hr = LoadFromDDSFile(path.c_str(), DDS_FLAGS_NONE, nullptr, image);
        if (FAILED(hr)) {
            // ���� �� �α� ��� �� continue �Ǵ� return (�ʿ信 ����)
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

// �ؽ�ó�� ���̴� ���ҽ� �� ��ȯ
ID3D11ShaderResourceView* CTextureManager::GetTextureView(const std::wstring& filePath) {
    auto it = m_textureMap.find(filePath);
    if (it != m_textureMap.end()) {
        return it->second.Get();  // ComPtr���� raw pointer ��ȯ
    }
    return nullptr;
}

// �ؽ�ó�� ���̴��� ���ε� (EObjectType�� ����)
void CTextureManager::BindTextureToShader(EObjectType type) {
    //std::wstring path = TexturePaths[(int)type];
    const std::wstring& path = TexturePaths[(int)type];

    // PSSetShaderResources�� ID3D11ShaderResourceView* �迭�� ����մϴ�.
    ID3D11ShaderResourceView* srv = GetTextureView(path);
    Context->PSSetShaderResources(0, 1, &srv);
    // SamplerState�� ComPtr ������ raw pointer�� ���ε��մϴ�.
    ID3D11SamplerState* sampler = SamplerState.Get();
    Context->PSSetSamplers(0, 1, &sampler);
}

// ���÷� ���� �Լ�
void CTextureManager::CreateSamplerState() {
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  // ���� ���͸�
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;      // �� ���
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
