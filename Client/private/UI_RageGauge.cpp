#include "stdafx.h"
#include "..\public\UI_RageGauge.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Status.h"

static _bool	bRageFog = false;

CUI_RageGauge::CUI_RageGauge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_RageGauge::CUI_RageGauge(const CUI_RageGauge & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_RageGauge::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_RageGauge::NativeConstruct(void * pArg)
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

	_float	fq = 0.5f;

	switch (iCom_Texture)
	{
	case 0:
		SizeX = 500.f;
		SizeY = 100.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 885.f;
		SetY = 105.f;

		m_fDepth = 0.18f; // 깊이
		m_fFrame = 0.f;
		break;
	case 1:
		SizeX = 240.f;
		SizeY = 16.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 965.f;
		SetY = 105.f;

		m_fDepth = 0.18f; // 깊이
		m_fFrame = 0.f;
		break;
	case 2:
		SizeX = 128.f;
		SizeY = 128.f;
		SettingX = m_fX - (g_iWinCX*0.5f);
		SettingY = m_fY - (g_iWinCY*0.5f);
		SetX = 1105.f;
		SetY = 90.f;

		m_fDepth = 0.18f; // 깊이
		m_fFrame = 0.f;
		break;
	default:
		break;
	}

	fMAX_Gauge = 100.f;
	fGauge = 0.f;
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	m_pPlayer = GM->Get_Player();

	if (nullptr == m_pPlayer)
		_breakpoint;

	return S_OK;
}

_int CUI_RageGauge::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;
	if (2 == iCom_Texture)
	{
		if (true == m_pGameInstance->Get_KeyEnter(DIK_H))
			return RESULT_DEAD;
	}

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (false == GM->Get_UIRender() || false == GM->Get_Rage_Gauge())
		return 0;

	if (nullptr == m_pPlayerWeapon)
	{
		m_pPlayerWeapon = m_pPlayer->Get_Weapon();

		if (nullptr == m_pPlayerWeapon)
			_breakpoint;
	}

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	CPlayer_Weapon::WEAPON_TYPE weapon_type = m_pPlayerWeapon->WeaponType();

	fGauge = m_pPlayer->Get_RageForm_GaugeRate();
	if (0.f > fGauge)
		fGauge = 0.f;
	else if (100.f < fGauge)
		fGauge = 100.f;

	/* For.Com_Texture */
	switch (iCom_Texture)
	{
	case 0:
		m_fFrame = 0;
		break;
	case 1:
		if (m_pGameInstance->Get_KeyPressing(DIK_UP))
			fGauge += 1.f;

		if (m_pGameInstance->Get_KeyPressing(DIK_DOWN))
			fGauge -= 1.f;

		fSetGauge_First = 100.f / fMAX_Gauge;
		fSetGauge_Second = 1.f - (fGauge*fSetGauge_First*0.01f);

		if (fGauge > fMAX_Gauge)
			fGauge = fMAX_Gauge;

		if (fGauge <= 0.f)
			fGauge = 0.f;

		m_fFrame = 1;

		/* === */

		if (100 <= fGauge && false == bRageGauge)
		{
			_int Setting = 2;
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Player_RageGauge"), &Setting)))
				return E_FAIL;

			bRageFog = true;
			bRageGauge = true;
		}

		if (99 >= fGauge)
		{
			bRageFog = false;
			bRageGauge = false;
		}
		
		break;
	case 2:

		if (true == bRageFog)
		{
			m_fFrame += (_float)(36.0 * TimeDelta);
			if (15 < m_fFrame)
			{
				m_fFrame = 3.f;
			}
		}
		else
		{
			bRageFog = false;
			m_bDead = true;
		}

		break;
	default:
		break;
	}

	return RESULT_NOPROBLEM;
}

_int CUI_RageGauge::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	/* For.Com_Texture */
	switch (iCom_Texture)
	{
	case 0:

		break;
	case 1:
		CHECK_FAILED(m_pShaderCom->Set_RawValue("FOCUS", &fSetGauge_Second, sizeof(_float)))
		break;
	case 2:
		break;
	default:
		break;
	}

	if (false == GM->Get_bCameraMove())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_RageGauge::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender() || false == GM->Get_Rage_Gauge())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());
	switch (iCom_Texture)
	{
	case 0:
	case 2:
		m_pVIBufferCom->Render(m_pShaderCom, 0);
		break;
	case 1:
		m_pVIBufferCom->Render(m_pShaderCom, 3); /*focus*/
			break;
	default:
		break;
	}


	return S_OK;
}

HRESULT CUI_RageGauge::SetUp_Components()
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
	case 1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerRageGauge"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case 2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_PlayerRageGauge_Fog"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_RageGauge::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_RageGauge * CUI_RageGauge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_RageGauge*	pInstance = new CUI_RageGauge(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_RageGauge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_RageGauge::Clone(void * pArg)
{
	CUI_RageGauge*	pInstance = new CUI_RageGauge(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_RageGauge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_RageGauge::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
