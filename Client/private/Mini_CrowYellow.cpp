#include "stdafx.h"
#include "..\public\Mini_CrowYellow.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Progress_Manager.h"
#include "SoundManager.h"
#include "Player.h"

CMini_CrowYellow::CMini_CrowYellow(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMini_CrowYellow::CMini_CrowYellow(const CMini_CrowYellow & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMini_CrowYellow::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMini_CrowYellow::NativeConstruct(void * pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fScalingPerSec = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pModelCom->Add_OnceList(0);
	m_pModelCom->Set_TempIndex(0);

	_int	iCreate_Random = rand() % 3;
	ChaseRandom = rand() % 2;

	Create_Y = (_float)(rand() % 6) + 4.f;

	if (0 == iCreate_Random)
	{
		Create_X = (_float)(88.f + (rand() % 9 * 10.f) + (rand() % 9));
		Create_Z = 95.f;
	}
	else if (1 == iCreate_Random)
	{
		Create_X = 68.f;
		Create_Z = (_float)(95.f + (rand() % 2 * 10.f) + (rand() % 9));
	}
	else
	{
		Create_X = 188.f;
		Create_Z = (_float)(95.f + (rand() % 2 * 10.f) + (rand() % 9));
	}

	dCrowSpeedA = (_double)((rand() % 2) + 1);
	dCrowSpeedB = (_double)((rand() % 9) * 0.1);

	dCrowSpeed = dCrowSpeedA + dCrowSpeedB;

	m_pTransformCom->Scaled(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Create_X, Create_Y, Create_Z, 1.f));
	return S_OK;
}

_int CMini_CrowYellow::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead)
		return RESULT_DEAD;

	// 애니메이션 반복
	if (m_pModelCom->Check_CurAnim_And_Finished(0))
	{
		m_pModelCom->ResetAnimation(0);
		m_pModelCom->Set_TempIndex(0);
	}

	_vector	TestPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

	/* === */

	LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Front(current_level, TEXT("Layer_Player")));
	vPlayer_Pos = m_pPlayer->Get_Transform()->Get_State(CTransform::STATE_POSITION);

	/* === */

	vCrow_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4 fCrow_Pos;
	XMStoreFloat4(&fCrow_Pos, vCrow_Pos);

	if (130.f <= fCrow_Pos.x
		&& 136.f >= fCrow_Pos.x
		&& 158.f <= fCrow_Pos.z
		&& 162.f >= fCrow_Pos.z
		)
	{
		GM->Math_Minigame_Score(+100);
		m_bDead = true;
	}
	else if (120.f <= fCrow_Pos.x
		&& 126.f >= fCrow_Pos.x
		&& 158.f <= fCrow_Pos.z
		&& 162.f >= fCrow_Pos.z
		)
	{
		GM->Math_Minigame_Score(+100);
		m_bDead = true;
	}



	/* === */

	if (0 == ChaseRandom)
	{
		vChase_Pos = XMVectorSet(133.f, 2.f, 160.f, 1.f);
	}
	else if (1 == ChaseRandom)
	{
		vChase_Pos = XMVectorSet(123.f, 2.f, 160.f, 1.f);
	}
	else
	{
		m_bDead = true;
	}

	m_pTransformCom->LookAt(vChase_Pos);
	m_pTransformCom->Chase(vChase_Pos, TimeDelta * dCrowSpeed);

	return RESULT_NOPROBLEM;
}

_int CMini_CrowYellow::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	/* 월드상의 현재 좌표*/
	_vector vWorldPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 뷰 스페이스 상에서의 타겟의 위치 보관(카메라 기준 전면, 후면 판별하기 위함)
	XMStoreFloat4(&m_vTargetPosition, vWorldPosition);

	/* 뷰스페이스 연산 */
	_matrix	view_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_vector position_in_view_space = XMVector3TransformCoord(vWorldPosition, view_mat);

	/* 투영스페이스 연산 */
	_matrix proj_mat = m_pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);
	_vector position_in_proj_space = XMVector3TransformCoord(position_in_view_space, proj_mat);

	// 위치 조정
	_vector position_viewport = XMVectorSet(
		(XMVectorGetX(position_in_proj_space) + 1.f) * +(g_iWinCX * 0.5f), // X
		(XMVectorGetY(position_in_proj_space) - 1.f) * -(g_iWinCY * 0.5f), // Y
		0.f, 1.f); // Z, W

				   // 최종 위치
	_vector final_position = __vector(vecx(position_viewport) - g_iWinCX * 0.5f, -vecy(position_viewport) + g_iWinCY * 0.5f, 0.f, 1.f);
	_float4	f_final_position;
	XMStoreFloat4(&f_final_position, final_position);

	if (0.2f < m_vTargetPosition.z
		&& 50.f >= f_final_position.x
		&& -50.f <= f_final_position.x
		&& 50.f >= f_final_position.y
		&& -50.f <= f_final_position.y)
	{
		bTarget_On = true;
	}
	else
	{
		bTarget_On = false;
	}

	if (true == bTarget_On && m_pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		PM->Set_CrowTarget(true);
		GM->Math_Minigame_Score(-100);
		SM->Sound_Command(L"Crow_Shot.wav", 1.f);
		m_bDead = true;
	}
		
	/*
	Crow 의 좌표를 투영까지 보낸 뒤,
	투영스페이스 안의 좌표가 중앙에서의 일정 범위 안에 들어왔을때 버튼누르면 죽는 모션 후 삭제
	*/

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
	return RESULT_NOPROBLEM;
}

HRESULT CMini_CrowYellow::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	m_iPass = 13;
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CMini_CrowYellow::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Collider */
	//CCollider::COLLIDERDESC ColliderDesc;
	//ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	//ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
	//ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
	//ColliderDesc.vTranslation = _float4(0.f, 0.f, -0.5f, 1.f);
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_MiniGame"), (CComponent**)&m_pMiniGameColliderCom, &ColliderDesc)))
	//	return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_Crow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMini_CrowYellow::SetUp_ConstantTable()
{
	_bool bBlur = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &bBlur, sizeof(_bool))))
	{
		return E_FAIL;

	}

	_float		fRim = 2.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRimPower", &fRim, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float		fRimRange = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRimRange", &fRimRange, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4		vColor = _float4(1.f, 1.f, 0.f, 0.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 월드행렬
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// 뷰행렬
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	// 투영행렬
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;


	return S_OK;
}

CMini_CrowYellow * CMini_CrowYellow::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMini_CrowYellow* pInstance = new CMini_CrowYellow(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMini_CrowYellow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CMini_CrowYellow::Clone(void * pArg)
{
	CMini_CrowYellow* pInstance = new CMini_CrowYellow(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMini_CrowYellow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMini_CrowYellow::Free()
{
	__super::Free();

	//Safe_Release(m_pMiniGameColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
