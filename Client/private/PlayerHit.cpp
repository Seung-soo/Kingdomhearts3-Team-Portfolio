#include "stdafx.h"
#include "..\public\PlayerHit.h"
#include "GameInstance.h"

CPlayerHit::CPlayerHit(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayerHit::CPlayerHit(const CPlayerHit & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayerHit::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHit::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_fSizeX = 1280.f;
	m_fSizeY = 720.f;
	m_fX = 640.f;
	m_fY = 360.f;
	m_fDepth = 50.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinCX, (_float)g_iWinCY, 0.f, 1.f)));

	m_iPass = 17;

	return S_OK;
}

_int CPlayerHit::Tick(_double dTimeDelta)
{

	if (0 > __super::Tick(dTimeDelta))
		return -1;

	m_fHitAlpha += (_float)dTimeDelta * 2.f;

	if (1.f < m_fHitAlpha)
	{
		m_fHitAlpha = 1.f;
	}

	return _int();
}

_int CPlayerHit::LateTick(_double dTimeDelta)
{
	if (0 > __super::LateTick(dTimeDelta))
		return -1;

	m_pTransformCom->Scaled(XMVectorSet(m_fSizeX, m_fSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinCX * 0.5f, -m_fY + g_iWinCY * 0.5f, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return _int();
}

HRESULT CPlayerHit::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix");
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("TBoxOpen", &m_fHitAlpha, sizeof(_float));

	if (FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture")))
	{
		MSGBOX("m_pTextureCom->SetUp_OnShader returned E_FAIL in CPlayerHit::Render");
		return E_FAIL;
	}

	m_pVIBufferCom->Render(m_pShaderCom, m_iPass);
	return S_OK;
}

HRESULT CPlayerHit::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerHit"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CPlayerHit * CPlayerHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayerHit*	pInstance = new CPlayerHit(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayerHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayerHit::Clone(void * pArg)
{
	CPlayerHit*	pInstance = new CPlayerHit(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CPlayerHit");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayerHit::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
