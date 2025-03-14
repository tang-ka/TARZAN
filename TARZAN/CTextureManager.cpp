#include "stdafx.h"
#include "CTextureManager.h"

// �̱��� �ν��Ͻ�
CTextureManager* CTextureManager::s_instance = nullptr;

CTextureManager::CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context)
    : Device(device), Context(context) {
}

CTextureManager::~CTextureManager() {
    // �ڿ� ����
    for (auto& texture : m_textureMap) {
        if (texture.second) {
            texture.second->Release();
        }
    }
}

// �̱��� �ν��Ͻ� ���
CTextureManager* CTextureManager::GetInstance(ID3D11Device* device, ID3D11DeviceContext* context) {
    if (s_instance == nullptr) {
        // ���� ȣ�� �� �ν��Ͻ� ����
        if (device == nullptr || context == nullptr) {
            throw std::runtime_error("Device and context must be provided during the first call.");
        }
        s_instance = new CTextureManager(device, context);
    }
    return s_instance;
}

// �ؽ�ó �ε�
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

     
       BindTextureToShader(DeviceContext, textureView, slotCount++);  // 0�� ���Կ� �ؽ�ó ���ε�
    }
}

// �ؽ�ó ���ε� (�������� ����)
void CTextureManager::BindTexture(const std::wstring& textureName, UINT slot) {
    auto it = m_textureMap.find(textureName);
    if (it != m_textureMap.end()) {
        // ���̴��� �ؽ�ó�� ���ε�
        Context->PSSetShaderResources(slot, 1, &it->second);
    }
}

// �̱��� ��ü ����
void CTextureManager::Cleanup() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}


ID3D11ShaderResourceView* CTextureManager::GetTextureView(const std::wstring& filePath) {
    auto it = m_textureMap.find(filePath);
    if (it != m_textureMap.end()) {
        return it->second;  // �ؽ�ó�� �̹� �ε�Ǿ� ������ �� �ؽ�ó �� ��ȯ
    }
    return nullptr;  // �ؽ�ó�� �ε���� �ʾҴٸ� nullptr ��ȯ
}

// ���̴��� �ؽ�ó ���ε� �Լ�
void CTextureManager::BindTextureToShader(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView, UINT slot) {
    context->PSSetShaderResources(slot, 1, &textureView);  // �ȼ� ���̴��� �ؽ�ó ���ε�
}

