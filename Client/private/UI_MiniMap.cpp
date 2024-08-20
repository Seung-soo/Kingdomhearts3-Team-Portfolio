#include "stdafx.h"
#include "..\public\UI_MiniMap.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Monster.h"
#include "Player.h"

CUI_MiniMap::CUI_MiniMap(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_MiniMap::CUI_MiniMap(const CUI_MiniMap & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_MiniMap::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_MiniMap::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;


	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	switch (iCom_Texture)
	{
	case 0:
		m_fX = 0.f;
		m_fY = 0.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SizeX = 16.f;
		SizeY = 16.f;
		SetX = 640.f;
		SetY = 360.f;
		m_fDepth = 0.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
		break;
	case 1:
		SizeX = 150.f * 4.f;
		SizeY = 150.f * 4.f;

		SetX = 0.f;
		SetY = 0.f;

		m_fX = 0.f;
		m_fY = 0.f;
		m_fDepth = 0.01f;
		break;
	case 2:
		SizeX = 200.f * 4.f;
		SizeY = 200.f * 4.f;

		SetX = 0.f;
		SetY = 0.f;

		m_fX = 0.f;
		m_fY = 0.f;
		m_fDepth = 0.01f;
		break;

	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	
	ZeroMemory(&m_vTargetPosition, sizeof(_float4));

	return S_OK;
}

_int CUI_MiniMap::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	
	switch (iCom_Texture)
	{
	case 0:
		//if (m_pGameInstance->Get_KeyPressing(DIK_UP))
		//	SetY += 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
		//	SetY -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
		//	SetX -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
		//	SetX += 1.f;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));
		break;
	case 1:
		if (false == bStart)
		{
			LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
			m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));
			Player_Origin_Pos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			bStart = true;
		}
		break;
	case 2:
		if (false == bStart)
		{
			LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
			m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));
			Player_Origin_Pos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

			bStart = true;
		}
		break;
	}
	
	return RESULT_NOPROBLEM;
}

_int CUI_MiniMap::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	switch (iCom_Texture)
	{
	case 0:

		break;
	case 1:
		/* 플레이어 포지션 */
		XMStoreFloat4(&fPlayerOriPos, Player_Origin_Pos);
		
		vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPlayerPos, vPlayerPos);

		Player_MovePosX = fPlayerOriPos.x + fPlayerPos.x;
		Player_MovePosZ = fPlayerOriPos.z + fPlayerPos.z;


		//if (m_pGameInstance->Get_KeyPressing(DIK_UP))
		//	SetY += 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
		//	SetY -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
		//	SetX -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
		//	SetX += 1.f;

		final_position = XMVectorSet((fPlayerOriPos.z + 30.f + (Player_MovePosZ * 4.f) + 190.f),
											-(fPlayerOriPos.x + 50.f + (Player_MovePosX * 4.f) - 54.f),
											0.f, 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position);
		break;
	case 2:
		/* 플레이어 포지션 */
		XMStoreFloat4(&fPlayerOriPos, Player_Origin_Pos);

		vPlayerPos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		XMStoreFloat4(&fPlayerPos, vPlayerPos);

		Player_MovePosX = fPlayerOriPos.x + fPlayerPos.x;
		Player_MovePosZ = fPlayerOriPos.z + fPlayerPos.z;



		//if (m_pGameInstance->Get_KeyPressing(DIK_UP))
		//	SetY += 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
		//	SetY -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_LEFT))
		//	SetX -= 1.f;
		//if (m_pGameInstance->Get_KeyPressing(DIK_RIGHT))
		//	SetX += 1.f;

		final_position = XMVectorSet(-(fPlayerOriPos.z + (Player_MovePosX * 4.f) + 196.f),
			-(fPlayerOriPos.x + (Player_MovePosZ * 4.f) + -241.f),
			0.f, 1.f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position);
		break;
	}

	/* === */
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_MiniMap::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (true == GM->Get_Render_MiniMap())
	{
		CHECK_FAILED(SetUp_ConstantTable());
		m_pVIBufferCom->Render(m_pShaderCom, 0);
	}
	

	return S_OK;
}

HRESULT CUI_MiniMap::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	switch (iCom_Texture)
	{
	case 0:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_Player_Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom))
		break;
	case 1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_tt"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom))
		break;
	case 2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_MiniMap_ra"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom))
			break;
	}

	return S_OK;
}

HRESULT CUI_MiniMap::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"))
	return S_OK;
}

CUI_MiniMap * CUI_MiniMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_MiniMap*	pInstance = new CUI_MiniMap(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_MiniMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MiniMap::Clone(void * pArg)
{
	CUI_MiniMap*	pInstance = new CUI_MiniMap(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_MiniMap");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MiniMap::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
