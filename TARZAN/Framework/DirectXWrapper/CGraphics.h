#pragma once

class CGraphics {
public:
	CGraphics(HWND hWnd);
	~CGraphics();

	void RenderBegin();
	void RenderEnd();
	void Release();
	D3D11_VIEWPORT GetViewport() { return _viewPort; }
	ID3D11Device* GetDevice() { return _device.Get(); }
	ID3D11DeviceContext* GetDeviceContext() { return _deviceContext.Get(); }
	void ResizeBuffers(int width, int height);

	D3D11_FILL_MODE GetFillMode() const;
	void SetFillMode(D3D11_FILL_MODE fillMode);

private:
	HWND _hWnd;
	uint32 _width = SCR_WIDTH;
	uint32 _height = SCR_HEIGHT;

	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _deviceContext;
	ComPtr<IDXGISwapChain> _swapChain;

	ComPtr<ID3D11Texture2D> _backBuffer;
	ComPtr<ID3D11RenderTargetView> _renderTargetView;

	ComPtr<ID3D11Texture2D> depthStencilBuffer;
	ComPtr<ID3D11DepthStencilView> depthStencilView;
	ComPtr<ID3D11DepthStencilState> depthStencilState;


	D3D11_VIEWPORT _viewPort = {};
	D3D11_FILL_MODE _fillMode;

	float _clearColor[4] = { 0.02f, 0.02f, 0.02f, 1.f };

private:
	void CreateDeviceAndSwapChain();
	void ReleaseDeviceAndSwapChain();
	void CreateRenderTargetView();
	void ReleaseRenderTargetView();
	void CreateDepthStencilBuffer();
	void SetViewport(float, float);
};

