#include "stdafx.h"
#include "..\public\UI_Player_Circle.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Status.h"

// static ================

static	_float	st_fHP;
static	_float	st_fMP;
static	_float	st_fFOCUS; // HP,MP,FOCUS BACK으로 보내기 위한 저장용

//=====================

CUI_Player_Circle::CUI_Player_Circle(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Player_Circle::CUI_Player_Circle(const CUI_Player_Circle & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Player_Circle::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Player_Circle::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());
	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// ==================================================================================
	// Setting 하기 전에 플레이어 정보 가져옴

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Player")));
	m_pPlayerStatus = static_cast<CStatus*>(m_pPlayer->Get_Component(TEXT("Com_Status")));

	if (nullptr == m_pPlayer || nullptr == m_pPlayerStatus)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ==================================================================================

	// Setting =========

	_float	fq = 0.5f;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:
		SizeX = 600.f* fq;
		SizeY = 400.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1100.f;
		SetY = 110.f;

		m_fDepth = 0.12f; // 깊이
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::UI_FACE:
		SizeX = 256.f* fq;
		SizeY = 272.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1186.f;
		SetY = 98.f;

		m_fDepth = 0.11f; // 깊이
		m_fFrame = 0.f;
		break;

	case Engine::CUI_Parents::UI_HP:
		SizeX = 356.f* fq;
		SizeY = 28.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1046.f;
		SetY = 64.f;

		m_fDepth = 0.13f; // 깊이
		m_fFrame = 0.f;
		//==
		fMAX_HP = m_pPlayerStatus->Get_HP();
		fHP = fMAX_HP;
		break;

	case Engine::CUI_Parents::UI_MP:
		SizeX = 254.f* fq;
		SizeY = 28.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1086.f;
		SetY = 48.f;

		m_fDepth = 0.13f; // 깊이
		m_fFrame = 0.f;
		//==
		fMAX_MP = m_pPlayerStatus->Get_MP();
		fMP = fMAX_MP;
		break;
	case Engine::CUI_Parents::UI_HP_BACK:
		SizeX = 356.f* fq;
		SizeY = 28.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1046.f;
		SetY = 64.f;

		m_fDepth = 0.14f; // 깊이
		m_fFrame = 0.f;
		//==
		fHP_Back = 0.f;
		break;

	case Engine::CUI_Parents::UI_MP_BACK:
		SizeX = 254.f* fq;
		SizeY = 28.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1086.f;
		SetY = 48.f;

		m_fDepth = 0.14f; // 깊이
		m_fFrame = 0.f;
		//==
		fMP_Back = 0.f;
		break;
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
		SizeX = 600.f* fq;
		SizeY = 400.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1100.f;
		SetY = 110.f;

		m_fDepth = 0.19f; // 깊이
		m_fFrame = 0.f;
		break;

	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		SizeX = 600.f* fq;
		SizeY = 400.f* fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1100.f;
		SetY = 110.f;

		m_fDepth = 0.18f; // 깊이
		m_fFrame = 0.f;
		break;

	default:
		break;
	}

	//================================

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	//===============

	return S_OK;
}

_int CUI_Player_Circle::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return RESULT_NOPROBLEM;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	// =============================================
	// update_data_from_player
	fMAX_HP		= m_pPlayerStatus->Get_MaxHP();
	fHP			= m_pPlayerStatus->Get_HP();
	fMAX_MP		= m_pPlayerStatus->Get_MaxMP();
	fMP			= m_pPlayerStatus->Get_MP();
	// =============================================

	int	tetest = 0;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:

		break;
	case Engine::CUI_Parents::UI_FACE:
		m_fFrame = 0.f;

		break;
	case Engine::CUI_Parents::UI_HP:
	case Engine::CUI_Parents::UI_HP_BACK:
		Fnc_Hp(TimeDelta);

		break;
	case Engine::CUI_Parents::UI_MP:
	case Engine::CUI_Parents::UI_MP_BACK:
		Fnc_MP(TimeDelta);

		break;
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
		break;
	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		break;
	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	//================================
/*
	if (pGameInstance->Get_KeyPressing(DIK_I))
	SetY += 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_K))
	SetY -= 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_J))
	SetX -= 1.f;

	if (pGameInstance->Get_KeyPressing(DIK_L))
	SetX += 1.f;

	tetest = 0;
*/
	//===============

	return RESULT_NOPROBLEM;
}

_int CUI_Player_Circle::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		break;
	case Engine::CUI_Parents::UI_HP:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("HP", &fSetHP_Second, sizeof(_float)))
		break;
	case Engine::CUI_Parents::UI_MP:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("MP", &fSetMP_Second, sizeof(_float)))
		break;
	case Engine::CUI_Parents::UI_HP_BACK:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("HPBack", &fHP_Back, sizeof(_float)))
		break;
	case Engine::CUI_Parents::UI_MP_BACK:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("MPBack", &fMP_Back, sizeof(_float)))
		break;
	case Engine::CUI_Parents::UI_FACE:
		break;
	default:
		break;
	}

	if (false == GM->Get_bCameraMove())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Player_Circle::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	case Engine::CUI_Parents::UI_HP:
		m_pVIBufferCom->Render(m_pShaderCom, 1);
		break;
	case Engine::CUI_Parents::UI_MP:
		m_pVIBufferCom->Render(m_pShaderCom, 2);
		break;
	case Engine::CUI_Parents::UI_HP_BACK:
		m_pVIBufferCom->Render(m_pShaderCom, 4);
		break;
	case Engine::CUI_Parents::UI_MP_BACK:
		m_pVIBufferCom->Render(m_pShaderCom, 5);
		break;
	case Engine::CUI_Parents::UI_FACE:
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_Player_Circle::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */ //====================
	switch (iCom_Texture)
	{ 
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_FACE:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Circle_MainSora"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_HP:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHpBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_MP:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMpBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_HP_BACK:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerHpBar_Back"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_MP_BACK:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMpBar_Back"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle_Back"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerMainCircle_Back2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

		break;
	default:
		break;
	}

	
	//======================================

	return S_OK;
}

HRESULT CUI_Player_Circle::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_CIRCLE_MAIN:
	case Engine::CUI_Parents::UI_HP:
	case Engine::CUI_Parents::UI_MP:
	case Engine::CUI_Parents::UI_HP_BACK:
	case Engine::CUI_Parents::UI_MP_BACK:
	case Engine::CUI_Parents::UI_CIRCLE_BACK:
	case Engine::CUI_Parents::UI_CIRCLE_BACK2:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		break;

	case Engine::CUI_Parents::UI_FACE:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;

	default:
		break;
	}

	return S_OK;
}

_int CUI_Player_Circle::Fnc_Hp(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_HP:
		/* TEST */
		//if (pGameInstance->Get_KeyPressing(DIK_))



		/*=======*/
		fSetHP_First = 100.f / fMAX_HP;
		fSetHP_Second = 1.f - (fHP*fSetHP_First*0.01f);
		st_fHP = fSetHP_Second;

		if (fHP > fMAX_HP)
			fHP = 0.f;

		if (fHP <= 0.f)
			fHP = fMAX_HP;

		break;

	case Engine::CUI_Parents::UI_HP_BACK:

		if (st_fHP != fHP_Back)
		{
			bHP_Change = true;
		}
		else
		{
			bHP_Change = false;
		}

		//==

		if (true == bHP_Change)
		{
			dHP_Change_Time += TimeDelta;

			if (0.05 < dHP_Change_Time)
			{
				if (st_fHP > fHP_Back)
				{
					fHP_Back += 0.05f; // 줄어드는 속도 조절
					if (st_fHP <= fHP_Back)
						fHP_Back = st_fHP;
				}
				else if (st_fHP < fHP_Back)
				{
					fHP_Back -= 0.05f; // 올라가는 솔도 조절 
					if (st_fHP >= fHP_Back)
						fHP_Back = st_fHP;
				}
				else
				{
				}
				dHP_Change_Time = 0.0;
			}
		}

		break;

	default:
		break;
	}


	return _int();
}

_int CUI_Player_Circle::Fnc_MP(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_MP:

		fSetMP_First = 100.f / m_pPlayerStatus->Get_MaxMP();
		fSetMP_Second = 1.f - (fMP*fSetMP_First*0.01f);
		st_fMP = fSetMP_Second;

		if (fMP > fMAX_MP)
			fMP = 0.f;

		if (fMP <= 0.f)
			fMP = fMAX_MP;

		break;

	case Engine::CUI_Parents::UI_MP_BACK:

		if (st_fMP != fMP_Back)
		{
			bMP_Change = true;
		}
		else
		{
			bMP_Change = false;
		}

		//==

		if (true == bMP_Change)
		{
			dMP_Change_Time += TimeDelta;

			if (0.05 < dMP_Change_Time)
			{
				if (st_fMP > fMP_Back)
				{
					fMP_Back += 0.05f;
					if (st_fMP <= fMP_Back)
						fMP_Back = st_fMP;
				}
				else if (st_fMP < fMP_Back)
				{
					fMP_Back -= 0.05f;
					if (st_fMP >= fMP_Back)
						fMP_Back = st_fMP;
				}
				else
				{

				}

				dMP_Change_Time = 0.0;
			}

		}

		break;

	default:
		break;
	}

	return _int();
}

_int CUI_Player_Circle::Fnc_FOCUS(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_FOCUS:

		fSetFOCUS_First = 100.f / fMAX_FOCUS;
		fSetFOCUS_Second = 1.f - (fFOCUS*fSetFOCUS_First*0.01f);
		st_fFOCUS = fSetFOCUS_Second;

		if (fFOCUS > fMAX_FOCUS)
			fFOCUS = 0.f;

		if (fFOCUS <= 0.f)
			fFOCUS = fMAX_FOCUS;

		break;

	case Engine::CUI_Parents::UI_FOCUS_BACK:
		
		if (st_fFOCUS != fFOCUS_Back)
		{
			bFOCUS_Change = true;
		}
		else
		{
			bFOCUS_Change = false;
		}

		//==

		if (true == bFOCUS_Change)
		{
			dFOCUS_Change_Time += TimeDelta;

			if (0.05 < dFOCUS_Change_Time)
			{
				if (st_fFOCUS > fFOCUS_Back)
				{
					fFOCUS_Back += 0.05f;
					if (st_fFOCUS <= fFOCUS_Back)
						fFOCUS_Back = st_fFOCUS;
				}
				else if (st_fFOCUS < fFOCUS_Back)
				{
					fFOCUS_Back -= 0.05f;
					if (st_fFOCUS >= fFOCUS_Back)
						fFOCUS_Back = st_fFOCUS;
				}
				else
				{

				}

				dFOCUS_Change_Time = 0.0;
			}

		}

		break;

	default:
		break;
	}


	return _int();
}

CUI_Player_Circle * CUI_Player_Circle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Player_Circle*	pInstance = new CUI_Player_Circle(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Player_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Player_Circle::Clone(void * pArg)
{
	CUI_Player_Circle*	pInstance = new CUI_Player_Circle(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Player_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Player_Circle::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
