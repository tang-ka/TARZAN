#include "stdafx.h"
#include "CRenderer.h"

CRenderer* CRenderer::_instance = nullptr;

CRenderer::CRenderer() {
}

void CRenderer::Init(HWND hWnd) {
    _graphics = std::make_unique<CGraphics>(hWnd);

    _matrixBuffer = std::make_unique<CConstantBuffer<FMatrix>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
    _matrixBuffer->Create();
    _flagsBuffer = std::make_unique<CConstantBuffer<FPrimitiveFlags>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
    _flagsBuffer->Create();
    _textCheckBuffer = std::make_unique<CConstantBuffer<FTextCheck>>(_graphics->GetDevice(), _graphics->GetDeviceContext());
    _textCheckBuffer->Create();

    CTextureManager::SetDeviceAndContext(_graphics->GetDevice(), _graphics->GetDeviceContext());

    CTextureManager::GetInstance()->CreateSamplerState();
    CTextureManager::GetInstance()->LoadSetTexture();

    SetRasterzierState(_graphics->GetFillMode());
    _graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    SetVertexShader(L"Shader.hlsl", "VS", "vs_5_0");
    SetPixelShader(L"Shader.hlsl", "PS", "ps_5_0");
}

void CRenderer::SetVertexShader(const FWString filename, FString funcname, FString version) {
    if (!_graphics)
        assert(0);
    if (_vertexShader)
        ResetVertexShader();
    if (_inputLayout)
        _inputLayout.reset();

    _vertexShader = std::make_unique<CVertexShader>(_graphics->GetDevice());
    _vertexShader->Create(filename, funcname, version);

    _inputLayout = std::make_unique<CInputLayout>(_graphics->GetDevice());
    _inputLayout->Create(FVertexSimple::descs, _vertexShader->GetBlob());

    _graphics->GetDeviceContext()->IASetInputLayout(_inputLayout->Get());
    _graphics->GetDeviceContext()->VSSetShader(_vertexShader->Get(), nullptr, 0);
}



void CRenderer::ResetVertexShader() {
    _graphics->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
    _vertexShader.reset();
}

void CRenderer::SetPixelShader(const FWString filename, FString funcname, FString version) {
    if (!_graphics)
        assert(0);
    if (_pixelShader)
        ResetPixelShader();
    _pixelShader = std::make_unique<CPixelShader>(_graphics->GetDevice());
    _pixelShader->Create(filename, funcname, version);
    _graphics->GetDeviceContext()->PSSetShader(_pixelShader->Get(), nullptr, 0);
}

void CRenderer::ResetPixelShader() {
    _graphics->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
    _pixelShader.reset();
}

void CRenderer::SetRasterzierState(D3D11_FILL_MODE fillMode) {
    _rasterizerState = std::make_unique<CRasterzierState>(_graphics->GetDevice());
    _rasterizerState->Create(fillMode);
    _graphics->GetDeviceContext()->RSSetState(_rasterizerState->Get());
}

void CRenderer::SetTransformToConstantBuffer(FMatrix matrix,bool isBill) {
	//FMatrix view = matrix * _mainCamera->GetRelativeTransform().Inverse();
	FMatrix view = _mainCamera->View();
	FMatrix projection = _mainCamera->Projection();
   
	matrix = matrix * view;
	matrix = matrix * projection;


	_matrixBuffer->CopyData(matrix);
	ID3D11Buffer* constantBuffer = _matrixBuffer->Get();
	_graphics->GetDeviceContext()->VSSetConstantBuffers(0, 1, &constantBuffer);
}

void CRenderer::SetFlagsToConstantBuffer(FPrimitiveFlags flags) {
    _flagsBuffer->CopyData(flags);
    ID3D11Buffer* constantBuffer = _flagsBuffer->Get();
    _graphics->GetDeviceContext()->VSSetConstantBuffers(1, 1, &constantBuffer);
}


void CRenderer::SetIsTextToConstantBuffer(EObjectType type)
{
    FTextCheck checkValue;
    if (type == EObjectType::Text)
        checkValue = { 1 };
    else
        checkValue = { 0 };

    _textCheckBuffer->CopyData(checkValue);
    ID3D11Buffer* constantBuffer = _textCheckBuffer->Get();
    _graphics->GetDeviceContext()->PSSetConstantBuffers(2, 1, &constantBuffer);
}

UCameraComponent* CRenderer::GetMainCamera() const {
    return _mainCamera;
}

void CRenderer::SetMainCamera(UCameraComponent* camera) {
    _mainCamera = camera;
}


void CRenderer::SetDepthStencil(ID3D11DepthStencilState* pDSState)
{
    // Bind OMSetDepthStencilState ( Default Stencil Ref Value : 1 )
    _graphics->GetDeviceContext()->OMSetDepthStencilState(pDSState, 1);
}