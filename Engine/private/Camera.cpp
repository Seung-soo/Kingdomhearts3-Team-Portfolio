#include "..\public\Camera.h"
#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::NativeConstruct_Prototype()
{

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

	if (FAILED(__super::NativeConstruct(&m_CameraDesc.TransformDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	_vector		vLook = XMLoadFloat3(&m_CameraDesc.vAt) - XMLoadFloat3(&m_CameraDesc.vEye);
	vLook = XMVector3Normalize(vLook);

	_vector		vRight = XMVector3Cross(XMLoadFloat3(&m_CameraDesc.vAxisY), vLook);
	vRight = XMVector3Normalize(vRight);

	_vector		vUp = XMVector3Cross(vLook, vRight);
	vUp = XMVector3Normalize(vUp);

	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMLoadFloat4(&_float4(m_CameraDesc.vEye, 1.f)));

	return S_OK;
}

_int CCamera::Tick(_double dTimeDelta)
{
	if (nullptr == m_pDevice ||
		nullptr == m_pDeviceContext)
	{
		BREAKPOINT;
		return -1;
	}


	return _int();
}

_int CCamera::LateTick(_double dTimeDelta)
{
	if (nullptr == m_pDevice ||
		nullptr == m_pDeviceContext)
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse())))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	_matrix		ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

	if (FAILED(m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, ProjMatrix)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}
	return RESULT_NOPROBLEM;
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
