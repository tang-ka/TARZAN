#include "stdafx.h"
#include "CGraphics.h"
#include <cassert>
#include <d3d11.h>
#include <dxgi.h>

//static ID3D11Debug* pDebug = nullptr;

CGraphics::CGraphics(HWND hWnd) : _hWnd(hWnd) {
    CreateDeviceAndSwapChain();
    CreateRenderTargetView();
    CreateDepthStencilBuffer();
    SetViewport(_width, _height);
    SetFillMode(D3D11_FILL_SOLID);
}

CGraphics::~CGraphics() {
    Release();
}

void CGraphics::RenderBegin() {
    _deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());
    if (_depthStencilView == nullptr)
        return;
    _deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    _deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
    _deviceContext->RSSetViewports(1, &_viewPort);
}

void CGraphics::RenderEnd() {
    _swapChain->Present(1, 0);
}

void CGraphics::Release() {
    ReleaseRenderTargetView();
    ReleaseDeviceAndSwapChain();
}

void CGraphics::ResizeBuffers(int width, int height)
{
    if (_device && _deviceContext)
    {
        // 기존 참조 모두 해제
        if (_renderTargetView)
        {
            _renderTargetView.Reset();
        }
        if (_depthStencilView)
        {
            _depthStencilView.Reset();
        }
        if (_backBuffer)
        {
            _backBuffer.Reset();
        }

        _width = width;
        _height = height;

        HRESULT hr = _swapChain->ResizeBuffers(1, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
        if (FAILED(hr))
        {
            OutputDebugString(L"ERROR: ResizeBuffers failed\n");
            return;
        }

        // 새 백 버퍼 얻기
        hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_backBuffer.GetAddressOf()));
        if (FAILED(hr))
        {
            OutputDebugString(L"ERROR: GetBuffer failed\n");
            return;
        }

        // 새 렌더 타겟 뷰 생성
        hr = _device->CreateRenderTargetView(_backBuffer.Get(), nullptr, _renderTargetView.GetAddressOf());
        if (FAILED(hr))
        {
            OutputDebugString(L"ERROR: CreateRenderTargetView failed\n");
            return;
        }

        // 새 해상도에 맞게 깊이 스텐실 버퍼 및 뷰 재생성
        CreateDepthStencilBuffer();

        // 뷰포트 업데이트
        SetViewport(width, height);
        _deviceContext->RSSetViewports(1, &_viewPort);

        // 렌더 타겟과 깊이 스텐실 뷰 설정
        _deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

        // 새 백버퍼를 클리어하여 이전 내용 제거
        _deviceContext->ClearRenderTargetView(_renderTargetView.Get(), _clearColor);
        if (_depthStencilView)
            _deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

D3D11_FILL_MODE CGraphics::GetFillMode() const {
    return _fillMode;
}

void CGraphics::SetFillMode(D3D11_FILL_MODE fillMode) {
    _fillMode = fillMode;
}

void CGraphics::ClearDepthStencilView()
{
	// Stencil Buffer to 0;
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void CGraphics::CreateDeviceAndSwapChain() {
    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferDesc.Width = _width;
    desc.BufferDesc.Height = _height;
    desc.BufferDesc.RefreshRate.Numerator = 60;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = 1;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.OutputWindow = _hWnd;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
        featureLevels,
        ARRAYSIZE(featureLevels),
        D3D11_SDK_VERSION,
        &desc,
        _swapChain.GetAddressOf(),
        _device.GetAddressOf(),
        nullptr,
        _deviceContext.GetAddressOf()
    );
    assert(SUCCEEDED(hr));

    //
    //if (SUCCEEDED(_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug))))
    //{
    //    // 디버그 인터페이스를 성공적으로 획득했습니다.
    //}
}

void CGraphics::ReleaseDeviceAndSwapChain() {

    //
    //if (pDebug)
    //{
    //    // D3D11_RLDO_DETAIL 옵션을 사용하여 상세한 객체 정보를 출력합니다.
    //    pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
    //    pDebug->Release();
    //}
    if (_deviceContext)
        _deviceContext->Flush();
    _renderTargetView.Reset();
    _depthStencilView.Reset();
    _deviceContext.Reset();
    _device.Reset();
}

void CGraphics::CreateRenderTargetView() {
    HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(_backBuffer.GetAddressOf()));
    assert(SUCCEEDED(hr));

    D3D11_RENDER_TARGET_VIEW_DESC desc = {};
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = _device->CreateRenderTargetView(_backBuffer.Get(), &desc, _renderTargetView.GetAddressOf());
    assert(SUCCEEDED(hr));
}

void CGraphics::ReleaseRenderTargetView() {
    _backBuffer.Reset();
    _renderTargetView.Reset();
}

void CGraphics::CreateDepthStencilBuffer() {
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = _width;
    descDepth.Height = _height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;

    HRESULT hr = _device->CreateTexture2D(&descDepth, nullptr, _depthStencilBuffer.GetAddressOf());
    if (FAILED(hr))
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = _device->CreateDepthStencilView(_depthStencilBuffer.Get(), &descDSV, _depthStencilView.GetAddressOf());
    if (FAILED(hr))
        return;

    // Release depthStencilBuffer if no longer needed
    _depthStencilBuffer.Reset();
}

void CGraphics::SetViewport(float width, float height) {
    _viewPort.TopLeftX = 0;
    _viewPort.TopLeftY = 0;
    _viewPort.Width = width;
    _viewPort.Height = height;
    _viewPort.MinDepth = 0.f;
    _viewPort.MaxDepth = 1.f;
}

// delete
void CGraphics::ReleaseDepthStencilView()
{
    _depthStencilView.Reset();
    _depthStencilState.Reset();
    _depthStencilBuffer.Reset();
}
