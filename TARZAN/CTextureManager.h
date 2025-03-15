#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>
#include <string>
#include <map>
#include <EObjectType.h>

using namespace DirectX;
using namespace Microsoft::WRL;


class CTextureManager {
public:

    std::wstring TexturePaths[2] = {
        L"Texture/TestTexure.dds",
        L"Texture/FontTexture.dds"
    };

    int slotCount = 0;

    const int NumberOfTexture= sizeof(TexturePaths) / sizeof(TexturePaths[0]);
    // �̱��� �ν��Ͻ� ���
    static CTextureManager* GetInstance(ID3D11Device* device, ID3D11DeviceContext* context);

    // �ؽ�ó �ε� �Լ� (DDS ����)
    void LoadSetTexture(ID3D11DeviceContext* DeviceContext);

    // �ؽ�ó�� ���̴��� ���ε��ϴ� �Լ�
    void BindTextureToShader(EObjectType type);

    // ���÷� ���� �Լ� �߰�
    void CreateSamplerState(ID3D11Device* device);

    // �ؽ�ó�� ���̴� ���ҽ� �並 ��ȯ�ϴ� �Լ�
    ID3D11ShaderResourceView* GetTextureView(const std::wstring& filePath);

private:
    // �����ڿ� ���� ������, ���� ������ ����
    CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context);
    CTextureManager(const CTextureManager&) = delete;
    CTextureManager& operator=(const CTextureManager&) = delete;

    // �Ҹ���
    ~CTextureManager();

    ID3D11SamplerState* SamplerState;
    ID3D11Device* Device;  // DX11 ����̽�
    ID3D11DeviceContext* Context;  // DX11 ����̽� ���ؽ�Ʈ
    std::map<std::wstring, ID3D11ShaderResourceView*> m_textureMap;  // �ؽ�ó ���� ��ο� �並 ����

    // �̱��� �ν��Ͻ�
    static CTextureManager* s_instance;
};
