#include "stdafx.h"
#include "..\public\Mini_Player.h"
#include "GameInstance.h"
#include "MiniGame_Manager.h"

/*
Prototype_GameObject_Minigame_Player
Prototype_GameObject_Minigame_Bullet
Prototype_GameObject_Minigame_Boss
Prototype_GameObject_Minigame_Damage
*/

CMini_Player::CMini_Player(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMini_Player::CMini_Player(const CMini_Player & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMini_Player::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMini_Player::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());

	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	SizeX = 32.f;
	SizeY = 32.f;
	SetX = 640.f;
	SetY = 360.f;

	m_fFrame = 0.f;

	m_fDepth = 0.5f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
	
	return S_OK;
}

_int CMini_Player::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	/* 이동 */
	dMoveSpeed = 50.0;
	if (m_pGameInstance->Get_KeyPressing(DIK_UP))
		m_pTransformCom->Go_Up(TimeDelta*dMoveSpeed);
	if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
		m_pTransformCom->Go_Up(-TimeDelta*dMoveSpeed);
	if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
		m_pTransformCom->Go_Left(TimeDelta*dMoveSpeed);
	if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
		m_pTransformCom->Go_Left(-TimeDelta*dMoveSpeed);

	/* 실시간 플레이어 포지션 받기 */
	_vector	vPlayerPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float4 fPlayerPosition;
	XMStoreFloat4(&fPlayerPosition, vPlayerPosition);
	MINIGAME->Set_PlayerPosition(fPlayerPosition);
	
	

	/* 공격 */
	if (m_pGameInstance->Get_KeyEnter(DIK_Z)/* && true == MINIGAME->Get_PlayerRender()*/)
	{
		if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_ENDING, TEXT("Layer_MINI_BULLET"), TEXT("Prototype_GameObject_Minigame_Bullet"))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	if (m_pGameInstance->Get_KeyEnter(DIK_2))
	{
		MINIGAME->Set_PlayerRender(true);
	}


	return _int();
}

_int CMini_Player::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	if (false == MINIGAME->Get_PlayerRender())
		return _int();
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CMini_Player::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMini_Player::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniGame_Player"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMini_Player::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}


CMini_Player * CMini_Player::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_Player*	pInstance = new CMini_Player(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_Player::Clone(void * pArg)
{
	CMini_Player*	pInstance = new CMini_Player(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_Player");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_Player::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
