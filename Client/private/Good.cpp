#include "stdafx.h"
#include "..\public\Good.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "Combat_Assistant.h"
#include "Collider.h"


CGood::CGood(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CGood::CGood(const CGood & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CGood::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CGood::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	fCrow_Pos = (*(_float4*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;
	m_fDepth = 0.2f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	SizeX = 100.f;
	SizeY = 50.f;

	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	ZeroMemory(&m_vTargetPosition, sizeof(_float4));

	return S_OK;
}

_int CGood::Tick(_double TimeDelta)
{
	if (true == m_bDead)
		return RESULT_DEAD;

	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	dDeadTime += TimeDelta;
	if (1.0 < dDeadTime)
		m_bDead = true;
	
	Position_Fnc();

	return RESULT_NOPROBLEM;
}

_int CGood::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CGood::Render()
{
	if (0.2f > m_vTargetPosition.z)
		return S_OK;

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CGood::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_Good"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CGood::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

void CGood::Position_Fnc()
{
	target_position = XMVectorSet(fCrow_Pos.x, fCrow_Pos.y, fCrow_Pos.z, 1.f);

	// 뷰 스페이스로 연산
	_matrix view_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_vector position_in_view_space = XMVector3TransformCoord(target_position, view_mat);

	// 뷰 스페이스 상에서의 타겟의 위치 보관(카메라 기준 전면, 후면 판별하기 위함)
	XMStoreFloat4(&m_vTargetPosition, position_in_view_space);

	// 투영 스페이스로 연산
	_matrix proj_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	_vector position_in_proj_space = XMVector3TransformCoord(position_in_view_space, proj_mat);

	// 위치 조정
	_vector position_viewport = XMVectorSet(
		(XMVectorGetX(position_in_proj_space) + 1.f) * +(g_iWinCX * 0.5f), // X
		(XMVectorGetY(position_in_proj_space) - 1.f) * -(g_iWinCY * 0.5f), // Y
		0.f, 1.f); // Z, W

	// 최종 위치
	_vector final_position = __vector(vecx(position_viewport) - g_iWinCX * 0.5f, -vecy(position_viewport) + g_iWinCY * 0.5f, 0.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position);
}

CGood * CGood::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGood*	pInstance = new CGood(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGood::Clone(void * pArg)
{
	CGood*	pInstance = new CGood(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGood");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGood::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
