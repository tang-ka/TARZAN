#include "stdafx.h"
#include "CRenderer.h"

CRenderer* CRenderer::_instance = nullptr;

CRenderer::CRenderer() {
}

void CRenderer::Init(HWND hWnd) {
	_graphics = new CGraphics(hWnd);
	_matrixBuffer = new CConstantBuffer<FMatrix>(_graphics->GetDevice(), _graphics->GetDeviceContext());
	_matrixBuffer->Create();
	_flagsBuffer = new CConstantBuffer<FPrimitiveFlags>(_graphics->GetDevice(), _graphics->GetDeviceContext());
	_flagsBuffer->Create();


	std::wstring texturePath1 = L"TestTexure.dds";
	textureManager = CTextureManager::GetInstance(_graphics->GetDevice(), _graphics->GetDeviceContext());
	textureManager->LoadTexture(texturePath1);


	SetVertexShader(L"Shader.hlsl", "VS", "vs_5_0");
	SetPixelShader(L"Shader.hlsl", "PS", "ps_5_0");
	SetRasterzierState(_graphics->GetFillMode());
	_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	// 텍스처를 셰이더에 바인딩
	ID3D11ShaderResourceView* textureView = textureManager->GetTextureView(texturePath1);  // 텍스처의 셰이더 리소스 뷰를 얻음
	textureManager->BindTextureToShader(_graphics->GetDeviceContext(), textureView, 0);  // 0번 슬롯에 텍스처 바인딩
}

void CRenderer::SetVertexShader(const FWString filename, FString funcname, FString version) {
	if ( _graphics == nullptr )
		assert(0);
	if ( _vertexShader != nullptr )
		ResetVertexShader();
	if ( _inputLayout != nullptr )
		SafeRelease(&_inputLayout);

	_vertexShader = new CVertexShader(_graphics->GetDevice());
	_vertexShader->Create(filename, funcname, version);

	_inputLayout = new CInputLayout(_graphics->GetDevice());
	_inputLayout->Create(FVertexSimple::descs, _vertexShader->GetBlob());

	_graphics->GetDeviceContext()->IASetInputLayout(_inputLayout->Get());
	_graphics->GetDeviceContext()->VSSetShader(_vertexShader->Get(), nullptr, 0);
}

void CRenderer::ResetVertexShader() {
	_graphics->GetDeviceContext()->VSSetShader(nullptr, nullptr, 0);
	SafeRelease(&_vertexShader);
}

void CRenderer::SetPixelShader(const FWString filename, FString funcname, FString version) {
	if ( _graphics == nullptr )
		assert(0);
	if ( _pixelShader != nullptr )
		ResetPixelShader();
	_pixelShader = new CPixelShader(_graphics->GetDevice());
	_pixelShader->Create(filename, funcname, version);
	_graphics->GetDeviceContext()->PSSetShader(_pixelShader->Get(), nullptr, 0);
}

void CRenderer::ResetPixelShader() {
	_graphics->GetDeviceContext()->PSSetShader(nullptr, nullptr, 0);
	SafeRelease(&_vertexShader);
}

void CRenderer::SetRasterzierState(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID) {
	_rasterizerState = new CRasterzierState(_graphics->GetDevice());
	_rasterizerState->Create(fillMode);
	_graphics->GetDeviceContext()->RSSetState(_rasterizerState->Get());
}

void CRenderer::SetTransformToConstantBuffer(FMatrix matrix) {
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

UCameraComponent* CRenderer::GetMainCamera() const
{
	return _mainCamera;
}

void CRenderer::SetMainCamera(UCameraComponent* camera)
{
	_mainCamera = camera;
}

void CRenderer::SetDepthStencil(ID3D11DepthStencilState* pDSState)
{
	// Bind OMSetDepthStencilState ( Default Stencil Ref Value : 1 )
	_graphics->GetDeviceContext()->OMSetDepthStencilState(pDSState, 1);
}
