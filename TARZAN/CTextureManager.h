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
    // 싱글턴 인스턴스 얻기
    static CTextureManager* GetInstance(ID3D11Device* device, ID3D11DeviceContext* context);

    // 텍스처 로드 함수 (DDS 포맷)
    bool LoadTexture(const std::wstring& filePath);

    // 텍스처를 셰이더에 바인딩 (동적으로 적용)
    void BindTexture(const std::wstring& textureName, UINT slot = 0);

    // 모든 텍스처 해제
    void Cleanup();

    // 텍스처를 셰이더에 바인딩하는 함수
    void BindTextureToShader(ID3D11DeviceContext* context, ID3D11ShaderResourceView* textureView, UINT slot);

    // 텍스처의 셰이더 리소스 뷰를 반환하는 함수
    ID3D11ShaderResourceView* GetTextureView(const std::wstring& filePath);

private:
    // 생성자와 복사 생성자, 대입 연산자 금지
    CTextureManager(ID3D11Device* device, ID3D11DeviceContext* context);
    CTextureManager(const CTextureManager&) = delete;
    CTextureManager& operator=(const CTextureManager&) = delete;

    // 소멸자
    ~CTextureManager();

    ID3D11Device* Device;  // DX11 디바이스
    ID3D11DeviceContext* Context;  // DX11 디바이스 컨텍스트
    std::map<std::wstring, ID3D11ShaderResourceView*> m_textureMap;  // 텍스처 파일 경로와 뷰를 매핑

    // 싱글턴 인스턴스
    static CTextureManager* s_instance;
};
