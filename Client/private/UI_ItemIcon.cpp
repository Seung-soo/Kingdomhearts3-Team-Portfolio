#include "stdafx.h"
#include "..\public\UI_ItemIcon.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "InvenManager.h"
#include "Item.h"

CUI_ItemIcon::CUI_ItemIcon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_ItemIcon::CUI_ItemIcon(const CUI_ItemIcon & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_ItemIcon::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_ItemIcon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	m_UI_Setting = (*(UI_SETTING*)pArg); /* UI_ItemGet에서 받아오는 정보들*/
	
	// UI 기본 Setting ===
	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	// Setting =========
	switch (Inven->Get_Frame())
	{
	case 0:
		m_fFrame = 0.f;
		break;
	case 1:
		m_fFrame = 1.f;
		break;
	case 2:
		m_fFrame = 2.f;
		break;
	default:
		break;
	}

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SizeX = 256.f;
	SizeY = 128.f;

	m_fDepth = 0.f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_ItemIcon::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	if (false == GM->Get_UIRender())
		return 0;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//m_fFrame = 0.f;

	_float fPulsX = *m_UI_Setting.pSetX; /* Icon Position */
	_float fPulsY = *m_UI_Setting.pSetY;

//=======
	if (800.f < fPulsY)
		m_bDead = true;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + fPulsX, SettingY + fPulsY, 0.f, 1.f));
	return RESULT_NOPROBLEM;
}

_int CUI_ItemIcon::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return RESULT_NOPROBLEM;
}

HRESULT CUI_ItemIcon::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	if (false == GM->Get_UIRender())
		return S_OK;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_ItemIcon::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemIcon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
	
	return S_OK;
}

HRESULT CUI_ItemIcon::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_uint)m_fFrame));

	return S_OK;
}

CUI_ItemIcon * CUI_ItemIcon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_ItemIcon*	pInstance = new CUI_ItemIcon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_ItemIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_ItemIcon::Clone(void * pArg)
{
	CUI_ItemIcon*	pInstance = new CUI_ItemIcon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_ItemIcon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_ItemIcon::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
