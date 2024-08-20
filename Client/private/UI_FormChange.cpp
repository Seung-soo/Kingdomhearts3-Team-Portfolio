#include "stdafx.h"
#include "..\public\UI_FormChange.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"

/*
1 : rageForm
2 : rageFinish
3 : HammerForm
4 : HammerFinish


iCom_Texture_Set = 1; 
if (FAILED(pGameInstance->Add_GameObject(LEVEL, TEXT("Layer_UI"), TEXT("Prototype_GameObject_FormChange"), &iCom_Texture_Set)))
{
BREAKPOINT;
return E_FAIL;
}

*/

CUI_FormChange::CUI_FormChange(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_FormChange::CUI_FormChange(const CUI_FormChange & rhs)
	: CUI_Parents(rhs)
{
}

void CUI_FormChange::Set_CounterAttackUI(_double _lifeTime, _double _timeSpeed)
{
	m_dCounterAttackUI_LifeTime = _lifeTime;
	m_dCounterAttackUI_TimeSpeed = _timeSpeed;
}

HRESULT CUI_FormChange::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_FormChange::NativeConstruct(void * pArg)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

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
	// Setting =========

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);

	_int aaa = 1;
	switch (iCom_Texture)
	{
	case 0:
		SizeX = 464.f * 0.65f;
		SizeY = 48.f * 0.65f;
		SetX = -252.f * 0.65f;
		SetY = 140.f;

		m_fDepth = 0.19f;
		break;
	case 1:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));

		SizeX = 464.f * 0.65f;
		SizeY = 48.f * 0.65f;
		SetX = -252.f * 0.65f;
		SetY = 140.f;

		m_fDepth = 0.18f;
		m_fFrame = 2.f;
		break;
	case 2:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));

		SizeX = 464.f * 0.65f;
		SizeY = 48.f * 0.65f;
		SetX = -252.f * 0.65f;
		SetY = 140.f;

		m_fDepth = 0.18f;
		m_fFrame = 3.f;
		break;
	case 3:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));

		SizeX = 464.f * 0.65f;
		SizeY = 48.f * 0.65f;
		SetX = -252.f * 0.65f;
		SetY = 140.f;

		m_fDepth = 0.18f;
		m_fFrame = 4.f;
		break;
	case 4:
		if (FAILED(pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_TBoxOpen"), &aaa)))
			return E_FAIL;
		m_pLightUI = pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_UI"));

		SizeX = 464.f * 0.65f;
		SizeY = 48.f * 0.65f;
		SetX = -252.f * 0.65f;
		SetY = 140.f;

		m_fDepth = 0.18f;
		m_fFrame = 5.f;
		break;
	case 5:

		SizeX = 200.f;
		SizeY = 50.f;
		SetX = 620.f;
		SetY = 260.f;

		m_fDepth = 0.18f;
		m_fFrame = 6.f;
		break;
	}
	
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	m_pPlayer = GM->Get_Player();

	if (nullptr == m_pPlayer)
		_breakpoint;

	return S_OK;
}

_int CUI_FormChange::Tick(_double TimeDelta)
{
	if (true == m_bDead)
	{
		m_pPlayer->Null_FormUI((CPlayer::FORM_FUNCTION)(iCom_Texture - 1));
		return RESULT_DEAD;
	}

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender())
		return RESULT_NOPROBLEM;

	switch (iCom_Texture)
	{
	case 0:
		dTime += TimeDelta;
		if (0.01 < dTime && 5.f > fSize)
		{
			fSize += 0.1f;
			fFade += 0.02f;

			if (5.f <= fSize)
			{
				fSize = 1.f;
				fFade = 1.f;
			}

			dTime = 0.0;
		}

		SizeX = 464.f * fSize * 0.5f;
		SizeY = 48.f * fSize;
		SetX = 226.f;
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		dTime += TimeDelta;
		if (false == bCase0)
		{
			if (0.01 < dTime && 454.f > fSpeedX)
			{
				fSpeedX *= 1.25f;
				dTime = 0.0;
			}

			if (454.f <= fSpeedX)
				fSpeedX = 454.f;

			SetX = -252.f + fSpeedX;
		}
		else
		{
			if (nullptr != m_pLightUI)
			{
				m_pLightUI->Set_Dead(true);
				m_pLightUI = nullptr;

				m_pPlayer->Play_FormChangeUI();
			}
			if (0.01<dTime && 600.f>fSpeedX)
			{
				fSpeedX *= 1.4f;
				dTime = 0.0;
			}

			if (600.f <= fSpeedX)
				m_bDead = true;

			SetX = 202.f - fSpeedX;
		}

		// 폼 체인지 UI
		if (1 == iCom_Texture || 3 == iCom_Texture)
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_H))
			{
				bCase0 = true;
				fSpeedX = 10.f;
			}
		}
		
		// 피니시 어택 UI
		else
		{
			if (m_pGameInstance->Get_KeyEnter(DIK_F))
			{
				bCase0 = true;
				fSpeedX = 10.f;
			}
		}
		

		m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX*0.65f, SettingY + SetY, 0.f, 1.f));

		break;

	case 5:
		
		m_dCounterAttackUI_LifeTime -= TimeDelta * m_dCounterAttackUI_TimeSpeed;

		if (0.0 >= m_dCounterAttackUI_LifeTime)
			m_bDead = true;

	default:
		break;
	}

	return _int();
}

_int CUI_FormChange::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_FormChange::Render()
{
	if (false == GM->Get_UIRender())
		return S_OK;

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	
	switch (iCom_Texture)
	{
	case 0:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("TBoxOpen", &fFade, sizeof(_float)))
			m_pVIBufferCom->Render(m_pShaderCom, 14);
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;

	default:
		break;
	}
	
	return S_OK;
}

HRESULT CUI_FormChange::SetUp_Components()
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
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button_Light"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Open_Button"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;

	}

	return S_OK;
}

HRESULT CUI_FormChange::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	switch (iCom_Texture)
	{
	case 0:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));
		break;

		
	
	}

	return S_OK;
}

CUI_FormChange * CUI_FormChange::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_FormChange*	pInstance = new CUI_FormChange(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_FormChange");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_FormChange::Clone(void * pArg)
{
	CUI_FormChange*	pInstance = new CUI_FormChange(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_FormChange");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_FormChange::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
