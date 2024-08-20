#include "stdafx.h"
#include "..\public\UI_Tip.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"

CUI_Tip::CUI_Tip(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Tip::CUI_Tip(const CUI_Tip & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Tip::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Tip::NativeConstruct(void * pArg)
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

	SizeX = 800.f;
	SizeY = 600.f;
	SetX = 640.f;
	SetY = 360.f;

	m_fDepth = 0.2f; // 깊이
	/*====*/

	if (0 != iCom_Texture)
	{
		m_fFrame = (_float)iCom_Texture - 1.f;
		m_fDepth = 0.1f;
		m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT3);
		m_pGameInstance->LetsPlaySound(TEXT("TipOn.wav"), CSoundMgr::SYSTEM_EFFECT3, 0.7f);
	}

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));


	GM->Get_Player()->Set_ControlMode(false);
	GM->Set_bCameraMove(true);
	GM->PlaySpeedControl(0.f);

	return S_OK;
}

_int CUI_Tip::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_KeyEnter(DIK_F))
	{
		m_pGameInstance->StopSound(CSoundMgr::SYSTEM_EFFECT3);
		m_pGameInstance->LetsPlaySound(TEXT("TipEnd.wav"), CSoundMgr::SYSTEM_EFFECT3, 0.7f);
		static_cast<CPlayer*>(m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_Player")))->Set_ControlMode(true);
		GM->Set_bCameraMove(false);
		GM->PlaySpeedControl(1.f);
		m_bDead = true;
	}

	return _int();
}

_int CUI_Tip::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CUI_Tip::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 10);

	return S_OK;
}

HRESULT CUI_Tip::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	if (0 == iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	if (0 != iCom_Texture)
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Tip_Text"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_Tip::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	if (0 == iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));
	if (0 != iCom_Texture)
		CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_Tip * CUI_Tip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Tip*	pInstance = new CUI_Tip(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Tip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Tip::Clone(void * pArg)
{
	CUI_Tip*	pInstance = new CUI_Tip(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Tip");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Tip::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
