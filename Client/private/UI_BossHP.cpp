
#include "stdafx.h"
#include "..\public\UI_BossHP.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "State.h"

#include "Nightmare.h"
#include "BigTree.h"
#include "IceMan.h"
#include "FrozenWolf.h"
#include "Goat.h"

static	_float	st_fMonsterHP;

CUI_BossHP::CUI_BossHP(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_BossHP::CUI_BossHP(const CUI_BossHP & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_BossHP::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_BossHP::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));
	iCom_Texture = (*(_int*)pArg);
	CHECK_FAILED(SetUp_Components());
	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	SizeX = 36.f;
	SizeY = 244.f;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = 1230.f;
	SetY = 505.f;


	switch (iCom_Texture)
	{
	case 0:
		m_fDepth = 0.31f; // 깊이

		switch (GM->Get_CurrentLevel())
		{
			case LEVEL_TWILIGHT:
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Nightmare"));
				fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
				fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
				break;
			case LEVEL_RAPUNZEL_BOSS:
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_BigTree"));
				fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
				fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
				break;
			case LEVEL_FROZEN:
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_IceMan"));
				fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
				fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
				break;
	
			case LEVEL_KEYGRAVE:
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_FrozenWolf"));
				fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
				fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
				break;
	
			case LEVEL_FINALBOSS:
				pBoss = m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Goat"));
				fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
				fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
				break;
	
			default:
			break;
		}
		break;

	case 1:
		m_fDepth = 0.32f; // 깊이

		break;

	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	return S_OK;
}

_int CUI_BossHP::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	switch (iCom_Texture)
	{
	case 0:
		switch (GM->Get_CurrentLevel())
		{
		case LEVEL_TWILIGHT:
			fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
			fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
			break;
		case LEVEL_RAPUNZEL_BOSS:
			fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
			fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
			break;
		case LEVEL_FROZEN:
			fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
			fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
			break;
		case LEVEL_KEYGRAVE:
			fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
			fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
			break;
		case LEVEL_FINALBOSS:
			fMonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
			fMAX_MonsterHP = static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
			break;
		}

		Fnc_MonsterHp(TimeDelta);

		if (0 >= fMonsterHP)
			m_bDead = true;
		break;
	case 1:

		Fnc_MonsterHp(TimeDelta);
		break;
	default:
		break;
	}



	return RESULT_NOPROBLEM;
}

_int CUI_BossHP::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	if (true == GM->Get_UIRender())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_BossHP::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());


	switch (iCom_Texture)
	{
	case 0:
		m_pVIBufferCom->Render(m_pShaderCom, 15);
		break;
	case 1:
		m_pVIBufferCom->Render(m_pShaderCom, 16);
		break;
	default:

		break;
	}


	return S_OK;
}

HRESULT CUI_BossHP::SetUp_Components()
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
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case 1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Boss_HPBack"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_BossHP::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	switch (iCom_Texture)
	{
	case 0:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("BOSSHP", &fSetMonsterHP_Second, sizeof(_float)))
		break;
	case 1:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("BOSSHP_BACK", &fMonsterHP_Back, sizeof(_float)))
		break;
	default:

		break;
	}


	return S_OK;
}

_int CUI_BossHP::Fnc_MonsterHp(_double TimeDelta)
{
	/*
	보스객체(pBoss)
	static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP_Ratio(); // 현재 남은 체력 비율
	static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_HP();		 // 현재 남은 체력
	static_cast<CStatus*>(pBoss->Get_Component(TEXT("Com_Status")))->Get_MaxHP();	 // 최대 체력
	*/

	switch (iCom_Texture)
	{
	case 0:
		iFirst = (_int)fMonsterHP / 100;
		iSecond = (_int)fMonsterHP % 100;

	
		/* === */
		if (0 == iSecond && false == bHp_Frist)
			iSecond = 100;
			
		if (99 >= iSecond)
			bHp_Frist = true;
		/* === */

		fSetMonsterHP_First = 1.f;
		fSetMonsterHP_Second = 1.f - (iSecond * fSetMonsterHP_First * 0.01f);
		st_fMonsterHP = fSetMonsterHP_Second;

		GM->Set_BossHpCnt(iFirst);
		break;
	case 1:
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
	default:
		break;
	}

	//if (fMonsterHP <= 0.f)
	//{
	//	GM->Math_BossHpCnt(-1);
	//	fMonsterHP = fMAX_MonsterHP;
	//}
	
	return _int();
}

CUI_BossHP * CUI_BossHP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_BossHP*	pInstance = new CUI_BossHP(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_BossHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_BossHP::Clone(void * pArg)
{
	CUI_BossHP*	pInstance = new CUI_BossHP(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_BossHP");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BossHP::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
