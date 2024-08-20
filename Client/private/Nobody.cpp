#include "stdafx.h"
#include "Nobody.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "Player.h"
#include "Effect_Manager.h"

// State
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h" 
#include "State_MonsterChase.h"
#include "State_MonsterAtk.h"
#include "State_MonsterRunAway.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"

CNobody::CNobody(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CNobody::CNobody(const CNobody& rhs)
	: CMonster(rhs)
{
}

HRESULT CNobody::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNobody::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 20.f;
	TransformDesc.fRotationPerSec = 1.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 몬스터 타입 설정
	m_MonsterType = MONSTERTYPE_NOBODY;

	// 컴포넌트 초기화
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// 루트노드인덱스 초기화
	if (FAILED(SetUp_RootNodeIndex()))
		return E_FAIL;

	// 애니메이션 설정
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// 콜라이더 정보 초기화
	Ready_Collider_Info();

	// 시작 상태 설정 (Idle)
	m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());

	// 쉐이더패스 설정
	m_iPass = 6;

	// 시작 위치 설정
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	m_pTransformCom->Scaled(__vector(0.1f, 0.1f, 0.1f, 0.f));

	m_bIsBoss = true;

	return S_OK;
}

HRESULT CNobody::NativeConstruct_Pool(void * pArg)
{
	// 시작 위치 설정
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	//	게임 매니저 몬스터 리스트에 추가
	GM->Add_Monster(this);

	return S_OK;
}

_int CNobody::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// 죽으면 RESULT_DEAD 반환
	if (true == m_bDead)
		return RESULT_DEAD;

	// 피격 애니메이션 재생 쿨타임 처리
	m_OnDamagedAnimTimeAcc += TimeDelta;

	// 상태 변경 처리
	ChangeState(TimeDelta);

	// 상태머신 Action 실행
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// MonsterCooler 업데이트 돌려서 쿨타임 초기화
	m_pMonsterCoolerCom->Update(TimeDelta);

	// 모델 업데이트
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_TEST || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);

	// 넉백 업데이트
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom);

	// 콜라이더의 위치 계산
	Collider_Transform_Update();

	// 콜라이더 위치 업데이트
	// 피격 콜라이더
	m_pHitColliderCom->Update_Transform(m_HitCollider_TransformMatrix);
	// 공격 콜라이더
	for (_uint i = 0; i < 2; ++i)
		m_pAtkColliderCom[i]->Update_Transform(m_AtkCollider_TransformMatrix[i]);

	// 콜리전매니저에 콜라이더 등록 (Tick 의 마지막에서 등록해야함)
	// 공격 콜라이더는 공격 상태일 때만 등록
	if (m_bReadyToDie == false)
		m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 2; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom[i]);
	}

	return RESULT_NOPROBLEM;
}

_int CNobody::LateTick(_double TimeDelta)
{
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
	// 피격 콜라이더
	m_pRendererCom->Add_Debug(m_pHitColliderCom);
	// 공격 콜라이더
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 2; ++i)
			m_pRendererCom->Add_Debug(m_pAtkColliderCom[i]);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CNobody::Render()
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

HRESULT CNobody::Render_Shadow()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	_uint iPass = 14;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Render(m_pShaderCom, iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CNobody::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 애니메이션 테스트
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// 월드행렬 테스트
	pImGui_Manager->SetGameObjectPosition(string("NO Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("NO Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("NO Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// 쉐이더 테스트
	pImGui_Manager->SetGameObjectShaderPass(string("NO Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// 상태 체크
	pImGui_Manager->CheckMonsterState(string("NO State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	// Dead 처리
	if (ImGui::Button("DEAD"))
	{
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(m_pPlayerTransform);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 12;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 13;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	ImGui::End();
}

HRESULT CNobody::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 월드행렬 저장
	_float4x4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, m_pTransformCom->Get_WorldMatrix());

	DWORD dwBytes = 0;
	WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CNobody::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// 세이브데이터에서 월드행렬을 로드
	DWORD		dwBytes = 0;
	_float4x4	worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// 월드행렬 세팅
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

_bool CNobody::Damaged(CCollider * pAtkCollider, _float fDamaged)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true)
		return false;

	// pAtkCollider 의 부모를 얻어와서 특정 기능을 실행시킬 수 있음
	CGameObject* pDestObject = pAtkCollider->Get_Parent();

	// 데미지 받음
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// 어느 방향에서 데미지 받았는지 계산
	CTransform* pDestTransfrom = pDestObject->Get_Transform();

	// 죽지 않았고, m_OnDamagedAnimTimeAcc 이 5.0 이상이면 OnDamaged 상태로 무조건 가도록
	if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 5.0)
	{
		// 공격 중이거나 이미 OnDamaged 상태이면 패스
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_OnDamaged"))
			return false;

		// 공격 쿨타임 3초로 초기화
		m_pMonsterCoolerCom->Set_Cool(CMonsterCooler::COOLTYPE_ATK, 3.0);

		// 피격 방향 계산 (앞뒤)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 10;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 10;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

		// 일정 시간 동안 OnDamaged 상태 되지 않도록 쿨타임 초기화
		m_OnDamagedAnimTimeAcc = 0.0;
	}
	// 죽었다면 Dead 상태로 무조건 가도록
	else if(m_bReadyToDie == true)
	{
		// 피격 방향 계산 (앞뒤)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_4Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 12;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 13;
		else if (DamagedDir == CTransform::DAMAGED_LEFT)
		{
			m_pTransformCom->LookAtXZ(pDestTransfrom);
			iAnimIndex = 12;
		}
		else
		{
			m_pTransformCom->LookAtXZ(pDestTransfrom);
			iAnimIndex = 12;
		}

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CNobody::Ready_Collider_Info()
{
	// 피격 콜라이더
	m_HitCollider_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("center");
	m_HitCollider_SocketPivotmMatrix = m_pModelCom->Get_TransformMatrix();

	// 공격 콜라이더
	const char* AtkBones[2] = { "center", "kama2" };
	for (_uint i = 0; i < 2; ++i)
	{
		m_AtkCollider_BoneMatrixPtr[i]      = m_pModelCom->Get_BoneMatrixPtr(AtkBones[i]);
		m_AtkCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}
}

void CNobody::Collider_Transform_Update()
{
	// 피격 콜라이더
	m_HitCollider_TransformMatrix =
		XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr.pCombinedMatrix)
		* XMLoadFloat4x4(&m_HitCollider_SocketPivotmMatrix)
		* m_pTransformCom->Get_WorldMatrix();

	// 공격 콜라이더
	for (_uint i = 0; i < 2; ++i)
	{
		m_AtkCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_AtkCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();
	}
}

HRESULT CNobody::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 100.f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel   = 1;
	StatusDesc.fEXP     = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_MonsterCooler */
	CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
	MonsterCoolerDesc.MaxAttackCool  = 5.0;
	MonsterCoolerDesc.MaxRunAwayCool = 8.0;
	MonsterCoolerDesc.RunAwayCool    = 8.0;		

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	{
		CCollider::COLLIDERDESC ColliderDesc;

		// 피격 콜라이더
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(2.f, 2.f, 2.f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Hit"), (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
			return E_FAIL;
		
		// 공격 콜라이더
		ColliderDesc.vScale       = _float3(3.f, 3.f, 3.f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 1.f, 0.f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_center"), (CComponent**)&m_pAtkColliderCom[0], &ColliderDesc)))
			return E_FAIL;

		ColliderDesc.vScale       = _float3(2.5f, 2.5f, 2.5f);
		ColliderDesc.vTranslation = _float4(0.5f, 0.55f, 0.5f, 1.f); 
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_kama2"), (CComponent**)&m_pAtkColliderCom[1], &ColliderDesc)))
			return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Dissolve_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Navigation */
	CNavigation::NAVIDESC tNaviDesc;
	tNaviDesc.iCurrentIndex = 0;
	tNaviDesc.pParent = m_pTransformCom;

	if (LEVEL_TEST == GM->Get_CurrentLevel())
	{
		if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Navigation_Rapunzel"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Nobody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNobody::SetUp_RootNodeIndex()
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

HRESULT CNobody::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Rmv_OnceList(1);
	m_pModelCom->Rmv_OnceList(3);
	m_pModelCom->Rmv_OnceList(6);

	for (_uint i = 0; i < 10; ++i)
		m_pModelCom->Add_LerpList(i);

	return S_OK;
}

HRESULT CNobody::SetUp_ConstantTable()
{
	// 림라이트
	if (m_iPass == 12)
	{
		if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &m_bBlur, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &m_bIsRim, sizeof(_bool))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRimRange", &m_fRimRange, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
			return E_FAIL;

		XMStoreFloat4(&m_vCamPosition, CGameInstance::GetInstance()->Get_CamPosition());
		if (FAILED(m_pShaderCom->Set_RawValue("g_vWorldCamPos", &m_vCamPosition, sizeof(_float4))))
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

	if (m_iPass == 4)
	{
		// 디졸브 텍스처 바인딩
		if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 99)))
			return E_FAIL;

		// 디졸브 Alpha 바인딩
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAmount, sizeof(_float))))
			return E_FAIL;
	}

	return S_OK;
}

void CNobody::OnCollision(_double TimeDelta)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true)
		return;

	// 몬스터끼리 밀어내기
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		if (pDestCollider == m_pHitColliderCom)
			continue;

		if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom, pDestCollider))
		{
			CGameObject* pDestObject = pDestCollider->Get_Parent();
			CTransform* pDestTransform = pDestObject->Get_Transform();
			CCollider* pDestCollider = pDestObject->Get_HitCollider();

			// 반지름
			_float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;
			_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;

			// 중점
			XMFLOAT3 sourcenterXM3 = m_pHitColliderCom->Get_Sphere()->Center;
			XMFLOAT3 destcenterXM3 = pDestCollider->Get_Sphere()->Center;
			_float4 sourcenterFloat4 = _float4(sourcenterXM3.x, sourcenterXM3.y, sourcenterXM3.z, 1.f);
			_float4 destcenterFloat4 = _float4(destcenterXM3.x, destcenterXM3.y, destcenterXM3.z, 1.f);
			_vector vSourCenter = XMLoadFloat4(&sourcenterFloat4);
			_vector vDestCenter = XMLoadFloat4(&destcenterFloat4);


			// 겹친부분 크기
			_float fCollisionSize = fDestColliderRadius + fSourColliderRadius - DISTANCE(vDestCenter - vSourCenter);

			// 방향계산
			_vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

			// 겹치부분만큼 밀려나기
			_vector vPos = m_pTransformCom->Get_Position();
			m_pTransformCom->Set_Position(vPos - vDir * fCollisionSize);
		}
	}

	// 플레이어 밀어내기
	for (auto& pDestCollider : *pColliderList)
	{
		if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom, pDestCollider))
		{
			CGameObject* pDestObject = pDestCollider->Get_Parent();
			CTransform* pDestTransform = pDestObject->Get_Transform();
			CCollider* pDestCollider = pDestObject->Get_HitCollider();

			// 반지름
			_float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;
			_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;

			// 중점
			XMFLOAT3 sourcenterXM3 = m_pHitColliderCom->Get_Sphere()->Center;
			XMFLOAT3 destcenterXM3 = pDestCollider->Get_Sphere()->Center;
			_float4 sourcenterFloat4 = _float4(sourcenterXM3.x, sourcenterXM3.y, sourcenterXM3.z, 1.f);
			_float4 destcenterFloat4 = _float4(destcenterXM3.x, destcenterXM3.y, destcenterXM3.z, 1.f);
			_vector vSourCenter = XMLoadFloat4(&sourcenterFloat4);
			_vector vDestCenter = XMLoadFloat4(&destcenterFloat4);


			// 겹친부분 크기
			_float fCollisionSize = fDestColliderRadius + fSourColliderRadius - DISTANCE(vDestCenter - vSourCenter);

			// 방향계산
			_vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

			// 겹치부분만큼 밀려나기
			_vector vPos = pDestTransform->Get_Position();
			pDestTransform->Set_Position(vPos + vDir * fCollisionSize);
		}
	}
}

void CNobody::ChangeState(_double TimeDelta)
{
	// 현재 레벨에 있는 Player 와 Player 의 Transform 을 가져옴
	if (m_pPlayer == nullptr)
	{
		m_pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);

		if (m_pPlayer == nullptr)
			return;

		m_pPlayerTransform = m_pPlayer->Get_Transform();

		if (m_pPlayerTransform == nullptr)
			return;
	}

	// ==========================================================================================

	// 0. 스폰
	if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
	{
		// Spawn 상태 생성
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
		m_bSpawn = true;

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		return;
	}


	// 1. 도망
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 7.f)
	{
		if (m_pStateMachineCom->FindMonsterState() == false)
		{
			// 쿨타임이 되는 경우에만 RunAway 상태 생성하고 쿨타임 초기화
			if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_RUNAWAY) == true)
			{
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create());
				m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_RUNAWAY);
				return;
			}
		}
	}


	// 2. 공격 
	if (m_pStateMachineCom->FindMonsterState() == false)
	{
		// 쿨타임이 되는 경우에만 Atk 상태 생성하고 쿨타임 초기화
		if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_ATK) == true)
		{
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_MonsterAtk::Create());
			m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_ATK);
			return;
		}
	}
}

CNobody* CNobody::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNobody* pInstance = new CNobody(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNobody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNobody::Clone(void* pArg)
{
	CNobody* pInstance = new CNobody(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNobody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNobody::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pMonsterCoolerCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pHitColliderCom);
	for (_uint i = 0; i < 2; ++i)
		Safe_Release(m_pAtkColliderCom[i]);
}
