#include "stdafx.h"
#include "..\Public\Camera_Dynamic.h"
#include "GameInstance.h"

CCamera_Dynamic::CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CCamera_Dynamic::CCamera_Dynamic(const CCamera_Dynamic & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Dynamic::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Dynamic::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	m_pTransformCom->Set_Speed(10.f);


	return S_OK;
}

_int CCamera_Dynamic::Tick(_double TimeDelta)
{
	KeyCheck(TimeDelta);

	return __super::Tick(TimeDelta);
}

_int CCamera_Dynamic::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return -1;

	return _int();
}

HRESULT CCamera_Dynamic::Render()
{
	return S_OK;
}

void CCamera_Dynamic::KeyCheck(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Mouse_Pressing(CInput_Device::MBS_RBUTTON))
	{
		if (pGameInstance->Get_KeyPressing(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
		}

		if (pGameInstance->Get_KeyPressing(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
		}

		if (pGameInstance->Get_KeyPressing(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
		}

		if (pGameInstance->Get_KeyPressing(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
		}

		if (pGameInstance->Get_KeyPressing(DIK_Q))
		{
			m_pTransformCom->Go_Down(TimeDelta);
		}

		if (pGameInstance->Get_KeyPressing(DIK_E))
		{
			m_pTransformCom->Go_Up(TimeDelta);
		}

		_long	MouseMove  = 0;
		_float	fSensitive = 0.0015f;

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::MMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fSensitive);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMoveState(CInput_Device::MMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fSensitive);
		}
	}

	 
}

CCamera_Dynamic * CCamera_Dynamic::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Dynamic::Clone(void * pArg)
{
	CCamera_Dynamic*	pInstance = new CCamera_Dynamic(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Creating CCamera_Dynamic");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Dynamic::Free()
{
	__super::Free();



}
