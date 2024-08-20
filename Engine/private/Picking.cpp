#include "..\public\Picking.h"
#include "PipeLine.h"

READY_SINGLETON(CPicking)

CPicking::CPicking()
	: m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CPicking::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd)
{
	m_hWnd = hWnd;
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(pDeviceContext);
	return S_OK;
}

_vector CPicking::Get_RayDirLocal()
{
	return m_vRayDirInLocal;
}

_vector CPicking::Get_RayPosLocal()
{
	return m_vRayPosInLocal;
}

_float4 CPicking::Get_RayDirLocal_Float4()
{
	_float4	vRayDirLocal;
	XMStoreFloat4(&vRayDirLocal, m_vRayDirInLocal);

	return vRayDirLocal;
}

_float4 CPicking::Get_RayPosLocal_Float4()
{
	_float4 vRayPosLocal;
	XMStoreFloat4(&vRayPosLocal, m_vRayPosInLocal);

	return vRayPosLocal;
}

HRESULT CPicking::Transform_ToWorldSpace()
{
	POINT		ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(m_hWnd, &ptMouse);

	RECT rcWindowSize = {};
	GetClientRect(m_hWnd, &rcWindowSize);

	_uint iViewNum = 1;

	m_pDeviceContext->RSGetViewports(&iViewNum, nullptr);
	D3D11_VIEWPORT*		ViewPortDesc = new D3D11_VIEWPORT[iViewNum];
	m_pDeviceContext->RSGetViewports(&iViewNum, ViewPortDesc);

	_vector		vMousePos = XMVectorSet(ptMouse.x / (/*ViewPortDesc[0].Width*/(rcWindowSize.right - rcWindowSize.left) * 0.5f) - 1,
										ptMouse.y / -(/*ViewPortDesc[0].Height*/(rcWindowSize.bottom - rcWindowSize.top) * 0.5f) + 1,
										0.f, 1.f);

	Safe_Delete_Array(ViewPortDesc);

	_matrix	ProjMatrixInverse;
	ProjMatrixInverse = m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInverse = XMMatrixInverse(nullptr, ProjMatrixInverse);
	vMousePos = XMVector3TransformCoord(vMousePos, ProjMatrixInverse);


	m_vRayDir = XMVector4Normalize(vMousePos - XMVectorSet(0.f, 0.f, 0.f, 1.f));
	m_vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	_matrix	ViewMatrixInverse;
	ViewMatrixInverse = m_pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInverse = XMMatrixInverse(nullptr, ViewMatrixInverse);
	m_vRayDir = XMVector3TransformNormal(m_vRayDir, ViewMatrixInverse);
	m_vRayPos = XMVector3TransformCoord(m_vRayPos, ViewMatrixInverse);
	
	//BoundingBox

	return S_OK;
}

HRESULT CPicking::Transform_ToLocalSpace(_matrix WorldMatrixInverse)
{
	m_vRayDirInLocal = XMVector3TransformNormal(m_vRayDir, WorldMatrixInverse);
	m_vRayDirInLocal = XMVector4Normalize(m_vRayDirInLocal);
	m_vRayPosInLocal = XMVector3TransformCoord(m_vRayPos, WorldMatrixInverse);

	return S_OK;
}

_bool CPicking::isPick(_fvector * pLocalPoints, _fvector * pOut)
{
	_float fDist = 0.f;

	//BoundingBox
	if (TriangleTests::Intersects((_fvector)m_vRayPosInLocal, (_fvector)m_vRayDirInLocal, (_fvector)pLocalPoints[0], (GXMVECTOR)pLocalPoints[1], (HXMVECTOR)pLocalPoints[2], fDist))
	{
		*(_vector*)pOut = m_vRayPosInLocal + m_vRayDirInLocal * fDist;
		return true;
	}
	else
		return false;
}

_bool CPicking::IsNaviPick(_fvector * pLocalPoints)
{
	_float fDist = 0.f;

	//BoundingBox
	if (TriangleTests::Intersects((_fvector)m_vRayPos, (_fvector)m_vRayDir, (_fvector)pLocalPoints[0], (GXMVECTOR)pLocalPoints[1], (HXMVECTOR)pLocalPoints[2], fDist))
	{
		return true;
	}
	else
		return false;
}

_bool CPicking::isPick_AABB(BoundingBox tAABB)
{
	_float fDist = 0.f;

	if (tAABB.Intersects((_fvector)m_vRayPos, (_fvector)m_vRayDir, fDist))
	{
		return true;
	}
	else
		return false;

}

void CPicking::Free()
{
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}
