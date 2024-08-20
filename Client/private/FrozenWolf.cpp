#include "stdafx.h"
#include "FrozenWolf.h"

#include "Player.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Camera_Perspective.h"
#include "MotionTrail.h"

// State
#include "State_MonsterIdle.h"
#include "State_MonsterChase.h"
#include "State_MonsterDynamicMove.h"
#include "State_MonsterSpawn.h"
#include "State_FrozenWolfAtk.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"


CFrozenWolf::CFrozenWolf(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CMonster(pDevice, pDeviceContext)
{
}

CFrozenWolf::CFrozenWolf(const CFrozenWolf& rhs)
    : CMonster(rhs)
{
}

HRESULT CFrozenWolf::NativeConstruct_Prototype()
{
    if(FAILED(__super::NativeConstruct_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFrozenWolf::NativeConstruct(void* pArg)
{
    //  ============================== Loader에서 실행 됨===========================================

    // 몬스터 타입 설정
    m_MonsterType = MONSTERTYPE_FROZENWOLF;
 
    CTransform::TRANSFORMDESC TransformDesc;
    TransformDesc.fSpeedPerSec = 4.f;
    TransformDesc.fRotationPerSec = 1.5f;
    TransformDesc.fScalingPerSec = 5.f;

    if (FAILED(__super::NativeConstruct(&TransformDesc)))
        return E_FAIL;

    if (FAILED(SetUp_Components()))
        return E_FAIL;

    if (FAILED(SetUp_RootNodeIndex()))
        return E_FAIL;

    // 애니메이션 설정
    if (FAILED(SetUp_Animations()))
        return E_FAIL;

    // 콜라이더 정보 초기화
   if (FAILED( Ready_Collider_Info()))
       return E_FAIL;

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
        m_pTransformCom->MakeSameDirection(__vector(-1.f, 0.f, 0.f, 0.f));
    }

    m_eWolfSkill = WOLF_DASH;

    //m_bImGuiController = true;

    // 스킬 패턴 정하기

    m_SkillSequence[0]  = WOLF_DASH;
    m_SkillSequence[1]  = WOLF_SCRATCH;
    m_SkillSequence[2]  = WOLF_SUMMON;
    m_SkillSequence[3]  = WOLF_SCRATCH;
    m_SkillSequence[4]  = WOLF_THROWFIRE;
    m_SkillSequence[5]  = WOLF_SPINATTACK;
    m_SkillSequence[6]  = WOLF_THROWFIRE;
    m_SkillSequence[7]  = WOLF_DASH;
    m_SkillSequence[8]  = WOLF_SCRATCH;
    m_SkillSequence[9]  = WOLF_PURPLERAIN;

	m_bIsBoss = true;

	if (LEVEL_KEYGRAVE != GM->Get_CurrentLevel())
	{
		// Effect_Trail
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		CEffect_Trail::TRAILDESC		tDesc;
		tDesc.pTargetTransform             = m_pTransformCom;
		tDesc.pBoneMatrixPtr               = m_pModelCom->Get_BoneMatrixPtr("C_mune_U3");
		tDesc.SocketPivotmMatrix           = m_pModelCom->Get_TransformMatrix();
		tDesc.iPassIndex                   = 19;
		tDesc.tTrailBuffDesc.iNumVerticies = 500;
		tDesc.tTrailBuffDesc.fLifeTime     = 0.4f;
		tDesc.tTrailBuffDesc.fLerpCnt      = 10.f;
		tDesc.tTrailBuffDesc.fDuration     = 0.f;
		tDesc.vAdjustValue                 = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
		tDesc.iTextureIndex                = 160;

		tDesc.vColor = _float4(0.2f, 0.f, 1.f, 0.f);

		tDesc.bBlur = true;

    m_fFrustumRange = 20.f;

		m_pTrail[0] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

		tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("L_ude2");
		m_pTrail[1] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

		tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_ude2");
		m_pTrail[2] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

		tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("tail12_D3");
		m_pTrail[3] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

		tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("tail17_B_all_spo");
		m_pTrail[4] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));


		//// 눈

		// Left

		//GM->Add_Monster(this);
		//GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_FROZENWOLF_APPEAR, m_pTransformCom);
	}



    return S_OK;
}

HRESULT CFrozenWolf::NativeConstruct_Pool(void* pArg)
{
    //  ============================== 실제 인게임에서 실행 됨 ========================================
    
    // 게임매니저가 게임 내 모든 몬스터들을 따로 관리하고 있기 때문에 오브젝트풀에서 꺼내올 때 게임매니저에 등록
    GM->Add_Monster(this);


	// Effect_Trail
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CEffect_Trail::TRAILDESC		tDesc;
	tDesc.pTargetTransform             = m_pTransformCom;
	tDesc.pBoneMatrixPtr               = m_pModelCom->Get_BoneMatrixPtr("C_mune_U3");
	tDesc.SocketPivotmMatrix           = m_pModelCom->Get_TransformMatrix();
	tDesc.iPassIndex                   = 19;
    tDesc.tTrailBuffDesc.iNumVerticies = 500;
    tDesc.tTrailBuffDesc.fLifeTime     = 0.4f;
    tDesc.tTrailBuffDesc.fLerpCnt      = 10.f;
    tDesc.tTrailBuffDesc.fDuration     = 0.f;
    tDesc.vAdjustValue                 = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
    tDesc.iTextureIndex                = 160;

	tDesc.vColor = _float4(0.2f, 0.f, 1.f, 0.f);

	tDesc.bBlur = true;


	m_pTrail[0] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("L_ude2");
	m_pTrail[1] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("R_ude2");
	m_pTrail[2] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("tail12_D3");
	m_pTrail[3] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	tDesc.pBoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("tail17_B_all_spo");
	m_pTrail[4] = static_cast<CEffect_Trail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Trail"), &tDesc));

	GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_FROZENWOLF_APPEAR, m_pTransformCom);
    
    return S_OK;
}

_int CFrozenWolf::Tick(_double TimeDelta)
{
    if (true == m_bFirstTick)
    {
        for (_uint i = 0; i < 5; ++i)
            m_pTrail[i]->Set_Active(false);
    }

    if(FAILED(__super::Tick(TimeDelta)))
        return RESULT_ERROR;

	if (true == m_bDead)
	{
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_BossTarget"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_Cnt"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_CntBack"));

        return RESULT_DEAD;
	}

    m_OnDamagedAnimTimeAcc += TimeDelta;
    m_TimeAccum_MotionTrail += TimeDelta;

    // CoolTime 누적
    m_CoolTime += TimeDelta;
    m_TimeAccum += TimeDelta;

    if(m_TimeAccum >= 5.0)
        m_isWaiting = false;

    if(FAILED(ChangeState(TimeDelta)))
        return RESULT_ERROR;

	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);


    if(true == m_GravitySwitch)
        Gravity(TimeDelta);

    if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
    {
        BREAKPOINT;
        return RESULT_ERROR;
    }

    // MonsterCooler 업데이트 돌려서 쿨타임 초기화
    m_pMonsterCoolerCom->Update(TimeDelta);


    if(FAILED(Collider_Transform_Update()))
        return RESULT_ERROR;

    if (m_bReadyToDie == false && m_bDead == false)
    {
        for (_uint i = 0; i < 3; ++i)
            m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom[i]);
    }

    // 공격 콜라이더는 공격 상태(m_bIsAttacking)일 때만 등록
    // m_bIsAttacking 는 공격 상태로 들어갈 때 직접 켜주면 됨 (SetAttackTime 함수 사용)
    if (m_bIsAttacking == true)
    {
        for (_uint i = 0; i < 3; ++i)
            m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom[i]);
    }

    _float fY = vecy(m_pTransformCom->Get_Position());

    if(fY < 0.f)
        m_pTransformCom->Set_PositionY(0.f);

	m_fMoveUV += (_float)TimeDelta * m_fMoveUVSpeed;
	m_fNoiseX += (_float)TimeDelta * m_fNoiseXSpeed;
	m_fNoiseY += (_float)TimeDelta * m_fNoiseYSpeed;

    return _int();
}

_int CFrozenWolf::LateTick(_double TimeDelta)
{
    if (FAILED(__super::LateTick(TimeDelta)))
        return RESULT_ERROR;

    if(FAILED(OnCollision(TimeDelta)))
        return RESULT_ERROR;

    if (true == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
        m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    if (m_bReadyToDie == false && m_bDead == false)
    {
        for (_uint i = 0; i < 3; ++i)
            m_pRendererCom->Add_Debug(m_pHitColliderCom[i]);
    }
    // 공격 콜라이더
    if (m_bIsAttacking == true)
    {
        for (_uint i = 0; i < 3; ++i)
            m_pRendererCom->Add_Debug(m_pAtkColliderCom[i]);
    }
#endif

    return RESULT_NOPROBLEM;
}

HRESULT CFrozenWolf::Render()
{
    if(nullptr == m_pModelCom)
        return E_FAIL;

    if (FAILED(SetUp_ConstantTable()))
        return E_FAIL;

	/* 모션 트레일 사용할 때 m_bUseMotionTrail 변수를 true로 끌 때 false로*/
	// 모션트레일에 쓰이는 본매트릭스 정보 받을 컨테이너
	vector<CModel::MESHDESC*> MeshDescVector;

    _uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

    _uint iPass = m_iPass;

    for (_uint i = 0; i < iNumMaterials; ++i)
    {

        m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
        m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");
        
        if (i == 0) 
        {
            m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");
            iPass = 10;
        }

        else
            iPass = m_iPass;

		if (2 == i || 3 == i)
		{
			if (12 != m_iPass)
				iPass = 17;
			else
				iPass = 18;
		}

		if (true == m_bUseMotionTrail)
			m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices");
    }

	// 모션 트레일 객체 생성
	if (true == m_bUseMotionTrail && m_TimeAccum_MotionTrail > 0.1) 
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 450;	// 본매트릭스갯수 (고정)
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// 본매트릭스 정보들
		tMotionTrailDesc.vColor = _float4(0.3f, 0.3f, 1.f, 1.f);	// 원하는 모션 트레일 색깔
		tMotionTrailDesc.fRimPower = 10.f;	// 림라이트 파워 (작을수록 두꺼움)


		if (FAILED(m_pGameInstance->Add_GameObject(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"), TEXT("Prototype_GameObject_MotionTrail"), &tMotionTrailDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		m_pGameInstance->Get_Back(GM->Get_CurrentLevel(), TEXT("Layer_MotionTrail"))->Get_Transform()->Set_WorldMatrix(m_pTransformCom->Get_WorldFloat4x4());
		MeshDescVector.clear();

        m_TimeAccum_MotionTrail = 0.0;
	}

    return S_OK;
}

HRESULT CFrozenWolf::Render_Shadow()
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
    _float4x4 LightViewMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix;
    if (FAILED(m_pShaderCom->Set_RawValue("g_LightViewMatrix", &LightViewMatrix, sizeof(_float4x4))))
    {
        BREAKPOINT;
        return E_FAIL;
    }

    _float4x4 LightProjMatrix = pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix;
    if (FAILED(m_pShaderCom->Set_RawValue("g_LightProjMatrix", &LightProjMatrix, sizeof(_float4x4))))
    {
        BREAKPOINT;
        return E_FAIL;
    }

    _uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();
    _uint iPass = 14;

    for (_uint i = 0; i < iNumMaterials; ++i)
    {
        m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices");
    }

    return S_OK;
}

void CFrozenWolf::PostRender(_double TimeDelta)
{
    ImGui::SetNextWindowBgAlpha(0.7f);
    ImGui::Begin("Object Control");

    CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

    // 애니메이션 테스트
    pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
    pImGui_Manager->SmallSeparator();

    // 월드행렬 테스트
    pImGui_Manager->SetGameObjectPosition(string("FW Pos"), m_pTransformCom);
    pImGui_Manager->SetGameObjectScale(string("FW Scale"), m_pTransformCom);
    pImGui_Manager->SmallSeparator();

    // 쉐이더 테스트
    pImGui_Manager->SetGameObjectShaderPass(string("FW Shader"), m_pShaderCom, (_int*)(&m_iPass));
    pImGui_Manager->SmallSeparator();

    // 상태 체크
    ImGui::Text("AnimIndex : %d", m_pModelCom->Get_CurrAnimIndex());
    pImGui_Manager->CheckMonsterState(string("FW State"), m_pStateMachineCom);

    // SpawnMotion
    if (ImGui::Button("SPAWN"))
    {
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
        m_iAnimPlayIndex = 0;
    }

    ImGui::SameLine();
    pImGui_Manager->SmallSeparator();

    // ScratchMotion
    if (ImGui::Button("SCRATCH"))
    {
        m_eWolfSkill = WOLF_SCRATCH;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // DashhMotion
    if (ImGui::Button("DASH"))
    {
        m_eWolfSkill = WOLF_DASH;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dash"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // SummonWolf
    if (ImGui::Button("SUMMON"))
    {
        m_eWolfSkill = WOLF_SUMMON;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Summon"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // Spin
    if (ImGui::Button("SPIN"))
    {
        m_eWolfSkill = WOLF_SPINATTACK;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_SpinAtk"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // ThrowFire
    if (ImGui::Button("THROWFIRE"))
    {
        m_eWolfSkill = WOLF_THROWFIRE;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_ThrowFire"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // Rain
    if (ImGui::Button("RAIN"))
    {
        m_eWolfSkill = WOLF_PURPLERAIN;
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_PurpleRain"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
        m_iAnimPlayIndex = 0;
    }

    pImGui_Manager->SmallSeparator();

    // 현재 상태제거
    if (ImGui::Button("Remove State"))
    {
        m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
    }
    ImGui::SameLine();

    // Idle 상태 제거
    if (ImGui::Button("Remove State Idle"))
    {
        m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_IDLE);
    }
    ImGui::SameLine();

    // Idle 상태 생성
    if (ImGui::Button("Create State Idle"))
    {
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());
    }
    pImGui_Manager->SmallSeparator();

    // 플레이어와의 거리
    ImGui::Text("Distance From Player : %.1f", m_pTransformCom->Get_Distance(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0)->Get_Transform()));
    pImGui_Manager->SmallSeparator();

    // 플레이어와의 거리
    ImGui::Text("HP : %.1f", m_pStatusCom->Get_HP());
    pImGui_Manager->SmallSeparator();

    // OnlyIdle 상태로 고정
    ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle);
    pImGui_Manager->SmallSeparator();

    // Dead 처리
    pImGui_Manager->ColoredButtonBegin(200, 50, 0);
    if (ImGui::Button("DEAD"))
    {
        m_bDead = true;
    }
    pImGui_Manager->ColoredButtonEnd();

    ImGui::End();

	ImGui::Begin("Noise Control");

	ImGui::DragFloat("MoveUV", &m_fMoveUVSpeed, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("NoiseX", &m_fNoiseXSpeed, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("NoiseY", &m_fNoiseYSpeed, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("NoisePower", &m_fNoisePower, 0.01f, -10.f, 10.f);

	if (ImGui::InputInt("TexIndex", &m_iTexIndex))
	{
		if (m_iTexIndex < 0)
			m_iTexIndex = 0;
		if (m_iTexIndex > 99)
			m_iTexIndex = 99;
	}

	ImGui::End();
}

HRESULT CFrozenWolf::Save_ObjectData(HANDLE hFile)
{
    return S_OK;
}

HRESULT CFrozenWolf::Load_ObjectData(HANDLE hFile)
{
    return S_OK;
}

_bool CFrozenWolf::Damaged(CCollider* pAtkCollider, _float fDamaged)
{
    // Player Weapon이랑 충돌 되었을 시 실행시키는 함수
        // 죽기 직전 상태라면 충돌검사 패스
    if (m_bReadyToDie == true)
        return false;

    // 데미지 받음
    auto fRate = m_pStatusCom->Get_MaxHP() / m_pStatusCom->Get_HP();

    if (false == m_pStatusCom->Damaged(fDamaged * fRate))
        return false;

    if (m_eWolfSkill != WOLF_DASH)
        ++m_iHitCnt;

    // pAtkCollider 의 부모(플에이어 무기 객체)를 얻어와서 특정 기능을 실행시킬 수 있음
    CGameObject* pDestObject = pAtkCollider->Get_Parent();

    // 어느 방향에서 데미지 받았는지 계산
    CTransform* pDestTransfrom = pDestObject->Get_Transform();

    // 죽지 않았고, m_OnDamagedAnimTimeAcc 이 5.0 이상이면 OnDamaged 상태로 무조건 가도록
    if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 5.0)
    {
        // 공격 중이거나 넉백상태이거나 도망 중이거나 이미 OnDamaged 상태이면 패스
        if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk")       ||
            m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_KnockBack") ||
            m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_OnDamaged"))
            return false;

        // 공격 쿨타임 0.5초로 초기화
        m_pMonsterCoolerCom->Set_Cool(CMonsterCooler::COOLTYPE_ATK, 0.5);

        // 피격 방향 계산 (앞뒤)
        CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
        _uint iAnimIndex = 0;

        // 피격 방향에 맞는 애니메이션 실행
        if (DamagedDir == CTransform::DAMAGED_FRONT)
            iAnimIndex = 13;
        else if (DamagedDir == CTransform::DAMAGED_BACK)
            iAnimIndex = 14;

        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

        CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Damaged.wav"), 0.5f, true, m_pTransformCom->Get_Position());

        
        // 일정 시간 동안 OnDamaged 상태 되지 않도록 쿨타임 초기화
        m_OnDamagedAnimTimeAcc = 0.0;


    }

    else if (m_bReadyToDie == true)
    {

        // 타겟에서 제거
        static_cast<CPlayer*>(m_pPlayer)->Eliminate_DeadTarget(this);

        // 피격 방향 계산 (앞뒤)
        CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
        _uint iAnimIndex = 0;

        // 피격 방향에 맞는 애니메이션 실행
        if (DamagedDir == CTransform::DAMAGED_FRONT)
            iAnimIndex = 15;
        else if (DamagedDir == CTransform::DAMAGED_BACK)
            iAnimIndex = 18;

        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
    }

    // 죽는 이펙트
 //   CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 4.5f, 0.f);

    // 충돌하면 m_bReadyToDie 를 true 로
   // m_bReadyToDie = true;

    return true;
}

_matrix CFrozenWolf::Get_AtkBoneMatrix(_uint iIndex)
{
    return m_AtkCollider_TransformMatrix[iIndex];
}

_float4* CFrozenWolf::Get_BonePos_Pointer(_uint iIndex)
{
    XMStoreFloat4(&m_vBonePos[iIndex], m_AtkCollider_TransformMatrix[iIndex].r[3]);

    return &m_vBonePos[iIndex];
}

_float4x4* CFrozenWolf::Get_CenterBoneMatrix()
{
    return &m_Center_TransformMatrix;
}

_vector CFrozenWolf::Get_TailPos()
{
    auto pBone = m_pModelCom->Get_BoneMatrixPtr("tail14_B_all_spo");

    auto World = XMLoadFloat4x4(pBone.pCombinedMatrix) * XMLoadFloat4x4(&m_HitCollider_SocketPivotMatrix[0]) * m_pTransformCom->Get_WorldMatrix();
   
    m_vTailPos = World.r[3];

    return m_vTailPos;
}

void CFrozenWolf::MoveToNextSkill()
{
    //++m_iCurSkill;

    //if(m_iCurSkill > 9)
    //    m_iCurSkill = 0;

    //m_eWolfSkill = m_SkillSequence[m_iCurSkill];

    m_CoolTime = 0.0;
}

void CFrozenWolf::Set_TrailOn(_bool isActive)
{
    for (_uint i = 0; i < 5; ++i)
    {
        m_pTrail[i]->Set_Active(isActive);
    }
}

HRESULT CFrozenWolf::Ready_Collider_Info()
{
    // 피격 콜라이더
    const char* HitBones[3] = {"shita2", "L_yubiB2", "R_yubiB2"};

    for (_uint i = 0; i < 3; ++i)
    {
        m_HitCollider_BoneMatrixPtr[i] = m_pModelCom->Get_BoneMatrixPtr(HitBones[i]);
        m_HitCollider_SocketPivotMatrix[i] = m_pModelCom->Get_TransformMatrix();
    }

    // 공격 콜라이더
    const char* AtkBones[3] = { "L_yubiB2", "R_yubiB2", "shita3"};

    for (_uint i = 0; i < 3; ++i)
    {
        m_AtkCollider_BoneMatrixPtr[i] = m_pModelCom->Get_BoneMatrixPtr(AtkBones[i]);
        m_AtkCollider_SocketPivotMatrix[i] = m_pModelCom->Get_TransformMatrix();
    }

    m_Center_BoneMatrixPtr = m_pModelCom->Get_BoneMatrixPtr("center");


    return S_OK;
}

HRESULT CFrozenWolf::Collider_Transform_Update()
{
    for (_uint i = 0; i < 3; ++i)
    {
        m_HitCollider_TransformMatrix[i] = XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr[i].pCombinedMatrix) * XMLoadFloat4x4(&m_HitCollider_SocketPivotMatrix[i]) * m_pTransformCom->Get_WorldMatrix();
        m_pHitColliderCom[i]->Update_Transform(m_HitCollider_TransformMatrix[i]);
    }

   for (_uint i = 0; i < 3; ++i)
   {
       m_AtkCollider_TransformMatrix[i] = XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr[i].pCombinedMatrix) * XMLoadFloat4x4(&m_AtkCollider_SocketPivotMatrix[i]) * m_pTransformCom->Get_WorldMatrix();

       m_pAtkColliderCom[i]->Update_Transform(m_AtkCollider_TransformMatrix[i]);
   }

   _matrix CenterMatrix_NoRot = XMLoadFloat4x4(m_Center_BoneMatrixPtr.pCombinedMatrix) * XMLoadFloat4x4(&m_HitCollider_SocketPivotMatrix[0]) * m_pTransformCom->Get_WorldMatrix();

   CenterMatrix_NoRot.r[0] = __vector(1.f, 0.f, 0.f, 0.f);
   CenterMatrix_NoRot.r[1] = __vector(0.f, 1.f, 0.f, 0.f);
   CenterMatrix_NoRot.r[2] = __vector(0.f, 0.f, 1.f, 0.f);

   XMStoreFloat4x4(&m_Center_TransformMatrix, CenterMatrix_NoRot);

   return S_OK;
}

HRESULT CFrozenWolf::SetUp_Components()
{
    /* For.Com_StateMachine */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
        return E_FAIL;

    /* For.Com_Status */
    CStatus::STATUSDESC StatusDesc;
    StatusDesc.fMaxHP = 400.f;
    StatusDesc.fHP = StatusDesc.fMaxHP;
    StatusDesc.fDamaged = 5.f;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
        return E_FAIL;

    /* For.Com_MonsterCooler */
    // 여기서 각 행동들의 쿨타임을 설정
    CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
    MonsterCoolerDesc.MaxAttackCool = 3.0;
    MonsterCoolerDesc.AttackCool = 7.0;
    MonsterCoolerDesc.MaxRunAwayCool = 6.5;
    MonsterCoolerDesc.RunAwayCool = 8.0;
    MonsterCoolerDesc.MaxChaseCool = 15.0;

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
        return E_FAIL;


    /* For.Com_Collider */
    {
        CCollider::COLLIDERDESC ColliderDesc;

        // 피격 콜라이더
        const _tchar* HitColliderTag[3] = { L"shita2Hit", L"L_yubiB2Hit", L"R_yubiB2Hit" };

        ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
        ColliderDesc.vScale = _float3(3.5f, 3.5f, 3.5f);
        ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
        ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);


        for (_int i = 0; i < 3; ++i)
        {
            if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), HitColliderTag[i], (CComponent**)&m_pHitColliderCom[i], &ColliderDesc)))
                return E_FAIL;
        }



        // 공격 콜라이더
        const _tchar* AtkColliderTag[3] = { L"L_yubiB2", L"R_yubiB2", L"shita3"};

        ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
        ColliderDesc.vScale = _float3(4.5f, 4.5f, 4.5f);
        ColliderDesc.vRotation = _float4(0.f, 0.f, 0.f, 1.f);
        ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

        for (_int i = 0; i < 3; ++i)
        {
            if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), AtkColliderTag[i], (CComponent**)&m_pAtkColliderCom[i], &ColliderDesc)))
                return E_FAIL;
        }
    }

    /* For.Com_Renderer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
        return E_FAIL;


    /* For.Com_Navigation */
    CNavigation::NAVIDESC tNaviDesc;
    tNaviDesc.iCurrentIndex = 0;
    tNaviDesc.pParent = m_pTransformCom;

    if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Navigation_KeyGrave"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
    {
        BREAKPOINT;
        return E_FAIL;
    }

    m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());


    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;


    /* Dissolve_Texture */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Noise"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
        return E_FAIL;


    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_KEYGRAVE, TEXT("Prototype_Component_Model_FrozenWolf"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFrozenWolf::SetUp_RootNodeIndex()
{
    //애니메이션이 이동하면서 원점에서 벗어나면 객체의 실제 위치도 그만큼 이동시켜줘야 하는데 매번 일일이 잡아주기 번거롭기 때문에
    //루트노드를 설정해두고, 루트노드의 이동값만큼 객체를 이동하게 만들기 위해

    _int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");

    if (UINT_MAX == iRootNodeIndex)
    {
        BREAKPOINT;
        return E_FAIL;
    }

    m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

    return S_OK;
}

HRESULT CFrozenWolf::SetUp_Animations()
{
    m_pModelCom->Add_OnceListAll();

    m_pModelCom->Rmv_OnceList(0);
    m_pModelCom->Rmv_OnceList(8);
    m_pModelCom->Rmv_OnceList(30);
    m_pModelCom->Rmv_OnceList(33);
    m_pModelCom->Rmv_OnceList(35);
    m_pModelCom->Rmv_OnceList(40);

    // 모든 애니메이션들을 보간되게 함 (이거 안 하면 보간 안 됨)
    /*for (_uint i = 0; i < 41; ++i)
        m_pModelCom->Add_LerpList(i);*/

    for (_uint i = 0; i < 41; ++i)
    {
        //if (i == 8)
        //    continue;

        m_pModelCom->Add_LerpList(i);
    }


    return S_OK;
}

HRESULT CFrozenWolf::SetUp_ConstantTable()
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

    if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
        return E_FAIL;

    _bool       isBlur = false;

    if (FAILED(m_pShaderCom->Set_RawValue("g_isBlur", &isBlur, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_isRim", &isBlur, sizeof(_bool))))
        return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveUV", &m_fMoveUV, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise", &m_fNoiseX, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fMoveNoise_Y", &m_fNoiseY, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fNoisePower", &m_fNoisePower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_SourTexture", m_iTexIndex)))
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

HRESULT CFrozenWolf::OnCollision(_double TimeDelta)
{
    // 플레이어 밀어내기
    auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
    for (auto& pDestCollider : *pColliderList)
    {
        for (_uint i = 0; i < 3; ++i)
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

    return S_OK;
}

HRESULT CFrozenWolf::ChangeState(_double TimeDelta)
{
    // 현재 레벨에 있는 Player 와 Player 의 Transform 을 가져옴
    if (m_pPlayer == nullptr)
    {
        m_pPlayer = m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0);

        if (m_pPlayer == nullptr)
            return E_FAIL;

        m_pPlayerTransform = m_pPlayer->Get_Transform();

        if (m_pPlayerTransform == nullptr)
            return E_FAIL;
    }

    // ==========================================================================================

    if (m_bOnlyIdle == true || m_isWaiting == true)
        return S_OK;

    // 0. 스폰 
    if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
    {
        // Spawn 상태 생성
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
        m_bSpawn = true;

        m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

        return S_OK;
    }


    // 1. 공격
		// 쿨타임이 되는 경우에만 상태 생성하고 쿨타임 초기화 
    if (m_pStateMachineCom->FindMonsterState() == false)
    {
        if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_ATK) == true && m_CoolTime > 3.0)
        {
            
            Change_AttackPattern();

            m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_FrozenWolfAtk::Create(&m_eWolfSkill));
            m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_ATK);
            return S_OK;
        }
    } 

    return S_OK;
}

void CFrozenWolf::Gravity(_double TimeDelta)
{
    m_fForce += m_fGravity * (_float)TimeDelta;

    auto vPosY = XMVectorGetY(m_pTransformCom->Get_Position());

    vPosY -= m_fForce;

    m_pTransformCom->Set_PositionY(vPosY);
}

void CFrozenWolf::Change_AttackPattern()
{
    _float fDist = m_pTransformCom->Get_Distance(m_pPlayerTransform);

    // 1 Phase
	if (m_bSpecialMove[0] == false)
	{
		_uint iNum = rand() % 2;

		switch (iNum)
		{
		case 0:
			m_eWolfSkill = WOLF_SUMMON;
			break;
		case 1:
			m_eWolfSkill = WOLF_SCRATCH;
			break;
		}
	}

    // 2 Phase 공격 추가 렌덤으로 출력
    else
    {
        if (fDist > 20.f)
        {
            _uint iNum = rand() % 2;
            switch (iNum)
            {
            case 0:
                m_eWolfSkill = WOLF_THROWFIRE;
                break;
            case 1:
                m_eWolfSkill = WOLF_SPINATTACK;
                break;
            }
        }

        else
        {
            _uint iNum = rand() % 2;

            switch (iNum)
            {
            case 0:
                m_eWolfSkill = WOLF_SCRATCH;
                break;
            case 1:
                m_eWolfSkill = WOLF_SUMMON;
                break;
            }
        }
    }


    if (m_iHitCnt >= 6 && fDist > 5.f)
    {
        m_eWolfSkill = WOLF_DASH;
        m_iHitCnt = 0;
    }

    // 필살기 두번정도 변수 지정해줘서 발동시키기
    if (m_pStatusCom->Get_HP() <= 300.f && m_bSpecialMove[0] == false)
    {
        m_eWolfSkill = WOLF_PURPLERAIN;
        m_bSpecialMove[0] = true;
    }

    //if (m_pStatusCom->Get_HP() <= 150.f && m_bSpecialMove[1] == false)
    //{
    //    m_eWolfSkill = WOLF_PURPLERAIN;
    //    m_bSpecialMove[1] = true;
    //}
}

CFrozenWolf* CFrozenWolf::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CFrozenWolf* pInstance = new CFrozenWolf(pDevice, pDeviceContext);

    if (FAILED(pInstance->NativeConstruct_Prototype()))
    {
        MSGBOX("Failed to Created CFrozenWolf(Prototype)!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFrozenWolf::Clone(void* pArg)
{
    CFrozenWolf* pInstance = new CFrozenWolf(*this);

    if (FAILED(pInstance->NativeConstruct(pArg)))
    {
        MSGBOX("Failed to Clone CFrozenWolf!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFrozenWolf::Free()
{
    __super::Free();

    Safe_Release(m_pStateMachineCom);
    Safe_Release(m_pMonsterCoolerCom);
    Safe_Release(m_pStatusCom);
    Safe_Release(m_pRendererCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pShaderCom);

    for (_uint i = 0; i < 3; ++i)
        Safe_Release(m_pHitColliderCom[i]);

    for(_uint i = 0; i < 3; ++i)
        Safe_Release(m_pAtkColliderCom[i]);
}
