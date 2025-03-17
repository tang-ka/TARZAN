#include "stdafx.h"
#include "UPrimitiveComponent.h"
#include "Framework/Core/CRenderer.h"
#include "FBoundingBox.h"

UPrimitiveComponent::~UPrimitiveComponent() {}

void UPrimitiveComponent::Render() {
	CGraphics* graphics = CRenderer::Instance()->GetGraphics();
	ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
	uint32 stride = _vertexBuffer->GetStride();
	uint32 offset = _vertexBuffer->GetOffset();
	graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);

	FMatrix m = GetComponentTransform();
	CRenderer::Instance()->SetTransformToConstantBuffer(m);
	CRenderer::Instance()->SetFlagsToConstantBuffer({ renderFlags });

	if (indices.size() > 0)
		graphics->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
	else
		graphics->GetDeviceContext()->Draw(vertices.size(), 0);

    if (!isShowBoundingBox) return;

    CreateBoundingBoxBuffer();
    RenderBoundingBox();
    CGraphics* graphics = CRenderer::Instance()->GetGraphics();
  
    /* Depth Stencil DESC */
    {
        if (DepthStencilState == nullptr)
        {
            DepthStencilState = new CDepthStencilState(graphics->GetDevice());
            DepthStencilState->SetDepthFlags(TRUE, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS);
            DepthStencilState->SetStencilFlags(TRUE, 0xFF, 0xFF);
            DepthStencilState->SetFrontFaceFlags(D3D11_COMPARISON_ALWAYS, D3D11_STENCIL_OP_REPLACE, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP);
            DepthStencilState->Create();

        }

        CRenderer::Instance()->SetDepthStencil(DepthStencilState->Get());
    }

    ID3D11Buffer* vertexBuffer = _vertexBuffer->Get();
    uint32 stride = _vertexBuffer->GetStride();
    uint32 offset = _vertexBuffer->GetOffset();
    graphics->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    graphics->GetDeviceContext()->IASetIndexBuffer(_indexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);
    FMatrix m = GetComponentTransform();
    CRenderer::Instance()->SetTransformToConstantBuffer(m);
    CRenderer::Instance()->SetFlagsToConstantBuffer({ renderFlags });
    if (indices.size() > 0)
        graphics->GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
    else
        graphics->GetDeviceContext()->Draw(vertices.size(), 0);
}


void UPrimitiveComponent::GenerateRayForPicking(const FVector& pickPosition, const FMatrix& viewMatrix, FVector* pickRayOrigin, FVector* rayDirection)
{
	FMatrix xmf4x4WorldView = GetComponentTransform() * viewMatrix;
	FMatrix xmf4x4Inverse = xmf4x4WorldView.Inverse();
	FVector xmf3CameraOrigin(0.0f, 0.0f, 0.0f);
	//ì¹´ë©”??ì¢Œí‘œê³„ì˜ ?ì ??ëª¨ë¸ ì¢Œí‘œê³„ë¡œ ë³€?˜í•œ?? 
	*pickRayOrigin = xmf4x4Inverse.TransformCoord(xmf3CameraOrigin);
	//ì¹´ë©”??ì¢Œí‘œê³„ì˜ ??ë§ˆìš°??ì¢Œí‘œë¥?????˜í•˜??êµ¬í•œ ????ëª¨ë¸ ì¢Œí‘œê³„ë¡œ ë³€?˜í•œ?? 
	*rayDirection = xmf4x4Inverse.TransformCoord(pickPosition);
	//ê´‘ì„ ??ë°©í–¥ ë²¡í„°ë¥?êµ¬í•œ?? 
	*rayDirection = (*rayDirection - *pickRayOrigin).Normalized();
}

int UPrimitiveComponent::PickObjectByRayIntersection(const FVector& pickPosition, const FMatrix& viewMatrix, float* hitDistance)
{
    int inters = 0;
		FVector pickRayOrigin, pickRayDirection;
		GenerateRayForPicking(pickPosition, viewMatrix, &pickRayOrigin, &pickRayDirection);

        inters = CheckRayIntersection(pickRayOrigin,pickRayDirection, hitDistance);

		if (inters)
		{
			OutputDebugString(L"Res :: hit!!!!-----------------\n");
		}
		else
		{
			//OutputDebugString(L"Res :: NOT!!!!-----------------\n");
			//OutputDebugString(L"Ray did not hit the cube\n");
		}
		return inters;
}

bool UPrimitiveComponent::IntersectRayTriangle(const FVector& rayOrigin, const FVector& rayDirection, const FVector& v0, const FVector& v1, const FVector& v2, float& hitDistance)
{
        const float epsilon = 1e-6f;
        FVector edge1 = v1 - v0;
        const FVector edge2 = v2 - v0;
        FVector FrayDirection = rayDirection;
        FVector h = FrayDirection.Cross(edge2);
        float a = edge1.Dot(h);

        if (fabs(a) < epsilon)
            return false; // Ray?€ ?¼ê°?•ì´ ?‰í–‰??ê²½ìš°

        float f = 1.0f / a;
        FVector s = rayOrigin - v0;
        float u = f * s.Dot(h);
        if (u < 0.0f || u > 1.0f)
            return false;

        FVector q = s.Cross(edge1);
        float v = f * FrayDirection.Dot(q);
        if (v < 0.0f || (u + v) > 1.0f)
            return false;

        float t = f * edge2.Dot(q);
        if (t > epsilon) // ? íš¨??êµì°¨ (ray???œì‘???´í›„)
        {
            FVector localIntersection = rayOrigin + rayDirection * t;
            FVector worldIntersection = GetComponentTransform().TransformCoord(localIntersection);
            hitDistance = (worldIntersection).MagnitudeSquared();
            return true;
        }

        return false;
}

int UPrimitiveComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float* pfNearHitDistance)
{
    int nIntersections = 0;
    if (vertices.empty())
        return 0;

    BYTE* pbPositions = reinterpret_cast<BYTE*>(vertices.data());

    int nPrimitives = indices.empty() ? (vertices.size() / 3) : (indices.size() / 3);

    float fNearHitDistance = FLT_MAX;
    for (int i = 0; i < nPrimitives; i++)
    {
        int idx0, idx1, idx2;
        if (indices.empty())
        {
            idx0 = i * 3;
            idx1 = i * 3 + 1;
            idx2 = i * 3 + 2;
        }
        else
        {
            idx0 = indices[i * 3];
            idx1 = indices[i * 3 + 1];
            idx2 = indices[i * 3 + 2];
        }

        // ê°??¼ê°?•ì˜ ë²„í…???„ì¹˜ë¥?FVectorë¡?ë¶ˆëŸ¬?µë‹ˆ??
        uint32 stride = _vertexBuffer->GetStride();
        FVector v0 = *reinterpret_cast<FVector*>(pbPositions + idx0 * stride);
        FVector v1 = *reinterpret_cast<FVector*>(pbPositions + idx1 * stride);
        FVector v2 = *reinterpret_cast<FVector*>(pbPositions + idx2 * stride);

        float fHitDistance = 0.0f;
        if (IntersectRayTriangle(rayOrigin, rayDirection, v0, v1, v2, fHitDistance)) {
            if (fHitDistance < fNearHitDistance)
                fNearHitDistance = fHitDistance;
            nIntersections++;
        }
    }

    *pfNearHitDistance = fNearHitDistance;
    return nIntersections;
}

//void UPrimitiveComponent::UpdateBoundingBox()
//{
//    if (vertices.empty()) return;
//
//    boundingBox;
//
//    for (const auto& vertex : vertices)
//    {
//        boundingBox->ExpandToInclude({ vertex.x, vertex.y, vertex.z });
//    }
//
//    CreateBoundingBoxBuffer();
//}

void UPrimitiveComponent::CreateBoundingBoxBuffer()
{
    ID3D11Device* device = CRenderer::Instance()->GetGraphics()->GetDevice();

    // Bounding BoxÀÇ Vertex ¹× Index »ı¼º
    //boundingBox->GenerateVertices();

    boundingBox->UpdateVerticesByBP(GetComponentTransform());

    // Vertex Buffer »ı¼º
    if (!_boundingBoxVertexBuffer)
        _boundingBoxVertexBuffer = new CVertexBuffer<FVertexSimple>(device);

    _boundingBoxVertexBuffer->Create(boundingBox->GetVertices());

    // Index Buffer »ı¼º
    if (!_boundingBoxIndexBuffer)
        _boundingBoxIndexBuffer = new CIndexBuffer(device);

    _boundingBoxIndexBuffer->Create(boundingBox->GetIndices());
    //_boundingBoxIndexBuffer->)
}

void UPrimitiveComponent::RenderBoundingBox()
{
    if (!_boundingBoxVertexBuffer || !_boundingBoxIndexBuffer) return;

    ID3D11DeviceContext* context = CRenderer::Instance()->GetGraphics()->GetDeviceContext();

    // ÀÔ·Â ¹öÆÛ ¼³Á¤
    ID3D11Buffer* vertexBuffer = _boundingBoxVertexBuffer->Get();
    uint32 stride = _boundingBoxVertexBuffer->GetStride();
    uint32 offset = _boundingBoxVertexBuffer->GetOffset();
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetIndexBuffer(_boundingBoxIndexBuffer->Get(), DXGI_FORMAT_R32_UINT, 0);

    CRenderer::Instance()->SetTransformToConstantBuffer(FMatrix::Identity);
    CRenderer::Instance()->SetFlagsToConstantBuffer({ 0 });

    // ¼±(¶óÀÎ)À¸·Î ·»´õ¸µ
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    // Transform ¼³Á¤ (Bounding Box´Â ¸ğµ¨ ÁÂÇ¥°è ±×´ë·Î)
    //CRenderer::Instance()->SetTransformToConstantBuffer(GetComponentTransform());

    // Bounding Box ±×¸®±â
    context->DrawIndexed(24, 0, 0); // 12°³ÀÇ ¼±, 24°³ ÀÎµ¦½º

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
