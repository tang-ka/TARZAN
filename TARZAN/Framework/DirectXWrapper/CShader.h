#pragma once
class CShader {
public:
	CShader(ID3D11Device* device) : _device(device), _blob(nullptr) {};
	virtual ~CShader() {};
	void Release();

	virtual void Create(const FWString& path, const FString& name, const FString& version) abstract;
	ID3DBlob* GetBlob() { return _blob.Get(); }

protected:
	void LoadShaderFromFile(const FWString& path, const FString& name, const FString& version);

protected:
	FWString _path;
	FString _name;
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3DBlob> _blob;
};

class CVertexShader : public CShader {
	using Super = CShader;
public:
	CVertexShader(ID3D11Device* device) : Super(device), _vertexShader(nullptr) {};
	~CVertexShader() {};

	void Create(const FWString& path, const FString& name, const FString& version) override;
	ID3D11VertexShader* Get() { return _vertexShader.Get(); }

private:
	ComPtr<ID3D11VertexShader> _vertexShader;
};

class CPixelShader : public CShader {
	using Super = CShader;
public:
	CPixelShader(ID3D11Device* device) : Super(device), _pixelShader(nullptr) {};
	~CPixelShader() {};

	void Create(const FWString& path, const FString& name, const FString& version) override;
	ID3D11PixelShader* Get() { return _pixelShader.Get(); }

private:
	ComPtr<ID3D11PixelShader> _pixelShader;
};
