#include "stdafx.h"
#include "..\public\UI_BackSimbol.h"
#include "GameInstance.h"
#include "GameManager.h"

CUI_BackSimbol::CUI_BackSimbol(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_BackSimbol::CUI_BackSimbol(const CUI_BackSimbol & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_BackSimbol::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_BackSimbol::NativeConstruct(void * pArg)
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

	_float fPositionA;
	_float fPositionB;
	_float fPositionC;

	/* rand() */

	_float fSizeA = (_float)(rand() % 2 + 1);
	_float fSizeB = (_float)(rand() % 9 + 1) * 0.1f;
	
	_float fSize = fSizeA + fSizeB;

	if (0 == GM->Get_GCnt())
	{
		fPositionA = (_float)(rand() % 2 * 100.f);
		fPositionB = (_float)(rand() % 9 * 10.f);
		fPositionC = (_float)(rand() % 9);
	}
	else if (1 == GM->Get_GCnt())
	{
		fPositionA = (_float)((rand() % 2 + 2) * 100.f);
		fPositionB = (_float)(rand() % 9 * 10.f);
		fPositionC = (_float)(rand() % 9);
	}
	else if (2 == GM->Get_GCnt())
	{
		fPositionA = (_float)((rand() % 2 + 4) * 100.f);
		fPositionB = (_float)(rand() % 9 * 10.f);
		fPositionC = (_float)(rand() % 9);
	}
	else
	{
		fPositionA = (_float)(rand() % 4 * 100.f);
		fPositionB = (_float)(rand() % 9 * 10.f);
		fPositionC = (_float)(rand() % 9);
	}

	_float fPositionY = fPositionA + fPositionB + fPositionC;

	_float fPositionD = (_float)(rand() % 3 * 100.f);
	_float fPositionE = (_float)(rand() % 9 * 10.f);
	_float fPositionF = (_float)(rand() % 9);
	
	_float fPositionX = fPositionD + fPositionE + fPositionF;

	fRotate = (_float)((rand() % 9 + 5.f)*0.1f);



	/*====*/

	SizeX = (112.f * 0.5f) * fSize;
	SizeY = (112.f * 0.5f) * fSize;
	SettingX = m_fX - (g_iWinCX*0.5f);
	SettingY = m_fY - (g_iWinCY*0.5f);
	SetX = fPositionX;
	SetY = 720.f - fPositionY;


	m_fDepth = 0.7f; // 깊이
	m_fFrame = (_float)(rand() % 5);

	/*====*/

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	return S_OK;
}

_int CUI_BackSimbol::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (true == m_bDead)
		return RESULT_DEAD;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pTransformCom->NowRotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRotate));

	SetY -= rand() % 9 * 0.1f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(SettingX + SetX, SettingY + SetY, 0.f, 1.f));

	if (0.f > SetY)
	{
		m_bDead = true;
		GM->Math_SimbolCnt(-1);
	}
		

	return RESULT_NOPROBLEM;
}

_int CUI_BackSimbol::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_BackSimbol::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_BackSimbol::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Copmonent_Texture_Logo_Back"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_BackSimbol::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture", (_int)m_fFrame));

	return S_OK;
}

CUI_BackSimbol * CUI_BackSimbol::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_BackSimbol*	pInstance = new CUI_BackSimbol(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_BackSimbol");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_BackSimbol::Clone(void * pArg)
{
	CUI_BackSimbol*	pInstance = new CUI_BackSimbol(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_BackSimbol");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BackSimbol::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
