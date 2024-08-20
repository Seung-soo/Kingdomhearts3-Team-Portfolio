#include "stdafx.h"
#include "..\public\UI_Target.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "Combat_Assistant.h"

#include "Collider.h"

CUI_Target::CUI_Target(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_Target::CUI_Target(const CUI_Target & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_Target::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_Target::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
	CHECK_FAILED(SetUp_Components());

	m_fSizeX = g_iWinCX;
	m_fSizeY = g_iWinCY;
	m_fX = 0.f;
	m_fY = 0.f;
	m_fDepth = 0.2f;

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((float)g_iWinCX, (float)g_iWinCY, 0.f, 1.f)));

	SizeX = 64.f;
	SizeY = 64.f;
	
	m_pTransformCom->Scaled(XMVectorSet(SizeX, SizeY, 1.f, 0.f));
	ZeroMemory(&m_vTargetPosition, sizeof(_float4));

	return S_OK;
}

_int CUI_Target::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (nullptr == m_pPlayer)
	{
		LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));

		if (nullptr == m_pPlayer)
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	return RESULT_NOPROBLEM;
}

_int CUI_Target::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	C_ASSIST::ASSIST_TYPE target_type = m_pPlayer->Get_CombatAssistant()->Get_TargetType();
	CCollider* target_collider = nullptr;

	switch (iCom_Texture)
	{
	case Engine::CUI_Parents::UI_LOCKON1:

		// UI_LOCKON1 == Yellow
		// LOCKON이 아니고 ATTACK도 아닐 때
		if (C_ASSIST::ASSIST_LOCKON != target_type &&
			C_ASSIST::ASSIST_ATTACK != target_type)
		{
			m_bRender = false;
			return RESULT_NOPROBLEM;
		}

		// LOCKON이거나 ATTACK일 때
		else
		{
			m_bRender = true;
			if (C_ASSIST::ASSIST_LOCKON == target_type)
				target_collider = m_pPlayer->Get_CombatAssistant()->Get_Collider(C_ASSIST::ASSIST_LOCKON);
			else
				target_collider = m_pPlayer->Get_CombatAssistant()->Get_Collider(C_ASSIST::ASSIST_ATTACK);
		}

		break;
	case Engine::CUI_Parents::UI_LOCKON2_1:
	case Engine::CUI_Parents::UI_LOCKON2_2:

		// UI_LOCKON2_1, UI_LOCKON2_2 == Blue
		// FASTEN이 아닐 때
		if (C_ASSIST::ASSIST_FASTEN != target_type)
		{
			m_bRender = false;
			return RESULT_NOPROBLEM;
		}

		// FASTEN일 때
		else
		{
			m_bRender = true;
			target_collider = m_pPlayer->Get_CombatAssistant()->Get_Collider(C_ASSIST::ASSIST_FASTEN);
		}
	
		break;
	}

	// Target Collider Position
	if (target_collider == nullptr)
		return RESULT_NOPROBLEM;
	_vector target_position = target_collider->Get_Center();
	

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

	if (false == GM->Get_bCameraMove() && true == GM->Get_UIRender())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_Target::Render()
{
	if (false == m_bRender)
		return S_OK;

	if (0.2f > m_vTargetPosition.z)
		return S_OK;

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_Target::SetUp_Components()
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
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_LOCKON2_1:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn2_1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	case Engine::CUI_Parents::UI_LOCKON2_2:
		CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_LockOn2_2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));
		break;
	default:
		break;
	}

	return S_OK;
}

HRESULT CUI_Target::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

CUI_Target * CUI_Target::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_Target*	pInstance = new CUI_Target(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_Target::Clone(void * pArg)
{
	CUI_Target*	pInstance = new CUI_Target(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_Target");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_Target::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
