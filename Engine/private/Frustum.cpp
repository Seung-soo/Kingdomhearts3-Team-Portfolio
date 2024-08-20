#include "..\public\Frustum.h"
#include "GameInstance.h"

READY_SINGLETON(CFrustum)

CFrustum::CFrustum()
{

}

HRESULT CFrustum::Initialize()
{
	/* 절두체에 사용될 투영스페이스상의 평면의 점들을 정의 */
	m_vPoint[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoint[1] = _float3(1.f, 1.f, 0.f);
	m_vPoint[2] = _float3(1.f, -1.f, 0.f);
	m_vPoint[3] = _float3(-1.f, -1.f, 0.f);

	m_vPoint[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoint[5] = _float3(1.f, 1.f, 1.f);
	m_vPoint[6] = _float3(1.f, -1.f, 1.f);
	m_vPoint[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Update()
{
	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_matrix		ProjMatrixInv, ViewMatrixInv;
	
	// 2차원 투영스페이스상에 정의된 절두체의 점 8개를 월드스페이스로 옮긴다
	// 투영행렬의 역행렬
	ProjMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	ProjMatrixInv = XMMatrixInverse(nullptr, ProjMatrixInv);

	// 뷰행렬의 역행렬
	ViewMatrixInv = pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	ViewMatrixInv = XMMatrixInverse(nullptr, ViewMatrixInv);

	for (_uint i = 0; i < 8; i++)
	{
		_vector		vPoint;

		// 투영스페이스상의 점들을 월드 스페이스로 변환
		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPoint[i]), ProjMatrixInv);
		vPoint = XMVector3TransformCoord(vPoint, ViewMatrixInv);

		// 월드 점 배열에 저장
		XMStoreFloat3(&m_vPointInWorld[i], vPoint);
	}

	//	현재의 절두체 평면들을 만들어준다.
	Compute_Plane(m_vPointInWorld, m_PlaneInWorld);

	 
}

HRESULT CFrustum::Transform_ToLocalSpace(CTransform * pTransform)
{
	for (_uint i = 0; i < 8; ++i)
	{
		_vector			vPoint;

		vPoint = XMVector3TransformCoord(XMLoadFloat3(&m_vPointInWorld[i]), pTransform->Get_WorldMatrixInverse());

		XMStoreFloat3(&m_vPointInLocal[i], vPoint);
	}

	Compute_Plane(m_vPointInLocal, m_PlaneInLocal);

	return S_OK;
}

_bool CFrustum::isIn_WorldSpace(_fvector vPoint, _float fRange)
{
	for (_uint i = 0; i < 6; i++)
	{
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlaneInWorld[i]), vPoint)) > fRange)
			return false;
	}

	return true;
}

_bool CFrustum::isIn_LocalSpace(_fvector vPoint, _float fRange)
{
	for (_uint i = 0; i < 6; ++i)
	{
		// 평면의 방정식의 해가 반지름보다 크면	절두체 외부에 있다고 판단
		if (XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_PlaneInLocal[i]), vPoint)) > fRange)
			return false;
	}
	return true;
}

void CFrustum::Compute_Plane(const _float3 * pPoints, _float4 * pPlanes)
{
	/* +x (오른쪽) */
	XMStoreFloat4(&pPlanes[0], XMPlaneFromPoints(XMLoadFloat3(&pPoints[1])
		, XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[6])));

	/* -x (왼쪽) */
	XMStoreFloat4(&pPlanes[1], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4])
		, XMLoadFloat3(&pPoints[0]), XMLoadFloat3(&pPoints[3])));

	/* +y (위) */
	XMStoreFloat4(&pPlanes[2], XMPlaneFromPoints(XMLoadFloat3(&pPoints[4])
		, XMLoadFloat3(&pPoints[5]), XMLoadFloat3(&pPoints[1])));

	/* -y (아래) */
	XMStoreFloat4(&pPlanes[3], XMPlaneFromPoints(XMLoadFloat3(&pPoints[6])
		, XMLoadFloat3(&pPoints[7]), XMLoadFloat3(&pPoints[3])));

	/* +z (Far) */
	XMStoreFloat4(&pPlanes[4], XMPlaneFromPoints(XMLoadFloat3(&pPoints[5])
		, XMLoadFloat3(&pPoints[4]), XMLoadFloat3(&pPoints[7])));

	/* -z (Near) */
	XMStoreFloat4(&pPlanes[5], XMPlaneFromPoints(XMLoadFloat3(&pPoints[0])
		, XMLoadFloat3(&pPoints[1]), XMLoadFloat3(&pPoints[2])));
}

void CFrustum::Free()
{
}
