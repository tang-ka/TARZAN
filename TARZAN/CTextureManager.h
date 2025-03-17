#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>
#include <string>
#include <map>
#include <EObjectType.h>

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class CTextureManager {
public:
    // ���� �ؽ�ó ��� �迭
    std::wstring TexturePaths[2] = {
        L"Texture/TestTexure.dds",
        L"Texture/FontTexture.dds"
    };

    int slotCount = 0;
    const int NumberOfTexture = sizeof(TexturePaths) / sizeof(TexturePaths[0]);

    // �̱��� �ν��Ͻ� ���
    static CTextureManager* GetInstance();
    static void SetDeviceAndContext(ID3D11Device* device, ID3D11DeviceContext* context);

    // �ؽ�ó �ε� �Լ� (DDS ����)
    void LoadSetTexture();

    // �ؽ�ó�� ���̴��� ���ε��ϴ� �Լ� (EObjectType�� ����)
    void BindTextureToShader(EObjectType type);

    // ���÷� ���� �Լ�
    void CreateSamplerState();

    // �ؽ�ó�� ���̴� ���ҽ� �並 ��ȯ�ϴ� �Լ�
    ID3D11ShaderResourceView* GetTextureView(const std::wstring& filePath);

private:
    // �����ڿ� ���� ������, ���� ������ ����
    CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context);
    CTextureManager(const CTextureManager&) = delete;
    CTextureManager& operator=(const CTextureManager&) = delete;

    // �Ҹ���
    ~CTextureManager();

    // SamplerState�� ComPtr�� ����
    ComPtr<ID3D11SamplerState> SamplerState;
    // �ܺο��� �����ϴ� Device, Context�� �������� �����Ƿ� raw pointer ���
    ID3D11Device* Device;
    ID3D11DeviceContext* Context;
    // �ؽ�ó ���� ��ο� �並 ���� (ComPtr�� ����)
    std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> m_textureMap;

    // �̱��� �ν��Ͻ�
    static CTextureManager* s_instance;
};
