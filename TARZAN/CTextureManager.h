#pragma once

#include <d3d11.h>
#include <DirectXTex.h>
#include <wrl/client.h>
#include <string>
#include <map>

using namespace DirectX;
using namespace Microsoft::WRL;

class CTextureManager {
public:
    // �̱��� �ν��Ͻ� ���
    static CTextureManager* GetInstance(ID3D11Device* device, ID3D11DeviceContext* context);

    // �ؽ�ó �ε� �Լ� (DDS ����)
    bool LoadTexture(const std::wstring& filePath);

    // �ؽ�ó�� ���̴��� ���ε� (�������� ����)
    void BindTexture(const std::wstring& textureName, UINT slot = 0);

    // ��� �ؽ�ó ����
    void Cleanup();

    // �ؽ�ó�� ���̴��� ���ε��ϴ� �Լ�
    void BindTextureToShader(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView, UINT slot);

    // �ؽ�ó�� ���̴� ���ҽ� �並 ��ȯ�ϴ� �Լ�
    ID3D11ShaderResourceView* GetTextureView(const std::wstring& filePath);

private:
    // �����ڿ� ���� ������, ���� ������ ����
    CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context);
    CTextureManager(const CTextureManager&) = delete;
    CTextureManager& operator=(const CTextureManager&) = delete;

    // �Ҹ���
    ~CTextureManager();

    ID3D11Device* Device;  // DX11 ����̽�
    ID3D11DeviceContext* Context;  // DX11 ����̽� ���ؽ�Ʈ
    std::map<std::wstring, ID3D11ShaderResourceView*> m_textureMap;  // �ؽ�ó ���� ��ο� �並 ����

    // �̱��� �ν��Ͻ�
    static CTextureManager* s_instance;
};
