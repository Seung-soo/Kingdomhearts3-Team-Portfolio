#include "Mouse.h"
#include "PipeLine.h"

READY_SINGLETON(CMouse)

CMouse::CMouse()
{
}

HRESULT CMouse::NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, HWND hWnd)
{
	m_pDevice        = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_hWnd           = hWnd;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

_vector CMouse::MouseDir_To_WorldSpace()
{
	// 뷰포트상에서의 마우스 위치 구함
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(m_hWnd, &pt);
	_float4 tempPos;

	// 뷰포트(0,0 ~ g_iWinCX, g_iWinCY) -> 투영스페이스로(-1,1 ~ 1,-1)
	D3D11_VIEWPORT	Viewport;
	_uint			iNumViewports = 1;
	m_pDeviceContext->RSGetViewports(&iNumViewports, &Viewport);

	tempPos.x = pt.x / (Viewport.Width * 0.5f) - 1.f;
	tempPos.y = pt.y / (Viewport.Height * -0.5f) + 1.f;
	tempPos.z = 0;
	tempPos.w = 0;

	// 투영스페이스 -> 뷰스페이스로 (투영행렬의 역행렬을 곱)
	_matrix ProjMatrixInverse = CPipeLine::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInverse         = XMMatrixInverse(nullptr, ProjMatrixInverse);

	_vector MousePos = XMLoadFloat4(&tempPos);
	MousePos         = XMVector3TransformCoord(MousePos, ProjMatrixInverse);
	MousePos         = XMVectorSetW(MousePos, 1);
	_vector MouseDir = XMVectorSetW(MousePos, 0); // 뷰스페이서스 상에서의 Mouse 방향벡터는 MousePos - 원점이기 때문에 여기까지는 그냥 MousePos 값이랑 같음(w 값만 0)
	MouseDir         = XMVector3Normalize(MouseDir);

	// 뷰스페이스 -> 월드스페이스로 (뷰행렬의 역행렬을 곱)
	_matrix ViewMatrixInverse = CPipeLine::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInverse         = XMMatrixInverse(nullptr, ViewMatrixInverse);

	m_MouseWorldPos = ViewMatrixInverse.r[3];

	m_MouseWorldDir = XMVector3TransformNormal(MouseDir, ViewMatrixInverse);
	m_MouseWorldDir = XMVector3Normalize(m_MouseWorldDir);

	return m_MouseWorldDir;
}

pair<_bool, _vector> CMouse::Check_Ray_VirtualPlane_Collision(_float fPlaneY)
{
	pair<_bool, _vector> result;

	// 가상의 평면을 만듦
	_vector P1{ -1000.f, fPlaneY, +1000.f };
	_vector P2{ +1000.f, fPlaneY, +1000.f };
	_vector P3{ +1000.f, fPlaneY, -1000.f };
	_vector vPlane = XMPlaneFromPoints(P1, P2, P3);


	// MRay 의 위치는 카메라위치
	m_MouseRay.position = CPipeLine::GetInstance()->Get_CamPosition();
	// MRay 의 방향은 월드스페이스로 옮겨진 m_MouseWorldDir
	m_MouseRay.direction = MouseDir_To_WorldSpace();

	// 길이
	_float fdistance;

	// 충돌 지점을 받을 벡터
	_vector vRayPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);

	// 가상의 평면과 m_MouseRay가 충돌하면 TRUE 반환하고 fDistance에 길이를 저장
	if (TRUE == m_MouseRay.Intersects(vPlane, fdistance))
	{
		// TRUE 반환하면 레이를 쏘는 위치, 방향, 길이로 충돌된 지점을 계산
		vRayPos       = m_MouseRay.position + m_MouseRay.direction * fdistance;
		vRayPos       = XMVectorSetW(vRayPos, 1.f);
		result.first  = TRUE;
		result.second = vRayPos;
		return result;
	}

	result.first = FALSE;
	result.second = vRayPos;
	return result;
}

_bool CMouse::Ray_Box_Collision(const BoundingBox& box, _Out_ float& Dist)
{
	return m_MouseRay.Intersects(box, Dist);
}

_bool CMouse::Ray_Sphere_Collision(const BoundingSphere& sphere, float& Dist)
{
	return m_MouseRay.Intersects(sphere, Dist);
}

void CMouse::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
}