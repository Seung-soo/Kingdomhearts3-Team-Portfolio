#include "stdafx.h"
#include "..\public\UI_MonsterHP.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Status.h"

// static ================

static	_int	st_Render_HP = 0; // HPTEXT Position -> Large:0 Middle:1 Small:2
static	_float	st_fMonsterHP;

_float	fq = 1.4f;

//=====================

CUI_MonsterHP::CUI_MonsterHP(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_MonsterHP::CUI_MonsterHP(const CUI_MonsterHP & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_MonsterHP::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_MonsterHP::NativeConstruct(void * pArg)
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

	// ==================================================================================

	// Setting =========

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MONSTER_TEXT:
		SizeX = 26.f * fq;
		SizeY = 19.f * fq;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);

		if (SIZE_LARGE == st_Render_HP)
		{
			SetX = 398.f;
			SetY = 650.f;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SetX = 505.f;
			SetY = 650.f;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SetX = 546.f;
			SetY = 650.f;
		}

		m_fDepth = 0.1f; // 깊이
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::MONSTER_HP:

		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 313.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 163.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 103.f * fq;
			SizeY = 14.f * fq;
		}

		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 650.f;

		m_fDepth = 0.1f; // 깊이
		m_fFrame = 0.f;

		//fMonsterHP = 100.f;
		//fMAX_MonsterHP = 100.f;

		break;
	case Engine::CUI_Parents::MONSTER_HP_BACK:

		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 313.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 163.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 103.f * fq;
			SizeY = 14.f * fq;
		}

		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 650.f;

		m_fDepth = 0.2f; // 깊이
		m_fFrame = 0.f;

		//fMonsterHP = 100.f;
		break;
	case Engine::CUI_Parents::MONSTER_HP_BAR:

		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 315.f * fq;
			SizeY = 16.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 165.f * fq;
			SizeY = 16.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 105.f * fq;
			SizeY = 16.f * fq;
		}
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 640.f;
		SetY = 650.f;

		m_fDepth = 0.0f; // 깊이
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

_int CUI_MonsterHP::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return RESULT_NOPROBLEM;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	target_object = m_pPlayer->Get_Target();
	if (nullptr != target_object && false == target_object->Get_ReadyToDie())
	{
		CStatus* target_status_com = static_cast<CStatus*>(target_object->Get_Component(TEXT("Com_Status")));

		if (nullptr != target_status_com)
		{
			fMAX_MonsterHP = target_status_com->Get_MaxHP();
			fMonsterHP = target_status_com->Get_HP();
		}
	}

	// ====================

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MONSTER_TEXT:

		if (SIZE_LARGE == st_Render_HP)
		{
			SetX = 398.f;
			SetY = 650.f;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SetX = 505.f;
			SetY = 650.f;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SetX = 546.f;
			SetY = 650.f;
		}

		break;
	case Engine::CUI_Parents::MONSTER_HP:

		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 313.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 163.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 103.f * fq;
			SizeY = 14.f * fq;
		}

		//if (0 >= fMonsterHP)
		//	fMonsterHP = fMAX_MonsterHP;

		//if (fMAX_MonsterHP <= fMonsterHP)
		//	fMonsterHP = fMAX_MonsterHP;

		Fnc_MonsterHp(TimeDelta);

		break;
	case Engine::CUI_Parents::MONSTER_HP_BACK:

		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 313.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 163.f * fq;
			SizeY = 14.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 103.f * fq;
			SizeY = 14.f * fq;
		}

		Fnc_MonsterHp(TimeDelta);

		break;
	case Engine::CUI_Parents::MONSTER_HP_BAR:
		
		if (SIZE_LARGE == st_Render_HP)
		{
			SizeX = 315.f * fq;
			SizeY = 16.f * fq;
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			SizeX = 165.f * fq;
			SizeY = 16.f * fq;
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			SizeX = 105.f * fq;
			SizeY = 16.f * fq;
		}

		break;
	default:

		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	return _int();
}

_int CUI_MonsterHP::LateTick(_double TimeDelta)
{

	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);



	return _int();
}

HRESULT CUI_MonsterHP::Render()
{
	if (false == GM->Get_UIRender())
		return S_OK;

	
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	if (nullptr != target_object)
	{
		switch (iCom_Texture)
		{
		case Engine::CUI_Parents::MONSTER_TEXT:
			m_pVIBufferCom->Render(m_pShaderCom, 0);
			break;
		case Engine::CUI_Parents::MONSTER_HP:
			m_pVIBufferCom->Render(m_pShaderCom, 7);
			break;
		case Engine::CUI_Parents::MONSTER_HP_BACK:
			m_pVIBufferCom->Render(m_pShaderCom, 8);
			break;
		case Engine::CUI_Parents::MONSTER_HP_BAR:
			m_pVIBufferCom->Render(m_pShaderCom, 0);
			break;
		default:
			break;
		}
	}
	return S_OK;
}

HRESULT CUI_MonsterHP::SetUp_Components()
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
	case Engine::CUI_Parents::MONSTER_TEXT:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_TEXT"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_TEXT]));
		break;
	case Engine::CUI_Parents::MONSTER_HP:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_LARGE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_SMALL]));
		break;
	case Engine::CUI_Parents::MONSTER_HP_BACK:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large_Back"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_LARGE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle_Back"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small_Back"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_SMALL]));
		break;
	case Engine::CUI_Parents::MONSTER_HP_BAR:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Large_Bar"), TEXT("Com_Texture0"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_LARGE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Middle_Bar"), TEXT("Com_Texture1"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]));
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Monster_HP_Small_Bar"), TEXT("Com_Texture2"), (CComponent**)&m_pTextureCom[UI_MONSTER_SIZE_SMALL]));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_MonsterHP::SetUp_ConstantTable()
{

	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MONSTER_TEXT:
		m_pTextureCom[UI_MONSTER_SIZE_TEXT]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		break;
	case Engine::CUI_Parents::MONSTER_HP:
		if (SIZE_LARGE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_LARGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_SMALL]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		break;
	case Engine::CUI_Parents::MONSTER_HP_BACK:
		if (SIZE_LARGE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_LARGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_SMALL]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		break;
	case Engine::CUI_Parents::MONSTER_HP_BAR:
		if (SIZE_LARGE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_LARGE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_MIDDLE == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_MIDDLE]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		else if (SIZE_SMALL == st_Render_HP)
		{
			m_pTextureCom[UI_MONSTER_SIZE_SMALL]->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture");
		}
		break;

	default:
		break;
	}

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MONSTER_TEXT:
		break;
	case Engine::CUI_Parents::MONSTER_HP:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("MONSTERHP", &fSetMonsterHP_Second, sizeof(_float)))
			break;
	case Engine::CUI_Parents::MONSTER_HP_BACK:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("MONSTERHP_BACK", &fMonsterHP_Back, sizeof(_float)))
			break;
	case Engine::CUI_Parents::MONSTER_HP_BAR:
		break;
	default:
		break;
	}
	
	return S_OK;
}

_int CUI_MonsterHP::Fnc_MonsterHp(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::MONSTER_HP:

		fSetMonsterHP_First = 100.f / fMAX_MonsterHP;
		fSetMonsterHP_Second = 1.f - (fMonsterHP*fSetMonsterHP_First*0.01f);
		st_fMonsterHP = fSetMonsterHP_Second;

		if (fMonsterHP > fMAX_MonsterHP)
			fMonsterHP = 0.f;

		if (fMonsterHP <= 0.f)
			fMonsterHP = fMAX_MonsterHP;

		break;

	case Engine::CUI_Parents::MONSTER_HP_BACK:

		if (st_fMonsterHP != fMonsterHP_Back)
		{
			bMonsterHP_Change = true;
		}
		else
		{
			bMonsterHP_Change = false;
		}

		//==

		if (true == bMonsterHP_Change)
		{
			if (st_fMonsterHP > fMonsterHP_Back)
			{
				fMonsterHP_Back += 0.01f;
				if (st_fMonsterHP <= fMonsterHP_Back)
					fMonsterHP_Back = st_fMonsterHP;
			}
			else if (st_fMonsterHP < fMonsterHP_Back)
			{
				fMonsterHP_Back -= 0.01f;
				if (st_fMonsterHP >= fMonsterHP_Back)
					fMonsterHP_Back = st_fMonsterHP;
			}
		}
		break;
	}

	return _int();
}

CUI_MonsterHP * CUI_MonsterHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_MonsterHP*	pInstance = new CUI_MonsterHP(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Player_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_MonsterHP::Clone(void * pArg)
{
	CUI_MonsterHP*	pInstance = new CUI_MonsterHP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Player_Circle");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_MonsterHP::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);

	for (_uint i = 0; i < UI_MONSTER_SIZE_END; ++i)
		Safe_Release(m_pTextureCom[i]);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
