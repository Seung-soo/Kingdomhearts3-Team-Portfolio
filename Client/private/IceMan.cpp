#include "stdafx.h"
#include "IceMan.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Effect_Manager.h"
#include "Player.h"
#include "Camera_Perspective.h"
#include "MotionTrail.h"

// State
#include "State_IceManAtk.h"
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h"
#include "State_MonsterChase.h"
#include "State_MonsterRunAway.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"
#include "State_MonsterKnockBack.h"

CIceMan::CIceMan(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CIceMan::CIceMan(const CIceMan& rhs)
	: CMonster(rhs)
{
}

HRESULT CIceMan::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIceMan::NativeConstruct(void* pArg)
{
	// 부모 초기화할 때 트랜스폼 초기화
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 4.f;
	TransformDesc.fRotationPerSec = 1.5f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// 몬스터 타입 설정
	m_MonsterType = MONSTERTYPE_ICEMAN;

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
	// 상태머신(StateMachine) 클래스와 상태(State) 클래스 설명은 문서 참고
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

	//m_bImGuiController = true;

	m_bIsBoss = true;

	m_pModelCom->SetUp_AnimIndex(25);
	m_pModelCom->Update_CombinedTransformationMatrices(0.0, m_pTransformCom, m_pNavigationCom);


	return S_OK;
}

HRESULT CIceMan::NativeConstruct_Pool(void* pArg)
{
	// 게임 진행중에 몬스터들을 바로바로 복사생성하면 렉이 걸림
	// 그래서 Loader 에서 미리 몬스터들을 생성해서 오브젝트풀에 넣어둔 다음 필요할 때마다 꺼내오는 방식으로 게임을 진행하고 있음
	// 이 상황에서 NativeConstruct 함수는 Loader 에서 실행되기 때문에 게임을 진행하다가 오브젝트풀에서 꺼내올 때 실행시키고 싶은 초기화 함수는 여기 NativeConstruct_Pool 에 넣어두면 됨
	// IceMan 에는 딱히 쓸 일이 없어서 내용이 없음

	// 게임 매니저 몬스터 리스트에 추가 (게임매니저가 게임 내 모든 몬스터들을 따로 관리하고 있기 때문에 오브젝트풀에서 꺼내올 때 게임매니저에 등록)
	GM->Add_Monster(this);

	GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_ICEMAN_APPEAR, m_pTransformCom);

	return S_OK;
}

_int CIceMan::Tick(_double TimeDelta)
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

	// 피격 애니메이션 재생 쿨타임 처리
	// 피격 애니메이션을 매번 재생하면 이상하니까 피격 애니메이션 재생에 쿨타임을 주기 위해 만든 변수
	m_OnDamagedAnimTimeAcc += TimeDelta;

	// 상태 변경 처리
	// IceMan 의 상태들을 조건에 맞춰서 변경하는 함수 (ChangeState 함수 안에서 상태머신에 상태를 바인딩함)
	ChangeState(TimeDelta);

	// 상태머신 Action 실행
	// 상태머신이 갖고 있는 상태들을 매 Tick 마다 실행시킴
	// 몬스터들은 상태머신에서 STATETYPE_IDLE 과 STATETYPE_MONSTERSTATE 를 사용하고 있음
	// STATETYPE_IDLE 에 바인딩된 상태의 Action 이 먼저 돌고, STATETYPE_MONSTERSTATE 에 바인딩된 상태의 Action 이 나중에 돌기 때문에 STATETYPE_MONSTERSTATE 에 상태가 없다면 계속 IDLE 상태만 Action 됨
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// MonsterCooler 업데이트 돌려서 쿨타임 초기화
	// 몬스터들 공격에 랜덤성을 주기 위해서 각 상태(공격, 회피, 추적 등등) 마다 쿨타임을 주고 쿨타임이 다 되면 해당 상태를 실행시키게 해뒀는데
	// m_pMonsterCoolerCom 컴포넌트를 계속 업데이트 시켜줘야 쿨타임이 줄어듦 (이건 선택사항, 그냥 조건문 돌려서 상태 실행시켜도 됨)
	m_pMonsterCoolerCom->Update(TimeDelta);

	// 모델 업데이트(복붙)
	// 첫번째 조건에 있는 것은 디버깅용이기 때문에 신경쓰지 않아도 됨
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
	// 모델 업데이트할 때 m_pTransformCom 을 넣어줘야 애니메이션의 이동값과 객체의 이동값이 동기화 됨
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);

	// 콜라이더의 위치 계산
	Collider_Transform_Update();

	// 피격 콜라이더 위치 업데이트
	for (_uint i = 0; i < 4; ++i)
		m_pHitColliderCom[i]->Update_Transform(m_HitCollider_TransformMatrix[i]);
	// 공격 콜라이더 위치 업데이트
	for (_uint i = 0; i < 2; ++i)
		m_pAtkColliderCom[i]->Update_Transform(m_AtkCollider_TransformMatrix[i]);

	// 콜리전매니저에 콜라이더 등록
	// 콜리전매니저에 콜라이더를 등록시켜줘야 충돌체크가 가능
	// 피격 콜라이더는 Monster_Hit, 공격 콜라이더는 Mosnter_Atk 으로 등록해줘야 함
	// 피격 콜라이더는 죽기 직전 상태나 죽은 상태에서는 등록하지 않음
	if (m_bReadyToDie == false && m_bDead == false)
	{
		for (_uint i = 0; i < 4; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom[i]);
	}

	// 공격 콜라이더는 공격 상태(m_bIsAttacking)일 때만 등록
	// m_bIsAttacking 는 공격 상태로 들어갈 때 직접 켜주면 됨 (SetAttackTime 함수 사용)
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 2; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom[i]);
	}

	return RESULT_NOPROBLEM;
}

_int CIceMan::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// 플레이어를 제외한 다른 객체와의 충돌 검사
	// 플레이어 무기에 피격됐을 때의 검사는 Player_Weapon 클래스에서 몬스터의 Damaged 함수를 호출하면서 실행됨 (아래 Damaged 함수 참고)
	// 그렇기 때문에 OnCollision 함수에서는 플레이어를 제외한 다른 객체와의 충돌을 체크하면 됨
	OnCollision(TimeDelta);

	// 절두체 안에 있다면 + m_bDead 상태가 아니면 렌더그룹에 추가
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// 콜라이더 디버그 렌더링
	// 피격 콜라이더
	if (m_bReadyToDie == false && m_bDead == false)
	{
		for (_uint i = 0; i < 4; ++i)
			m_pRendererCom->Add_Debug(m_pHitColliderCom[i]);
	}		
	// 공격 콜라이더
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 2; ++i)
			m_pRendererCom->Add_Debug(m_pAtkColliderCom[i]);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CIceMan::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// 쉐이더 전역 상수에 값 전달
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

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
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	// 모션 트레일 객체 생성
	if (true == m_bUseMotionTrail)
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 280;	// 본매트릭스갯수 (고정)
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// 본매트릭스 정보들
		tMotionTrailDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);	// 원하는 모션 트레일 색깔
		tMotionTrailDesc.fRimPower = 1.f;	// 림라이트 파워 (작을수록 두꺼움)
		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"), TEXT("Prototype_GameObject_MotionTrail"), &tMotionTrailDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"))->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
		MeshDescVector.clear();
	}

	return S_OK;
}

HRESULT CIceMan::Render_Shadow()
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

void CIceMan::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// 애니메이션 테스트
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// 월드행렬 테스트
	pImGui_Manager->SetGameObjectPosition(string("IM Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("IM Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// 쉐이더 테스트
	pImGui_Manager->SetGameObjectShaderPass(string("IM Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// 상태 체크
	ImGui::Text("AnimIndex : %d", m_pModelCom->Get_CurrAnimIndex());
	pImGui_Manager->CheckMonsterState(string("IM State"), m_pStateMachineCom);

	// 공격테스트
	const char* Skills[ICEMAN_SKILL_END] = { "WalkSlash", "Clap", "LongSlash", "JumpDownwardBlow" };
	int Select = -1;
	ImGui::Combo("Select Atk", &Select, Skills, IM_ARRAYSIZE(Skills));
	if (Select != -1)
	{
		m_iAnimPlayIndex = 0;
		m_eIceManSkill = (ICEMAN_SKILL)Select;

		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) != string("State_Atk"))
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_IceManAtk::Create(&m_eIceManSkill), true);
	}
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
	ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle);
	ImGui::SameLine();

	if (ImGui::Button("CHASE"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Chase"), CState_MonsterChase::Create());
	ImGui::SameLine();

	if (ImGui::Button("RunAway"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create());
	pImGui_Manager->SmallSeparator();

	// Dead 처리
	pImGui_Manager->ColoredButtonBegin(200, 50, 0);
	if (ImGui::Button("DEAD"))
	{
		m_bReadyToDie = true;
		_uint iAnimIndex = 24;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}
	pImGui_Manager->ColoredButtonEnd();

	ImGui::End();
}

HRESULT CIceMan::Save_ObjectData(HANDLE hFile)
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

HRESULT CIceMan::Load_ObjectData(HANDLE hFile)
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

_bool CIceMan::Damaged(CCollider* pAtkCollider, _float fDamaged)
{
	// 플레이어 무기에 피격됐을 때 동작시킬 정보들은 여기 Damaged 함수에 작성
	CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_OnDamaged_7.wav"), 0.9f, true, m_pTransformCom->Get_Position());

	// 죽기 직전 상태이거나 죽은 상태라면 충돌검사 패스
	if (m_bReadyToDie == true || m_bDead == true)
		return false;

	// pAtkCollider 의 부모(플에이어 무기 객체)를 얻어와서 특정 기능을 실행시킬 수 있음
	CGameObject* pDestObject = pAtkCollider->Get_Parent();

	// 데미지 받음
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// 어느 방향에서 데미지 받았는지 계산
	CTransform* pDestTransfrom = pDestObject->Get_Transform();

	// 죽지 않았고, m_OnDamagedAnimTimeAcc 이 5.0 이상이면 OnDamaged 상태로 무조건 가도록
	if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 5.0)
	{
		// 공격 중이거나 넉백상태이거나 도망 중이거나 이미 OnDamaged 상태이면 패스
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_KnockBack") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_RunAway") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_OnDamaged"))
			return false;

		// 공격 쿨타임 0.5초로 초기화
		m_pMonsterCoolerCom->Set_Cool(CMonsterCooler::COOLTYPE_ATK, 0.5);

		// 피격 방향 계산 (앞뒤)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		// 피격 방향에 맞는 애니메이션 실행
		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 23;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 22;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

		// 일정 시간 동안 OnDamaged 상태 되지 않도록 쿨타임 초기화
		m_OnDamagedAnimTimeAcc = 0.0;
	}
	// 죽었다면 Dead 상태로 무조건 가도록
	else if (m_bReadyToDie == true)
	{
		_uint iAnimIndex = 24;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CIceMan::Ready_Collider_Info()
{
	// 참고로 콜라이더 위치랑 크기 설정하기 전에 몬스터의 크기먼저 제대로 설정해야함
	// 몬스터 크기 달라지면 콜라이더 위치랑 크기도 다시 설정해줘야 하기 때문에 무조건 몬스터 크기 먼저 제대로 잡기

	// 피격 콜라이더 (몸통, 얼굴, 왼쪽어깨, 오른쪽어깨)
	const char* HitBones[4] = { "center", "facial", "L_kata", "R_kata" };
	for (_uint i = 0; i < 4; ++i)
	{
		m_HitCollider_BoneMatrixPtr[i]      = m_pModelCom->Get_BoneMatrixPtr(HitBones[i]);
		m_HitCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}

	// 공격 콜라이더 (왼쪽다리, 오른쪽다리, 왼쪽손, 오른쪽손)
	const char* AtkBones[4] = { "L_yubiC1", "R_yubiC1" };
	for (_uint i = 0; i < 2; ++i)
	{
		m_AtkCollider_BoneMatrixPtr[i]      = m_pModelCom->Get_BoneMatrixPtr(AtkBones[i]);
		m_AtkCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}
}

void CIceMan::Collider_Transform_Update()
{
	// 피격 콜라이더
	for (_uint i = 0; i < 4; ++i)
	{
		m_HitCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_HitCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();
	}

	// 공격 콜라이더
	for (_uint i = 0; i < 2; ++i)
	{
		m_AtkCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_AtkCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();
	}
}

HRESULT CIceMan::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 200.f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_MonsterCooler */
	// 여기서 각 행동들의 쿨타임을 설정
	CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
	MonsterCoolerDesc.MaxAttackCool  = 2.0;
	MonsterCoolerDesc.AttackCool     = 7.0;
	MonsterCoolerDesc.MaxRunAwayCool = 5.5;
	MonsterCoolerDesc.RunAwayCool    = 8.0;
	MonsterCoolerDesc.MaxChaseCool   = 18.0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Navigation */
	CNavigation::NAVIDESC tNaviDesc;
	tNaviDesc.iCurrentIndex = 0;
	tNaviDesc.pParent       = m_pTransformCom;

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
		if (FAILED(__super::Add_Component(LEVEL_FROZEN, TEXT("Prototype_Component_Navigation_Frozen"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	
	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Collider */
	{
		// 콜라이더를 달기 전에 꼭 객체의 크기 먼저 설정해야함
		// 객체의 크기가 달라지면 나중에 콜라이더 크기 수정하기 번거롭기 때문에 미리 크기 설정 잘 해둘 것
		// 콜라이더를 달아야 할 뼈의 위치는 콜라이더 툴에서 확인 가능

		CCollider::COLLIDERDESC ColliderDesc;

		// 피격 콜라이더
		const _tchar* HitColliderTag[4] = { L"Com_Collider_Hit_center", L"Com_Collider_Hit_facial", L"Com_Collider_Hit_L_kata", L"Com_Collider_Hit_R_kata" };

		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(2.5f, 2.5f, 2.5f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

		for (_int i = 0; i < 4; ++i)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), HitColliderTag[i], (CComponent**)&m_pHitColliderCom[i], &ColliderDesc)))
				return E_FAIL;
		}

		// 공격 콜라이더
		const _tchar* AtkColliderTag[2] = { L"Com_Collider_Atk_L_yubiC1", L"Com_Collider_Atk_R_yubiC1" };

		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(3.5f, 3.5f, 3.5f); 
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.25f, -0.5f, 0.f, 1.f);

		for (_int i = 0; i < 2; ++i)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), AtkColliderTag[i], (CComponent**)&m_pAtkColliderCom[i], &ColliderDesc)))
				return E_FAIL;
		}
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

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_FROZEN, TEXT("Prototype_Component_Model_IceMan"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CIceMan::SetUp_RootNodeIndex()
{
	// IceMan 모델의 루트노드를 설정함 (이 게임의 모든 모델은 루트노드 이름이 갖기 때문에 이 함수는 복붙하면 됨)
	// 루트노드를 설정하는 이유 :	애니메이션이 이동하면서 원점에서 벗어나면 객체의 실제 위치도 그만큼 이동시켜줘야 하는데 매번 일일이 잡아주기 번거롭기 때문에
	//								루트노드를 설정해두고, 루트노드의 이동값만큼 객체를 이동하게 만들기 위해
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

	return S_OK;
}

HRESULT CIceMan::SetUp_Animations()
{
	// 애니메이션 타입은 두 종류로 나뉨. 하나는 ONCE, 나머지 하나는 LOOP
	// ONCE 타입 : 한 번 애니메이션이 완료되면 종료되는 함수
	// LOOP 타입 : 애니메이션이 종료돼도 계속 반복시킬 함수
	// Model 클래스 구조상 미리 타입을 다 잡아줘야하기 때문에 여기서 다 잡아줌

	// 일단 모든 애니메이션을 전부 다 ONCE 타입으로 만듦
	m_pModelCom->Add_OnceListAll();

	// LOOP 타입은 여기서 설정
	m_pModelCom->Rmv_OnceList(0);
	m_pModelCom->Rmv_OnceList(9);
	m_pModelCom->Rmv_OnceList(12);
	m_pModelCom->Rmv_OnceList(14);
	m_pModelCom->Rmv_OnceList(25);
	m_pModelCom->Rmv_OnceList(32);
	m_pModelCom->Rmv_OnceList(36);
	m_pModelCom->Rmv_OnceList(38);

	// 모든 애니메이션들을 보간되게 함 (이거 안 하면 보간 안 됨)
	for (_uint i = 0; i < 39; ++i)
		m_pModelCom->Add_LerpList(i);

	// 참고로 애니메이션 변경방법은 두 가지가 있는데 하나는 모델클래스에 있는 Set_TempIndex 로 변경하는 것
	// 나머지 하나는 Mosnter 클래스에 있는 Set_AnimAndReset 함수로 변경하는 것
	// LOOP 애니메이션은 Set_TempIndex 함수로 설정하면 되고, ONCE 애니메이션은 Set_AnimAndReset 함수로 설정하면 됨

	return S_OK;
}

HRESULT CIceMan::SetUp_ConstantTable()
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

void CIceMan::OnCollision(_double TimeDelta)
{
	// 죽기 직전 상태라면 충돌검사 패스
	if (m_bReadyToDie == true || m_bDead == true)
		return;

	// 벽이랑 충돌했을 때 실행시킬 함수 (벽이랑 충돌했는지는 State_IceManAtk 에서 실행되고있음)
	if (m_bCollisionWall)
	{
		CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom, 4.f);
		CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom, 4.f);

		CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("IceMan_FallDown_10.wav"), 0.7f, true, m_pTransformCom->Get_Position());

		// KnockBack 으로 상태변경 (마지막 인자를 true 로 했기 때문에 기존 상태를 제거하고 무조건 실행됨)
		_uint iAnimIndex = 24;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_KnockBack"), CState_MonsterKnockBack::Create(&iAnimIndex), true);

		// 다른 애니메이션 재생 도중 KnockBack 상태가 되면 Monster 클래스에 있는 PlayAnimations 함수의 재생인덱스가 초기화되지 않기 때문에 따로 초기화
		m_iAnimPlayIndex = 0;

		m_bCollisionWall = false;
		return;
	}

	// 고드름이랑 충돌검사 (점프 공격 중일때만)
	if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") && m_eSpecialSkill == ICEMAN_SKILL_JUMPDOWNWARDBLOW)
	{
		// 콜리전 매니저에 "Icicle" 이름으로 등록된 콜라이더들을 가져옴
		auto pColliderList = m_pGameInstance->Get_ColliderList(string("Icicle"));
		for (auto& pDestCollider : *pColliderList)
		{
			// 0 번 피격 콜라이더(center) 가 Icicle 과 충돌하면 (둘 다 Sphere 콜라이더)
			for (_uint i = 0; i < 4; ++i)
			{
				if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom[0], pDestCollider))
				{
					// 이펙트
					CEffect_Manager::GetInstance()->Create_HitStar(m_pTransformCom);

					// 10.f 만큼의 데미지 받음
					// 여기서 HP 가 0 아래로 떨어지면 m_bReadyToDie 가 true 가 됨
					m_pStatusCom->Damaged(10.f);

					// 죽지 않았다면 OnDamaged 로 상태 변경
					if (m_bReadyToDie == false)
					{
						_uint iAnimIndex = 27;
						m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

						// 공격 쿨타임 초기화
						m_pMonsterCoolerCom->Set_Cool(CMonsterCooler::COOLTYPE_ATK, 0.0);

						// 다른 애니메이션 재생 도중 OnDamaged 상태가 되면 Monster 클래스의 애니메이션 재생인덱스가 초기화되지 않기 때문에 따로 초기화
						m_iAnimPlayIndex = 0;

						// 일정 시간 동안 OnDamaged 상태 되지 않도록 쿨타임 초기화
						m_OnDamagedAnimTimeAcc = 0.0;
					}
					// 죽었다면 Dead 상태로 무조건 가도록
					else if (m_bReadyToDie == true)
					{
						_uint iAnimIndex = 24;
						m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
					}

					return;
				}
			}
		}
	}

	// 플레이어 밀어내기
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		for (_uint i = 0; i < 4; ++i)
		{
			if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom[i], pDestCollider))
			{
				CGameObject* pDestObject = pDestCollider->Get_Parent();
				CTransform* pDestTransform = pDestObject->Get_Transform();
				CCollider* pDestCollider = pDestObject->Get_HitCollider();

				// 반지름
				_float fSourColliderRadius = m_pHitColliderCom[i]->Get_Sphere()->Radius;
				_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;

				// 중점
				XMFLOAT3 sourcenterXM3 = m_pHitColliderCom[i]->Get_Sphere()->Center;
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
}

void CIceMan::ChangeState(_double TimeDelta)
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

	// 0. 스폰
	if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
	{
		// Spawn 상태 생성
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
		m_bSpawn = true;

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		return;
	}


	// 1. 추적
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) >= 20.f)
	{
		if (m_pStateMachineCom->FindMonsterState() == false)
		{
			// 쿨타임이 되는 경우에만 Chase 상태 생성하고 쿨타임 초기화
			if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_CHASE) == true)
			{
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Chase"), CState_MonsterChase::Create());
				m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_CHASE);
				return;
			}
		}
	}


	// 2. 도망
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 5.5f)
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


	// 3. 공격 
	if (m_pStateMachineCom->FindMonsterState() == false)
	{
		// 쿨타임이 되는 경우에만 Atk 상태 생성하고 쿨타임 초기화
		if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_ATK) == true)
		{
			if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 10.f)
			{
				m_eIceManSkill = (ICEMAN_SKILL)(rand() % 2);
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_IceManAtk::Create(&m_eIceManSkill));
			}
			else
			{
				if (m_ePrevSpecialSkill == ICEMAN_SKILL_JUMPDOWNWARDBLOW)
				{
					if (rand() % 10 < 8) // 80%
						m_eSpecialSkill = ICEMAN_SKILL_LONGSLASH;
					else // 20%
						m_eSpecialSkill = ICEMAN_SKILL_JUMPDOWNWARDBLOW;
				}
				else if (m_ePrevSpecialSkill == ICEMAN_SKILL_LONGSLASH)
				{
					if (rand() % 10 < 8) // 80%
						m_eSpecialSkill = ICEMAN_SKILL_JUMPDOWNWARDBLOW;
					else // 20%
						m_eSpecialSkill = ICEMAN_SKILL_LONGSLASH;
				}

				m_ePrevSpecialSkill = m_eSpecialSkill;
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_IceManAtk::Create(&m_eSpecialSkill));
			}

			m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_ATK);
			return;
		}
	}
}

CIceMan* CIceMan::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CIceMan* pInstance = new CIceMan(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CIceMan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CIceMan::Clone(void* pArg)
{
	CIceMan* pInstance = new CIceMan(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CIceMan");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CIceMan::Free()
{
	__super::Free();

	// 디졸브텍스처는 GameObject 에서 Safe_Release 해주고 있기 때문에 따로 Safe_Release 해줄 필요 없음

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pMonsterCoolerCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	for (_uint i = 0; i < 4; ++i)
		Safe_Release(m_pHitColliderCom[i]);
	for (_uint i = 0; i < 2; ++i)
		Safe_Release(m_pAtkColliderCom[i]);
}