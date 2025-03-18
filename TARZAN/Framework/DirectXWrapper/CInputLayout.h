#pragma once

#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class CInputLayout {
public:
    CInputLayout(ID3D11Device* device) : _device(device) {}
    ~CInputLayout() = default;  // ComPtr이 자동으로 Release() 해줌
    void Release() {}           // 더 이상 수동 Release가 필요없음
    ID3D11InputLayout* Get() { return _inputLayout.Get(); }

    void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ID3DBlob* vsblob);

private:
    ComPtr<ID3D11Device> _device;
    ComPtr<ID3D11InputLayout> _inputLayout;
};

struct FVertexSimple {
    float x, y, z;
    float r, g, b, a;
    float u, v;
    static TArray<D3D11_INPUT_ELEMENT_DESC> descs;

    bool operator==(const FVertexSimple& other) const {
        return x == other.x && y == other.y && z == other.z &&
            r == other.r && g == other.g && b == other.b && a == other.a;
    }
};

namespace std {
    template <>
    struct hash<FVertexSimple> {
        size_t operator()(const FVertexSimple& v) const {
            size_t h1 = hash<float>()(v.x) ^ (hash<float>()(v.y) << 1);
            size_t h2 = hash<float>()(v.z) ^ (hash<float>()(v.r) << 1);
            size_t h3 = hash<float>()(v.g) ^ (hash<float>()(v.b) << 1) ^ (hash<float>()(v.a) << 2);
            return h1 ^ h2 ^ h3;
        }
    };
}
