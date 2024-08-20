#include "stdafx.h"
#include "..\public\UI_BossTarget.h"
#include "GameInstance.h"
#include "GameManager.h"

#include "Player.h"
#include "Combat_Assistant.h"
#include "Collider.h"

CUI_BossTarget::CUI_BossTarget(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CUI_Parents(pDevice, pDeviceContext)
{
}

CUI_BossTarget::CUI_BossTarget(const CUI_BossTarget & rhs)
	: CUI_Parents(rhs)
{
}

HRESULT CUI_BossTarget::NativeConstruct_Prototype()
{
	CHECK_FAILED(__super::NativeConstruct_Prototype());

	return S_OK;
}

HRESULT CUI_BossTarget::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 5.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	tTransformDesc.fScalingPerSec = 5.f;

	CHECK_FAILED(__super::NativeConstruct(&tTransformDesc));

//	iCom_Texture = (*(_int*)pArg); // 텍스쳐 정보
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

_int CUI_BossTarget::Tick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::Tick(TimeDelta)));

	if (nullptr == m_pPlayer)
	{
		m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(GM->Get_CurrentLevel(), TEXT("Layer_Player")));

		if (nullptr == m_pPlayer)
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	return RESULT_NOPROBLEM;
}

_int CUI_BossTarget::LateTick(_double TimeDelta)
{
	RETURN_NEGATIVE(if (0 > __super::LateTick(TimeDelta)));

	if (LEVEL_TWILIGHT == GM->Get_CurrentLevel())
	{
		CGameObject* pBoss = m_pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_Nightmare"));
		if (nullptr != pBoss)
		{
			target_position = m_pGameInstance->Get_Back(LEVEL_TWILIGHT, TEXT("Layer_Nightmare"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			Position_Fnc();
		}
	}
	else if (LEVEL_RAPUNZEL_BOSS == GM->Get_CurrentLevel())
	{
		CGameObject* pBoss = m_pGameInstance->Get_Back(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_BigTree"));
		if (nullptr != pBoss)
		{
			target_position = m_pGameInstance->Get_Back(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_BigTree"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			Position_Fnc();
		}
	}
	else if (LEVEL_FROZEN == GM->Get_CurrentLevel())
	{
		CGameObject* pBoss = m_pGameInstance->Get_Back(LEVEL_FROZEN, TEXT("Layer_IceMan"));
		if (nullptr != pBoss)
		{
			target_position = m_pGameInstance->Get_Back(LEVEL_FROZEN, TEXT("Layer_IceMan"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			Position_Fnc();
		}
	}
	else if (LEVEL_KEYGRAVE == GM->Get_CurrentLevel())
	{
		CGameObject* pBoss = m_pGameInstance->Get_Back(LEVEL_KEYGRAVE, TEXT("Layer_FrozenWolf"));
		if (nullptr != pBoss)
		{
			target_position = m_pGameInstance->Get_Back(LEVEL_KEYGRAVE, TEXT("Layer_FrozenWolf"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			Position_Fnc();
		}
	}
	else if (LEVEL_FINALBOSS == GM->Get_CurrentLevel())
	{
		CGameObject* pBoss = m_pGameInstance->Get_Back(LEVEL_FINALBOSS, TEXT("Layer_Goat"));
		if (nullptr != pBoss)
		{
			target_position = m_pGameInstance->Get_Back(LEVEL_FINALBOSS, TEXT("Layer_Goat"))->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			Position_Fnc();
		}
	}
	else
	{
	}

	if (true == bPositionCheck && true == GM->Get_UIRender())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	return RESULT_NOPROBLEM;
}

HRESULT CUI_BossTarget::Render()
{
	//if (false == m_bRender)
	//	return S_OK;

	//if (false == bTarget_Render)
	//	return S_OK;

	if (nullptr == m_pVIBufferCom)
		return E_FAIL;

	CHECK_FAILED(SetUp_ConstantTable());
	m_pVIBufferCom->Render(m_pShaderCom, 0);

	return S_OK;
}

HRESULT CUI_BossTarget::SetUp_Components()
{
	/* For.Com_Renderer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom));

	/* For.Com_VIBuffer */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom));

	/* For.Com_Shader */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom));

	/* For.Com_Texture */
	CHECK_FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_BossLockOn"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom));

	return S_OK;
}

HRESULT CUI_BossTarget::SetUp_ConstantTable()
{
	CHECK_FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix"));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &XMMatrixIdentity(), sizeof(_float4x4)));
	CHECK_FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4)));
	CHECK_FAILED(m_pTextureCom->SetUp_OnShader(m_pShaderCom, "g_DiffuseTexture"));

	return S_OK;
}

void CUI_BossTarget::Position_Fnc()
{
	// 뷰 스페이스로 연산
	_matrix view_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_vector position_in_view_space = XMVector3TransformCoord(target_position, view_mat);

	// 뷰 스페이스 상에서의 타겟의 위치 보관(카메라 기준 전면, 후면 판별하기 위함)
	XMStoreFloat4(&m_vTargetPosition, position_in_view_space);

	// 투영 스페이스로 연산
	_matrix proj_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	_vector position_in_proj_space = XMVector3TransformCoord(position_in_view_space, proj_mat);

	_float4	fproj_position;
	_float4 fproj_position2;

	XMStoreFloat4(&fproj_position, position_in_proj_space);

	if (0 <= m_vTargetPosition.z)
	{
		bPosition_Z = true;
	}
	else
	{
		bPosition_Z = false;
	}

	_vector vproj_position;

	if (true == bPosition_Z)
	{
		if (0.95f <= fproj_position.x)
		{
			fproj_position.x = 0.95f;
			bPositionCheck = true;
		}
		else if (-0.95f >= fproj_position.x)
		{
			fproj_position.x = -0.95f;
			bPositionCheck = true;
		}
		else if (0.95f > fproj_position.x
			&& -0.95f < fproj_position.x
			&& 0.95f > fproj_position.y
			&& -0.95f < fproj_position.y)
		{
			bPositionCheck = false;
		}

		if (0.95f <= fproj_position.y)
		{
			fproj_position.y = 0.95f;
			bPositionCheck = true;
		}
		else if (-0.95f >= fproj_position.y)
		{
			fproj_position.y = -0.95f;
			bPositionCheck = true;
		}
		else
		{
		}
		vproj_position = XMVectorSet(fproj_position.x, fproj_position.y, 0.f, 1.f);
	}
	else
	{
		bPositionCheck = true;

		if (0.95f <= fproj_position.x)
		{
			fproj_position.x = 0.95f;
		}
		else if (-0.95f >= fproj_position.x)
		{
			fproj_position.x = -0.95f;
		}
		else
		{
		}

		fproj_position.y = -0.95f;
		vproj_position = XMVectorSet(-fproj_position.x, fproj_position.y, 0.f, 1.f);
	}
	// 위치 조정
	_vector position_viewport = XMVectorSet(
		(XMVectorGetX(vproj_position) + 1.f) * +(g_iWinCX * 0.5f), // X
		(XMVectorGetY(vproj_position) - 1.f) * -(g_iWinCY * 0.5f), // Y
		0.f, 1.f); // Z, W

	// 최종 위치
	_vector final_position_modify = __vector(vecx(position_viewport) - g_iWinCX * 0.5f, -vecy(position_viewport) + g_iWinCY * 0.5f, 0.f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, final_position_modify);
}

CUI_BossTarget * CUI_BossTarget::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUI_BossTarget*	pInstance = new CUI_BossTarget(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CUI_BossTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CUI_BossTarget::Clone(void * pArg)
{
	CUI_BossTarget*	pInstance = new CUI_BossTarget(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CUI_BossTarget");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CUI_BossTarget::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
}
