#include "stdafx.h"
#include "Crow.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "Terrain.h"

CCrow::CCrow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext) 
	: CMonster(pDevice, pDeviceContext)
{
}

CCrow::CCrow(const CCrow& rhs)
	: CMonster(rhs)
{
}

HRESULT CCrow::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrow::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 5.f;
	TransformDesc.fRotationPerSec = 10.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 루트노드인덱스 초기화
	if (FAILED(SetUp_RootNodeIndex()))
		return E_FAIL;

	// 애니메이션 설정
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// 쉐이더패스 설정
	m_iPass = 13;

	// 현재 레벨에 있는 Player 와 Player 의 Transform 을 가져옴
	if (m_pPlayer == nullptr)
	{
		m_pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);
		m_pPlayerTransform = m_pPlayer->Get_Transform();

		if (m_pPlayer == nullptr || m_pPlayerTransform == nullptr)
			return E_FAIL;
	}

	// 시작 위치 설정
	if (pArg != nullptr)
	{
		memcpy(&m_CrowDesc, pArg, sizeof(CROWDESC));
		m_pTransformCom->Set_Position(m_CrowDesc.vPos);
	}

	// 터레인설정
	m_pTerrain = static_cast<CTerrain*>(m_pGameInstance->Get_Front(LEVEL_RAPUNZEL_BOSS, TEXT("Layer_Terrain")));

	// 파티클 생성
	//CEffect_Manager::GetInstance()->Create_CrowParticle(m_pTransformCom, &m_bDead);

	m_pBigTree = m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_BigTree"));

	return S_OK;
}

HRESULT CCrow::NativeConstruct_Pool(void * pArg)
{
	return S_OK;
}

_int CCrow::Tick(_double TimeDelta)
{
	TimeDelta = TimeDelta * m_pStatusCom->Get_SlowSpeed();

	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead || true == m_pBigTree->Get_Dead())
	{
		return RESULT_DEAD;
	}

	if (m_bReadyToDie)
		m_bDead = true;

	// 애니메이션 반복
	if (m_pModelCom->Check_CurAnim_And_Finished(0))
		Set_AnimAndReset(m_pModelCom, 0);

	// 플레이어 추적
	if (m_pStatusCom->Get_Stun() == false)
	{
		m_pTransformCom->Get_PositionFloat4_Pointer();
		ChasePlayer(TimeDelta);
	}

	// 죽으면 RESULT_DEAD 반환
	if (m_pGameInstance->Get_LevelIndex() != LEVEL_MAPTOOL)
	{
		if (true == m_bDead)
		{
			// 할당해제를 하기 직전에 이 객체가 가진 모든 충돌체를 플레이어의 타겟 목록에서 제거한다.
			static_cast<CPlayer*>(m_pPlayer)->Eliminate_DeadTarget(this);
			return RESULT_DEAD;
		}
	}

	// 스테이터스 업데이트
	m_pStatusCom->Update(TimeDelta);

	// 모델 업데이트
	if (m_pStatusCom->Get_Stun() == false)
	{
		if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_TEST || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
		else
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);
	}

	// 콜라이더 위치 업데이트
	m_pHitColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pAtkColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// 콜리전매니저에 콜라이더 등록 (Tick 의 마지막에서 등록해야함)
	m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom);

	return RESULT_NOPROBLEM;
}

_int CCrow::LateTick(_double TimeDelta)
{
	TimeDelta = TimeDelta * m_pStatusCom->Get_SlowSpeed();

	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 충돌 검사 (LateTick 에서 실행할 것)
	OnCollision(TimeDelta);

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) &&
		FALSE == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// 콜라이더 디버그 렌더링
	m_pRendererCom->Add_Debug(m_pHitColliderCom);
	m_pRendererCom->Add_Debug(m_pAtkColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CCrow::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// 쉐이더 전역 상수에 값 전달
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// 모델이 갖고있는 머테리얼 수 가져와서 쉐이더의 텍스처 전역 상수에 값 전달하고 렌더링
	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
		m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CCrow::PostRender(_double TimeDelta)
{
}

_bool CCrow::Damaged(CCollider * pAtkCollider, _float fDamaged)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true)
		return false;

	// 데미지 받음
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// 죽는 이펙트
	CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 1.5f, 0.f);

	// 충돌하면 m_bReadyToDie 를 true 로
	m_bReadyToDie = true;

	return true;
}

HRESULT CCrow::SetUp_Components()
{
	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 0.1f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel   = 1;
	StatusDesc.fEXP     = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, -0.5f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Hit"), (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Atk"), (CComponent**)&m_pAtkColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC tNaviDesc;
	tNaviDesc.iCurrentIndex = 0;
	tNaviDesc.pParent = m_pTransformCom;
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_Crow"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CCrow::SetUp_RootNodeIndex()
{
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

	return S_OK;
}

HRESULT CCrow::SetUp_Animations()
{
	m_pModelCom->Add_OnceList(0);
	m_pModelCom->Set_TempIndex(0);

	return S_OK;
}

HRESULT CCrow::SetUp_ConstantTable()
{
	_bool bBlur = false;
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

	_float4		vColor = _float4(0.5f, 0.f, 0.f, 0.f); 
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &vColor, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_float4 vCam;
	XMStoreFloat4(&vCam, CGameInstance::GetInstance()->Get_CamPosition());

	if (FAILED(m_pShaderCom->Set_RawValue("g_vWorldCamPos", &vCam, sizeof(_float4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_bool isUsingRim = true;
	if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &isUsingRim, sizeof(_bool))))
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

void CCrow::OnCollision(_double TimeDelta)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true)
		return;

	// 플레이어와 충돌 검사(Hit)
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		if (m_pGameInstance->Collision_Enter_Sphere(m_pHitColliderCom, pDestCollider))
		{
			CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 1.5f, 0.f);
			m_bReadyToDie = true;
		}
	}

	if (m_bReadyToDie == true)
		return;

	// 몬스터끼리 밀어내기
	pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		if (pDestCollider == m_pHitColliderCom)
			continue;

		if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom, pDestCollider))
		{
			CGameObject* pDestObject = pDestCollider->Get_Parent();
			CTransform* pDestTransform = pDestObject->Get_Transform();
			CCollider* pDestCollider = pDestObject->Get_HitCollider();

			_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;
			_float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;

			// 겹친부분 크기
			_float fCollisionSize = fDestColliderRadius + fSourColliderRadius - m_pTransformCom->Get_Distance(pDestTransform);

			// 방향계산
			_vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

			// 겹치부분만큼 밀려나기
			_vector vPos = m_pTransformCom->Get_Position();
			m_pTransformCom->Set_Position(vPos - vDir * fCollisionSize);
		}
	}
}

void CCrow::ChasePlayer(_double TimeDelta)
{
	m_TimeAcc += TimeDelta;

	if (m_TimeAcc < 3.0) // 3초 까지는 랜덤한 방으로 이동
	{
		if (m_pGameInstance->Get_LevelIndex() != LEVEL_TEST)
		{
			m_pTransformCom->Go_Vector(m_CrowDesc.vRandomDir, TimeDelta);
			m_pTransformCom->Turn_To_Direction(m_CrowDesc.vRandomDir, TimeDelta);

			_float fY = vecy(m_pTerrain->Compute_PositionOnTerrain(m_pTransformCom->Get_Position())) + 1.f;
			if (XMVectorGetY(m_pTransformCom->Get_Position()) < fY)
				m_pTransformCom->Set_Position(XMVectorSetY(m_pTransformCom->Get_Position(), fY));
		}
	}
	else // 이후 플레이어 추적
	{
		_vector vDir = m_pTransformCom->Get_Dir(m_pPlayerTransform) + __vector(0.f, 0.5f, 0.f, 0.f);
		m_pTransformCom->MakeSameDirection(vDir);
		_vector vTargetPos = m_pPlayerTransform->Get_Position() + __vector(0.f, 0.5f, 0.f, 0.f);
		m_pTransformCom->Go_To_Target(vTargetPos, 0.f, TimeDelta);
	}
}

CCrow* CCrow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CCrow* pInstance = new CCrow(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CCrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CCrow::Clone(void* pArg)
{
	CCrow* pInstance = new CCrow(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CCrow");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCrow::Free()
{
	__super::Free();

	Safe_Release(m_pStatusCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pAtkColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}