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


	textureManager = CTextureManager::GetInstance(_graphics->GetDevice(), _graphics->GetDeviceContext());
	textureManager->CreateSamplerState(_graphics->GetDevice());
	textureManager->LoadSetTexture(_graphics->GetDeviceContext());

	SetRasterzierState(_graphics->GetFillMode());
	_graphics->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	SetVertexShader(L"Shader.hlsl", "VS", "vs_5_0");
	SetPixelShader(L"Shader.hlsl", "PS", "ps_5_0");
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

void CRenderer::SetTransformToConstantBuffer(FMatrix matrix,bool isBill) {
	//FMatrix view = matrix * _mainCamera->GetRelativeTransform().Inverse();
	FMatrix view = _mainCamera->View();

	FMatrix viewNoRotation = FMatrix::Identity;
	if (isBill)
	{
		// View 행렬에서 위치 정보만 추출하여 viewNoRotation에 적용
		viewNoRotation.m[0][3] = -view.m[0][3];  // X 위치
		viewNoRotation.m[1][3] = -view.m[1][3];  // Y 위치
		viewNoRotation.m[2][3] = -view.m[2][3];  // Z 위치
	}

	FMatrix RealView = isBill ? viewNoRotation : view;

	FMatrix projection = _mainCamera->Projection();
	matrix = matrix * RealView;
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

CTextureManager* CRenderer::GetTextureManager()
{
	return textureManager;
}

