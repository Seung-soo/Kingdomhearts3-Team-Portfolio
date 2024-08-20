#include "stdafx.h"
#include "Goat.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"
#include "Player.h"
#include "GoatWeapon.h"
#include "Camera_Perspective.h"
#include "MotionTrail.h"
#include "Progress_Manager.h"

// State
#include "State_GoatAtk.h"
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h"
#include "State_MonsterDynamicMove.h"
#include "State_MonsterRunAway.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"

CGoat::CGoat(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CGoat::CGoat(const CGoat& rhs)
	: CMonster(rhs)
{
}

HRESULT CGoat::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoat::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 17.5f;
	TransformDesc.fRotationPerSec = 5.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 몬스터 타입 설정
	m_MonsterType = MONSTERTYPE_GOAT;

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
	m_iPass = 0;

	// 시작 위치 설정
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	m_bIsBoss = true;

	m_bImGuiController = true;


	return S_OK;
}

HRESULT CGoat::NativeConstruct_Pool(void* pArg)
{
	GM->Add_Monster(this);

	GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_GOAT_APPEAR, m_pTransformCom);


	return S_OK;
}

_int CGoat::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	m_pTransformCom->Get_PositionFloat4_Pointer();

	// 죽으면 RESULT_DEAD 반환 (오브젝트 매니저에서 삭제)
	if (true == m_bDead)
	{
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_BossTarget"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_Cnt"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_CntBack"));

		return RESULT_DEAD;
	}

	// 상태 변경 처리
	ChangeState(TimeDelta);

	// 상태머신 Action 실행
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// 넉백 처리
	m_pTransformCom->Set_KnockBackInputPower(2.f);
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta * 0.1, m_pNavigationCom);

	// 텔레포트 업데이트
	TeleportDissolveUpdate(TimeDelta);

	// 피격카운트 초기화 (10초)
	if (m_iDamagedCountAcc != 0)
	{
		m_DamagedTimeAcc += TimeDelta;

		if (m_DamagedTimeAcc >= 10.0)
		{
			m_iDamagedCountAcc = 0;
			m_DamagedTimeAcc   = 0.0;
		}
	}

	// 공격 쿨타임 초기화
	if (m_bCanAttack == false)
	{
		m_AttackCool += TimeDelta;

		if (m_AttackCool >= m_MaxAttackCool)
		{
			m_bCanAttack = true;
			m_AttackCool = 0.0;
		}
	}

	// 모션트레일 인터벌 시간 증가
	m_MotionTrailInterval += TimeDelta;

	// 네비게이션보다 아래로 내려가지 않게 하기
	_vector vPos = m_pTransformCom->Get_Position();
	_float fMinY = m_pNavigationCom->Compute_Y(vPos);
	if (vecy(vPos) < fMinY)
		m_pTransformCom->Set_Position(XMVectorSetY(vPos, fMinY));

	// 모델 업데이트
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * 1.2, m_pTransformCom);
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * 1.2, m_pTransformCom, m_pNavigationCom);

	// 넉백 업데이트
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom);

	// 콜라이더의 위치 계산
	Collider_Transform_Update();

	// 피격 콜라이더 위치 업데이트
	m_pHitColliderCom->Update_Transform(m_HitCollider_TransformMatrix);

	// 콜리전매니저에 콜라이더 등록
	m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	
	return RESULT_NOPROBLEM;
}

_int CGoat::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 플레이어를 제외한 다른 객체와의 충돌 검사
	OnCollision(TimeDelta);

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// 콜라이더 디버그 렌더링
	// 피격 콜라이더
	if (m_bReadyToDie == false && m_bDead == false)
	{
		m_pRendererCom->Add_Debug(m_pHitColliderCom);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CGoat::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// 쉐이더 전역 상수에 값 전달
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* 모션 트레일 사용할 때 m_bUseMotionTrail 변수를 true로 끌 때 false로*/
	// 모션트레일에 쓰이는 본매트릭스 정보 받을 컨테이너
	vector<CModel::MESHDESC*> MeshDescVector;

	// 모델이 갖고있는 머테리얼 수 가져와서 쉐이더의 텍스처 전역 상수에 값 전달하고 렌더링
	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
		if (true == m_bUseMotionTrail)
			m_pModelCom->Render_Heavy(m_pShaderCom, m_iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render_Heavy(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	// 모션 트레일 객체 생성
	if (true == m_bUseMotionTrail)
	{
		if (m_MotionTrailInterval >= 0.015)
		{
			CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
			tMotionTrailDesc.iSize = 450;	// 본매트릭스갯수 (고정)
			tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// 본매트릭스 정보들
			tMotionTrailDesc.vColor = _float4(0.5f, 0.4f, 0.4f, 0.3f);	// 원하는 모션 트레일 색깔
			tMotionTrailDesc.fRimPower = 0.1f;	// 림라이트 파워 (작을수록 두꺼움)
			if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"), TEXT("Prototype_GameObject_MotionTrail"), &tMotionTrailDesc)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
			m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"))->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
			MeshDescVector.clear();

			m_MotionTrailInterval = 0.0;
		}
	}

	m_pTransformCom->Set_OldWorldMatrixFloat4x4(m_pTransformCom->Get_WorldFloat4x4());

	return S_OK;
}

HRESULT CGoat::Render_Shadow()
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

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Render_Heavy(m_pShaderCom, 14, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CGoat::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 애니메이션 테스트
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// 월드행렬 테스트
	pImGui_Manager->SetGameObjectPosition(string("GO Pos"), m_pTransformCom);
	static _bool bPosZero = false;
	ImGui::Checkbox("Position Zero", &bPosZero);
	if (bPosZero)
		m_pTransformCom->Set_Position(__vector(0.f, 0.f, 0.f, 1.f));

	pImGui_Manager->SmallSeparator();

	// 쉐이더 테스트
	pImGui_Manager->SetGameObjectShaderPass(string("GO Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// 상태 체크
	ImGui::Text("AnimIndex : %d", m_pModelCom->Get_CurrAnimIndex());
	pImGui_Manager->CheckMonsterState(string("GO State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	// 공격 테스트
	const char* Skills[GOAT_ATK_END] = { "Spear", "(Teleport)Spear", "(Teleport)ConsecutiveSpear", "FlySwing", "(Teleport)FlySwing", "DashSwing", "(Teleport)DashSwing", "SpinDashSwing", "(Teleport)SpinDashSwing", "JumpDashSwing", "UnderSwing", "(Teleport)UnderSwing", "LongUnderAtk", "(Teleport)LongUnderAtk", "ConsecutiveSwing", "MagicShot", "ShortPillarOfFire", "GravityPillarOfFire", "PillarOfFire"};
	_int iAtkSelect = -1;
	ImGui::Combo("Select Atk", &iAtkSelect, Skills, IM_ARRAYSIZE(Skills));
	if (iAtkSelect != -1)
	{
		m_iAnimPlayIndex = 0;
		m_eGoatAtk[0] = (GOAT_ATK)iAtkSelect;

		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) != string("State_Atk"))
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[0]), true);
	}
	pImGui_Manager->SmallSeparator();

	// 기타 상태 테스트
	if (ImGui::Button("Spawn"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create(), true);
	ImGui::SameLine();
	if (ImGui::Button("RunAway"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create(), true);
	ImGui::SameLine();
	if (ImGui::Button("DynamicMove"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_DynamicMove"), CState_MonsterDynamicMove::Create(), true);
	pImGui_Manager->SmallSeparator();

	// 상태제거
	if (ImGui::Button("Remove State"))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
	ImGui::SameLine();

	// Idle 상태 제거
	if (ImGui::Button("Remove Idle"))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_IDLE);
	ImGui::SameLine();
	// Idle 상태 생성
	if (ImGui::Button("Create Idle"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());
	pImGui_Manager->SmallSeparator();

	// 플레이어와의 거리
	ImGui::Text("Distance From Player : %.1f", m_pTransformCom->Get_Distance(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0)->Get_Transform()));

	// OnlyIdle 상태로 고정
	if (ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);

	pImGui_Manager->SmallSeparator();

	// Look 방향
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_Look());
	ImGui::Text("Look Direction : (%.1f, %.1f, %.1f)", vecx(vLook), vecy(vLook), vecz(vLook));

	// 현재 체력
	ImGui::Text("HP : %.0f / %.0f (%.0f %%)", m_pStatusCom->Get_HP(), m_pStatusCom->Get_MaxHP(), m_pStatusCom->Get_HP_Ratio() * 100.f);

	// 피격 횟수
	ImGui::Text("DamagedCountAcc : %d", m_iDamagedCountAcc);
	pImGui_Manager->SmallSeparator();

	// 페이즈 관리
	ImGui::Text("Phase : %d", m_iCurrentPhase);
	if (ImGui::Button("Phase 0"))
	{
		m_bPhaseChanged[0] = false;
		m_bPhaseChanged[1] = false;
		m_iCurrentPhase    = 0;
		m_pStatusCom->Set_HP(m_pStatusCom->Get_MaxHP());
	}
	ImGui::SameLine();
	if (ImGui::Button("Phase 1"))
	{
		m_bPhaseChanged[0] = false;
		m_bPhaseChanged[1] = false;
		m_pStatusCom->Set_HP(m_pStatusCom->Get_MaxHP() * 0.75f);
	}
	ImGui::SameLine();
	if (ImGui::Button("Phase 2"))
	{
		m_bPhaseChanged[0] = false;
		m_bPhaseChanged[1] = false;
		m_pStatusCom->Set_HP(m_pStatusCom->Get_MaxHP() * 0.5f);
	}
	pImGui_Manager->SmallSeparator();

	// Dead 처리
	pImGui_Manager->ColoredButtonBegin(200, 50, 0);
	if (ImGui::Button("DEAD"))
	{
		m_bReadyToDie = true;
		_uint iAnimIndex = 15;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}
	pImGui_Manager->ColoredButtonEnd();

	ImGui::End();
}

HRESULT CGoat::Save_ObjectData(HANDLE hFile)
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

HRESULT CGoat::Load_ObjectData(HANDLE hFile)
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

_bool CGoat::Damaged(CCollider* pAtkCollider, _float fDamaged)
{
	// 죽기 직전 상태이거나 죽은 상태라면 충돌검사 패스
	if (m_bReadyToDie == true || m_bDead == true)
		return false;

	// 피격 카운트 증가
	++m_iDamagedCountAcc;

	// pAtkCollider 의 부모(플에이어 무기 객체)를 얻어와서 특정 기능을 실행시킬 수 있음
	CGameObject* pDestObject = pAtkCollider->Get_Parent();

	// 데미지 받음
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// 어느 방향에서 데미지 받았는지 계산
	CTransform* pDestTransfrom = pDestObject->Get_Transform();

	// 죽지 않으면 OnDamaged 로
	if (m_bReadyToDie == false)
	{
		// 공격 중이거나 넉백상태이거나 도망 중이거나 이미 OnDamaged 상태이면 패스
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_RunAway"))
			return false;

		// 피격 방향 계산 (앞뒤)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		// 피격 방향에 맞는 애니메이션 실행
		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 14;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 15;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);
	}
	// 죽었다면 Dead 상태로 무조건 가도록
	else if (m_bReadyToDie == true)
	{
		_uint iAnimIndex = 15;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CGoat::CreateWeapon()
{
	// 무기생성
	if (m_pWeapon == nullptr)
	{
		m_pWeapon = static_cast<CGoatWeapon*>(m_pGameInstance->Add_GameObject_And_Return(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_GoatWeapon"), TEXT("Prototype_GameObject_GoatWeapon")));
	}

	if (m_pWeapon == nullptr)
	{
		BREAKPOINT;
	}

	m_pWeapon->Set_IsAttacking(false);
}

void CGoat::SetWeaponAttackTime(CModel* pModel, _uint iAnimIndex, _double startTime, _double endTime)
{
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= startTime)
		SetIsWeaponAttacking(true);
	if (pModel->Get_TargetTimeAcc(iAnimIndex) >= endTime)
		SetIsWeaponAttacking(false);
}

void CGoat::SetIsWeaponAttacking(_bool bOption)
{
	m_pWeapon->Set_IsAttacking(bOption);
}

void CGoat::StartTeleportDissolve()
{
	m_bDissolve   = true;
	m_iPass       = 7;
	m_fDistortion = 1.f;
}

void CGoat::Ready_Collider_Info()
{
	m_HitCollider_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("center");
	m_HitCollider_SocketPivotmMatrix = m_pModelCom->Get_TransformMatrix();
}

void CGoat::Collider_Transform_Update()
{
	m_HitCollider_TransformMatrix =
		XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr.pCombinedMatrix)
		* XMLoadFloat4x4(&m_HitCollider_SocketPivotmMatrix)
		* m_pTransformCom->Get_WorldMatrix();
}

HRESULT CGoat::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 500.f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

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
		if (FAILED(__super::Add_Component(LEVEL_FINALBOSS, TEXT("Prototype_Component_Navigation_FinalBoss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Hit"), (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* Dissolve_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FINALBOSS, TEXT("Prototype_Component_Model_Goat"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CGoat::SetUp_RootNodeIndex()
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

HRESULT CGoat::SetUp_Animations()
{
	// 일단 모든 애니메이션을 전부 다 ONCE 타입으로 만듦
	m_pModelCom->Add_OnceListAll();

	// LOOP 타입은 여기서 설정
	m_pModelCom->Rmv_OnceList(0);
	m_pModelCom->Rmv_OnceList(1);
	m_pModelCom->Rmv_OnceList(9);
	m_pModelCom->Rmv_OnceList(12);
	m_pModelCom->Rmv_OnceList(23);
	m_pModelCom->Rmv_OnceList(27);
	m_pModelCom->Rmv_OnceList(30);
	m_pModelCom->Rmv_OnceList(33);

	m_pModelCom->Add_LerpListAll();

	m_pModelCom->Rmv_LerpList(14);
	m_pModelCom->Rmv_LerpList(15);

	m_pModelCom->Set_TargetPlaySpeed_Rate(14, 0.7);
	m_pModelCom->Set_TargetPlaySpeed_Rate(15, 0.7);

	return S_OK;
}

HRESULT CGoat::SetUp_ConstantTable()
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

	// 왜곡 바인딩
	if (FAILED(m_pShaderCom->Set_RawValue("g_fModelDistortion", &m_fDistortion, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	// 모션블러
	if (FAILED(m_pTransformCom->Bind_OnOldShader(m_pShaderCom, "g_OldWorldMatrix")))
		return E_FAIL;

	if (m_iPass == 7)
	{
		// 디졸브 텍스처 바인딩
		if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 4)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		
		// 디졸브 Alpha 바인딩
		if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAlpha, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor1", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor2", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(m_pShaderCom->Set_RawValue("g_vDissolveColor3", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

void CGoat::OnCollision(_double TimeDelta)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true || m_bDead == true)
		return;
}

void CGoat::ChangeState(_double TimeDelta)
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

	// 디버깅용 (Idle 상태로 고정)
	if (m_bOnlyIdle == true)
		return;

	// 스폰
	if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
	{
		// Spawn 상태 생성
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
		m_bSpawn = true;

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		return;
	}

	// ==========================================================================================

	// 회피 또는 다이나믹무브 (일정시간 내에 일정 횟수 피격되면 회피 또는 다이나믹무브 발동하고 초기화)
	if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) != string("State_Dead") && m_iDamagedCountAcc >= 5)
	{
		if (rand() % 2 == 0)
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create(), true);
		else
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_DynamicMove"), CState_MonsterDynamicMove::Create(), true);

		m_iDamagedCountAcc = 0;
		m_DamagedTimeAcc   = 0.0;

		// 공격 가능 상태로 만들기
		m_bCanAttack = true;
		m_AttackCool = 0.0;
	}

	// 페이즈 변경 처리 (페이즈 넘어갈 때 특정 스킬 실행)
	if (m_pStatusCom->Get_HP_Ratio() <= 0.75f && m_bPhaseChanged[0] == false)
	{
		// 두 번째 페이즈로
		m_iCurrentPhase = 1;
		m_bPhaseChanged[0] = true;

		// 공격 쿨타임 감소
		m_MaxAttackCool = 4.0;

		// 중앙으로 이동
		m_pTransformCom->Set_Position(__vector(0.f, 10.f, 0.f, 1.f));

		// 끌어당기는 불기둥 스킬 실행
		m_eGoatAtk[0]     = GOAT_ATK_GRAVITYPILLAROFFIRE;
		m_ePrevGoatAtk[0] = GOAT_ATK_GRAVITYPILLAROFFIRE;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[0]), true);

		// 플레이어 위치 이동
		m_pPlayerTransform->Set_Position(__vector(20.f, 10.f, 20.f, 1.f));
		static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")))->Find_Cell(m_pPlayerTransform->Get_Position());

		m_pPlayerTransform->LookAtXZ(m_pTransformCom);

		// 공격 누적값 초기화
		m_iNormalAtkAcc  = 0;
		m_iSpecialAtkAcc = 0;

		// 카메라 액션 넣기 ================================================================
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_GOAT_PILLARFIRE, m_pTransformCom);
	}
	else if (m_pStatusCom->Get_HP_Ratio() <= 0.5f && m_bPhaseChanged[1] == false)
	{
		// 세 번째 페이즈로
		m_iCurrentPhase = 2;
		m_bPhaseChanged[1] = true;

		// 공격 쿨타임 감소
		m_MaxAttackCool = 3.5;

		// 중앙으로 이동
		m_pTransformCom->Set_Position(__vector(0.f, 10.f, 0.f, 1.f));

		// 공전하는 불기둥 스킬 실행
		m_eGoatAtk[0]     = GOAT_ATK_PILLAROFFIRE;
		m_ePrevGoatAtk[0] = GOAT_ATK_PILLAROFFIRE;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[0]), true);

		// 플레이어 위치 이동
		m_pPlayerTransform->Set_Position(__vector(0.01f, 10.f, 0.f, 1.f));
		static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")))->Find_Cell(m_pPlayerTransform->Get_Position());
		m_pPlayerTransform->LookAtXZ(m_pTransformCom);

		GM->Get_Player()->Attacked_By_Teleport();

		// 공격 누적값 초기화
		m_iNormalAtkAcc  = 0;
		m_iSpecialAtkAcc = 0;

		// 카메라 액션 넣기 ================================================================
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_GOAT_PILLARROT, m_pTransformCom);
	}

	// 공격 (0페이즈, 1페이즈, 2페이즈 별로 다르게)
	// 이미 다른 상태가 있으면 건너뛰기, m_bCanAttack 이 true 일때만 공격 가능
	_uint iAtkType = 0;
	if (m_pStateMachineCom->FindMonsterState() == false && m_bCanAttack == true)
	{
		if (m_iCurrentPhase == 0) // 0 페이즈 ========================================================
		{
			// 노말 공격
			// FlySwing, DashSwing, SpinDashSwing, JumpDashSwing, UnderSwing
			vector<GOAT_ATK> eNormalAtks { GOAT_ATK_FLYSWING, GOAT_ATK_DASHSWING, GOAT_ATK_SPINDASHSWING, GOAT_ATK_JUMPDASHSWING, GOAT_ATK_UNDERSWING };

			// 스페셜공격
			// TeleportDashSwing, TeleportSpinDashSwing, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks { GOAT_ATK_TELEPORTDASHSWING, GOAT_ATK_TELEPORTSPINDASHSWING, GOAT_ATK_SHORTPILLAROFFIRE };

			// 연속공격
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks { GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// 스페셜공격을 두번 했다면 랜덤으로 연속공격 하나를 고름
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// 노말공격 누적횟수가 3회라면 랜덤으로 스페셜 공격을 고르고 노말공격 누적횟수 초기화
			else if (m_iNormalAtkAcc >= 3)
			{
				if (m_ePrevGoatAtk[1] != GOAT_ATK_SHORTPILLAROFFIRE)
				{
					m_eGoatAtk[1] = GOAT_ATK_SHORTPILLAROFFIRE;
				}
				else
				{
					while (m_eGoatAtk[1] == m_ePrevGoatAtk[1])
						m_eGoatAtk[1] = eSpecialAtks[rand() % eSpecialAtks.size()];
				}

				m_iNormalAtkAcc = 0;

				// 스페셜공격 누적횟수 증가
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// 노말공격 누적횟수가 3회가 아니라면 랜덤으로 노말공격 하나를 고름
			else if (m_iNormalAtkAcc < 3)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				if (m_eGoatAtk[0] == GOAT_ATK_JUMPDASHSWING)
				{
					// JumpDashSwing 일 경우에 Goat 가 플레이어보다 낮은 위치가 아니라면 그냥 Spear 공격
					if (m_pTransformCom->IsUnderTarget(m_pPlayerTransform, 3.5f) == false)
						m_eGoatAtk[0] = GOAT_ATK_SPEAR;
				}

				// 노말공격 누적횟수 증가
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// 공격상태 바인딩
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[iAtkType]));
		}
		else if (m_iCurrentPhase == 1) // 1 페이즈 ===================================================
		{
			// 노말 공격
			// Spear, FlySwing, SpinDashSwing, UnderSwing
			vector<GOAT_ATK> eNormalAtks{ GOAT_ATK_SPEAR, GOAT_ATK_FLYSWING, GOAT_ATK_SPINDASHSWING, GOAT_ATK_UNDERSWING };

			// 스페셜공격
			// TeleportSpear, TeleportUnderSwing, MagicShot, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks{ GOAT_ATK_TELEPORTSPEAR, GOAT_ATK_TELEPORTUNDERSWING, GOAT_ATK_MAGICSHOT, GOAT_ATK_SHORTPILLAROFFIRE };

			// 연속공격
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks{ GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// 스페셜공격을 두번 했다면 랜덤으로 연속공격 하나를 고름
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// 노말공격 누적횟수가 2회라면 랜덤으로 스페셜 공격을 고르고 노말공격 누적횟수 초기화
			else if (m_iNormalAtkAcc >= 2)
			{
				if (m_ePrevGoatAtk[1] != GOAT_ATK_SHORTPILLAROFFIRE)
				{
					m_eGoatAtk[1] = GOAT_ATK_SHORTPILLAROFFIRE;
				}
				else
				{
					while (m_eGoatAtk[1] == m_ePrevGoatAtk[1])
						m_eGoatAtk[1] = eSpecialAtks[rand() % eSpecialAtks.size()];
				}

				m_iNormalAtkAcc = 0;

				// 스페셜공격 누적횟수 증가
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// 노말공격 누적횟수가 2회가 아니라면 랜덤으로 노말공격 하나를 고름
			else if (m_iNormalAtkAcc < 2)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				// 노말공격 누적횟수 증가
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// 공격상태 바인딩
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[iAtkType]));
		}
		else // 2 페이즈 =============================================================================
		{
			// 노말 공격
			// Spear, UnderSwing
			vector<GOAT_ATK> eNormalAtks{ GOAT_ATK_SPEAR, GOAT_ATK_SPINDASHSWING };

			// 스페셜공격
			// MagicShot, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks{ GOAT_ATK_MAGICSHOT, GOAT_ATK_SHORTPILLAROFFIRE };

			// 연속공격
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks{ GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// 스페셜공격을 두번 했다면 랜덤으로 연속공격 하나를 고름
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// 노말공격 누적횟수가 2회라면 랜덤으로 스페셜 공격을 고르고 노말공격 누적횟수 초기화
			else if (m_iNormalAtkAcc >= 2)
			{
				if (m_ePrevGoatAtk[1] != GOAT_ATK_SHORTPILLAROFFIRE)
				{
					m_eGoatAtk[1] = GOAT_ATK_SHORTPILLAROFFIRE;
				}
				else
				{
					while (m_eGoatAtk[1] == m_ePrevGoatAtk[1])
						m_eGoatAtk[1] = eSpecialAtks[rand() % eSpecialAtks.size()];
				}

				m_iNormalAtkAcc = 0;

				// 스페셜공격 누적횟수 증가
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// 노말공격 누적횟수가 2회가 아니라면 랜덤으로 노말공격 하나를 고름
			else if (m_iNormalAtkAcc < 2)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				// 노말공격 누적횟수 증가
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// 공격상태 바인딩
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[iAtkType]));
		}

		m_bCanAttack   = false;
		m_ePrevGoatAtk[iAtkType] = m_eGoatAtk[iAtkType];
	}
}

void CGoat::TeleportDissolveUpdate(_double TimeDelta)
{
	if (true == m_bDissolve)
	{
		if (true == m_bDissolvePlus) // 사라질 때
		{
			m_fDissolveAlpha += (_float)TimeDelta * 5.f;
			if (1.f < m_fDissolveAlpha)
			{
				m_fDissolveAlpha = 1.f;
				m_bDissolvePlus  = false;
				m_bDissolve      = false;
				m_iPass          = 0;
				m_fDistortion    = 0.f;

				// 전부 다 사라지면 디졸브 (역으로) 다시 시작하고 텔레포트하기
				StartTeleportDissolve();
				m_pTransformCom->TeleportActivate(m_pNavigationCom);

				// 텔레포트사운드
				if (rand() % 2 == 0)
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_58_Teleport.wav"), 0.6f, true, m_pTransformCom->Get_Position());
				else
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_62_Teleport.wav"), 0.6f, true, m_pTransformCom->Get_Position());

				// 플레이어를 쳐다보게 하기(특정 애니메이션은 xz로만 쳐다보게)
				if (m_pModelCom->Get_CurrAnimIndex() == 35 || m_pModelCom->Get_CurrAnimIndex() == 1)
					m_pTransformCom->LookAtXZ(m_pPlayerTransform);
				else
					m_pTransformCom->LookAt(m_pPlayerTransform);
			}
		}
		else // 다시 나타날 때
		{
			m_fDissolveAlpha -= (_float)TimeDelta * 5.f;
			if (0.f > m_fDissolveAlpha)
			{
				m_fDissolveAlpha = 0.f;
				m_bDissolvePlus  = true;
				m_bDissolve      = false;
				m_iPass          = 0;
				m_fDistortion    = 0.f;
			}
		}
	}
}

CGoat* CGoat::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CGoat* pInstance = new CGoat(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CGoat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CGoat::Clone(void* pArg)
{
	CGoat* pInstance = new CGoat(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CGoat");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGoat::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pHitColliderCom);
}