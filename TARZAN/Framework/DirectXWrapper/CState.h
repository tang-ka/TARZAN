#pragma once
class CState {
public:
	CState(ID3D11Device* device) : _device(device) {};
	virtual ~CState() {};

protected:
	ID3D11Device* _device;
};

class CSamplerState : public CState {
	using Super = CState;
public:
	CSamplerState(ID3D11Device* device) : Super(device), _samplerState(nullptr) {};
	~CSamplerState() { 
		SafeRelease(&_samplerState);
	};
	ID3D11SamplerState* Get() { return _samplerState; }

	
	void SetAddress(D3D11_TEXTURE_ADDRESS_MODE u, D3D11_TEXTURE_ADDRESS_MODE v, D3D11_TEXTURE_ADDRESS_MODE w);
	void SetBorderColor(float r, float g, float b, float a);
	void Create();

private:
	ID3D11SamplerState* _samplerState;
	D3D11_SAMPLER_DESC _desc = {};
};

class CRasterzierState : public CState {
	using Super = CState;
public:
	CRasterzierState(ID3D11Device* device) : Super(device), _rasterizerState(nullptr) {};
	~CRasterzierState() {
		SafeRelease(&_rasterizerState);
	};
	ID3D11RasterizerState* Get() { return _rasterizerState; }

	void Create(D3D11_FILL_MODE fillMode);
private:
	ID3D11RasterizerState* _rasterizerState;
};

class CBlendState : public CState {
	using Super = CState;
public:
	CBlendState(ID3D11Device* device) : Super(device), _blendState(nullptr) {};
	~CBlendState() {
		SafeRelease(&_blendState);
	}
	ID3D11BlendState* Get() { return _blendState; }

	void Create();
	void SetSrcBlend(D3D11_BLEND srcBlend, D3D11_BLEND srcBlendAlpha);
	void SetDescBlend(D3D11_BLEND destBlend, D3D11_BLEND destBlendAlpha);
	void SetOp(D3D11_BLEND_OP op, D3D11_BLEND_OP opAlpha);
private:
	ID3D11BlendState* _blendState;
	D3D11_BLEND_DESC _desc = {};
};

class CDepthStencilState : public CState
{
	using Super = CState;

public:
	CDepthStencilState(ID3D11Device* Device) : Super(Device), DSState(nullptr) {};
	~CDepthStencilState()
	{
		SafeRelease(&DSState);
	}

	ID3D11DepthStencilState* Get() { return DSState; }

	void Create();
	void SetDepthFlags(BOOL InDepthEnable, D3D11_DEPTH_WRITE_MASK InDepthWriteMask, D3D11_COMPARISON_FUNC InDepthFunc);
	void SetStencilFlags(BOOL InStencilEnable, UINT8 InStencilReadMask, UINT8 InStencilWriteMask);
	void SetFrontFaceFlags(D3D11_COMPARISON_FUNC InStencilFunc, D3D11_STENCIL_OP InStencilPassOp, D3D11_STENCIL_OP InStencilFailOp, D3D11_STENCIL_OP InDepthFailOp);

private:
	ID3D11DepthStencilState* DSState;
	D3D11_DEPTH_STENCIL_DESC Desc = {};
};