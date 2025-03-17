#pragma once
class CInputLayout {
public:
	CInputLayout(ID3D11Device* device) : _device(device), _inputLayout(nullptr) {};
	~CInputLayout() { 
		SafeRelease(&_inputLayout); 
	};
	void Release() {};
	ID3D11InputLayout* Get() { return _inputLayout; }

	void Create(const std::vector<D3D11_INPUT_ELEMENT_DESC>& descs, ID3DBlob* vsblob);

private:
	ID3D11Device* _device;
	ID3D11InputLayout* _inputLayout;
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

