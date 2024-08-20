#include "stdafx.h"
#include "..\public\UI_LockOn.h"
#include "GameInstance.h"
#include "SkullSoldier.h"

CUI_LockOn::CUI_LockOn(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_LockOn::CUI_LockOn(const CUI_LockOn & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_LockOn::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_LockOn::NativeConstruct(void * pArg)
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

	// Setting =========

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_LOCKON1:
		m_UI.SizeX = 64.f;
		m_UI.SizeY = 64.f;
		m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
		m_UI.SettingY = m_fY - (g_iWinCY*0.5f);
		m_UI.SetX = 300.f;
		m_UI.SetY = 300.f;

		m_fDepth = 0.0f; // 깊이
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::UI_LOCKON2_1:
		m_UI.SizeX = 80.f;
		m_UI.SizeY = 80.f;
		m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
		m_UI.SettingY = m_fY - (g_iWinCY*0.5f);
		m_UI.SetX = 400.f;
		m_UI.SetY = 300.f;

		m_fDepth = 0.1f; // 깊이
		m_fFrame = 0.f;
		break;
	case Engine::CUI_Parents::UI_LOCKON2_2:
		m_UI.SizeX = 80.f;
		m_UI.SizeY = 80.f;
		m_UI.SettingX = m_fX - (g_iWinCX*0.5f);
		m_UI.SettingY = m_fY - (g_iWinCY*0.5f);
		m_UI.SetX = 400.f;
		m_UI.SetY = 300.f;

		m_fDepth = 0.0f; // 깊이
		m_fFrame = 0.f;
		break;

	default:
		break;
	}

	m_pTransformCom->Scaled(XMVectorSet(m_UI.SizeX, m_UI.SizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UI.SettingX + m_UI.SetX, m_UI.SettingY + m_UI.SetY, 0.f, 1.f)); // UI에도 적용가능 
	return S_OK;
}

_int CUI_LockOn::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	 
	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_LOCKON1:
		break;
	case Engine::CUI_Parents::UI_LOCKON2_1:
		m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(5.f));
		break;
	case Engine::CUI_Parents::UI_LOCKON2_2:
		break;
	default:
		break;
	}

	return _int();
}

_int CUI_LockOn::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	
	//==
	_vector m_vRight;
	_vector	m_vViewPortPosition;
	_float m_fz;
	//_vector m_vMyPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector m_vMyPosition = static_cast<CTransform*>(pGameInstance->Get_Component(LEVEL_TWILIGHT, TEXT("Layer_SkullSolidier"), TEXT("Com_Transform")))->Get_State(CTransform::STATE_POSITION);
	//==

	_matrix ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	//ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	//m_pTransformCom->Set_State(CTransform::STATE_RIGHT, ViewMatrix.r[0] * m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT));
	//m_pTransformCom->Set_State(CTransform::STATE_UP, ViewMatrix.r[1] * m_pTransformCom->Get_Scale(CTransform::STATE_UP));
	//m_pTransformCom->Set_State(CTransform::STATE_LOOK, ViewMatrix.r[2] * m_pTransformCom->Get_Scale(CTransform::STATE_LOOK));

	m_vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	m_vViewPortPosition = XMVector3TransformCoord(m_vMyPosition, ViewMatrix);
	m_fz = XMVectorGetZ(m_vViewPortPosition);

	_matrix  ProjMatrix;
	ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	m_vViewPortPosition = XMVector3TransformCoord(m_vMyPosition, ProjMatrix);

	m_vViewPortPosition = XMVectorSet((XMVectorGetX(m_vViewPortPosition) + 1) * (g_iWinCX * 0.5f)
		, (XMVectorGetY(m_vViewPortPosition) - 1) * -(g_iWinCY * 0.5f), 0.f, 1.f);

	/* 임시로 몬스터 포지션을 만들어서 줘 보기*/
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vViewPortPosition);

	 //===
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	return _int();
}

HRESULT CUI_LockOn::Render()
{
	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());

	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_LockOn::SetUp_Components()
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
	case Engine::CUI_Parents::UI_LOCKON1:
		CHECK_FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Texture_UI_LockOn1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_LOCKON2_1:
		CHECK_FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Texture_UI_LockOn2_1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_LOCKON2_2:
		CHECK_FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Texture_UI_LockOn2_2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}
	return S_OK;
}

HRESULT CUI_LockOn::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_LockOn * CUI_LockOn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_LockOn*	pInstance = new CUI_LockOn(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_LockOn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_LockOn::Clone(void * pArg)
{
	CUI_LockOn*	pInstance = new CUI_LockOn(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_LockOn");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_LockOn::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
