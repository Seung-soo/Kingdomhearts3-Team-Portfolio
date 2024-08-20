#include "stdafx.h"
#include "..\public\Menu_Burble.h"
#include "GameInstance.h"
#include "GameManager.h"

CMenu_Burble::CMenu_Burble(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CMenu_Burble::CMenu_Burble(const CMenu_Burble & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CMenu_Burble::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CMenu_Burble::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	//iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());
	// UI 기본 Setting ===

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));
	// Setting =========

	/* ============= */
	_float fPositionA = (_float)(rand() % 2 * 1000);
	_float fPositionB = (_float)(rand() % 9 * 100);
	_float fPositionC = (_float)(rand() % 9 * 10);
	_float fPositionD = (_float)(rand() % 9);
	
	_float fPositionX = fPositionA + fPositionB + fPositionC + fPositionD;

	_float fSizeA = (_float)((rand() % 7 + 4)*0.1f);
	/* ============= */

	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SizeX = 64.f * fSizeA;
	SizeY = 64.f * fSizeA;

	SetX = fPositionX;
	SetY = 0.f;

	fPlusY = (_float)((rand() % 9 + 5) * 0.2f);
	m_fFrame = (_float)(rand() % 5);

	m_fDepth = 0.08f; // 깊이
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CMenu_Burble::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	dTime += TimeDelta;

	_float fRotate = (_float)((rand() % 9 + 5.f)*0.1f);
	m_pTransformCom->NowRotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRotate));

	if (0.01 < dTime)
	{
		SetY += fPlusY;
		dTime = 0.0;
	}
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	if (730.f < SetY)
		m_bDead = true;
	
	return RESULT_NOPROBLEM;
}

_int CMenu_Burble::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CMenu_Burble::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CMenu_Burble::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	//CHECK_FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Texture_Menu_Burble"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Menu_Simbol"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CMenu_Burble::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

CMenu_Burble * CMenu_Burble::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMenu_Burble*	pInstance = new CMenu_Burble(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMenu_Burble");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMenu_Burble::Clone(void * pArg)
{
	CMenu_Burble*	pInstance = new CMenu_Burble(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMenu_Burble");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMenu_Burble::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
