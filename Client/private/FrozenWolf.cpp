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
    //  ============================== Loader���� ���� ��===========================================

    // ���� Ÿ�� ����
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

    // �ִϸ��̼� ����
    if (FAILED(SetUp_Animations()))
        return E_FAIL;

    // �ݶ��̴� ���� �ʱ�ȭ
   if (FAILED( Ready_Collider_Info()))
       return E_FAIL;

    // ���� ���� ���� (Idle)
    m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());

    // ���̴��н� ����
    m_iPass = 0;

    // ���� ��ġ ����
    if (pArg != nullptr)
    {
        _vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
        memcpy(&vPosition, pArg, sizeof(_vector));
        m_pTransformCom->Set_Position(vPosition);
        m_pTransformCom->MakeSameDirection(__vector(-1.f, 0.f, 0.f, 0.f));
    }

    m_eWolfSkill = WOLF_DASH;

    //m_bImGuiController = true;

    // ��ų ���� ���ϱ�

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


		//// ��

		// Left

		//GM->Add_Monster(this);
		//GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_FROZENWOLF_APPEAR, m_pTransformCom);
	}



    return S_OK;
}

HRESULT CFrozenWolf::NativeConstruct_Pool(void* pArg)
{
    //  ============================== ���� �ΰ��ӿ��� ���� �� ========================================
    
    // ���ӸŴ����� ���� �� ��� ���͵��� ���� �����ϰ� �ֱ� ������ ������ƮǮ���� ������ �� ���ӸŴ����� ���
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

    // CoolTime ����
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

    // MonsterCooler ������Ʈ ������ ��Ÿ�� �ʱ�ȭ
    m_pMonsterCoolerCom->Update(TimeDelta);


    if(FAILED(Collider_Transform_Update()))
        return RESULT_ERROR;

    if (m_bReadyToDie == false && m_bDead == false)
    {
        for (_uint i = 0; i < 3; ++i)
            m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom[i]);
    }

    // ���� �ݶ��̴��� ���� ����(m_bIsAttacking)�� ���� ���
    // m_bIsAttacking �� ���� ���·� �� �� ���� ���ָ� �� (SetAttackTime �Լ� ���)
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
    // ���� �ݶ��̴�
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

	/* ��� Ʈ���� ����� �� m_bUseMotionTrail ������ true�� �� �� false��*/
	// ���Ʈ���Ͽ� ���̴� ����Ʈ���� ���� ���� �����̳�
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

	// ��� Ʈ���� ��ü ����
	if (true == m_bUseMotionTrail && m_TimeAccum_MotionTrail > 0.1) 
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 450;	// ����Ʈ�������� (����)
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// ����Ʈ���� ������
		tMotionTrailDesc.vColor = _float4(0.3f, 0.3f, 1.f, 1.f);	// ���ϴ� ��� Ʈ���� ����
		tMotionTrailDesc.fRimPower = 10.f;	// ������Ʈ �Ŀ� (�������� �β���)


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

    // �ִϸ��̼� �׽�Ʈ
    pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
    pImGui_Manager->SmallSeparator();

    // ������� �׽�Ʈ
    pImGui_Manager->SetGameObjectPosition(string("FW Pos"), m_pTransformCom);
    pImGui_Manager->SetGameObjectScale(string("FW Scale"), m_pTransformCom);
    pImGui_Manager->SmallSeparator();

    // ���̴� �׽�Ʈ
    pImGui_Manager->SetGameObjectShaderPass(string("FW Shader"), m_pShaderCom, (_int*)(&m_iPass));
    pImGui_Manager->SmallSeparator();

    // ���� üũ
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

    // ���� ��������
    if (ImGui::Button("Remove State"))
    {
        m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
    }
    ImGui::SameLine();

    // Idle ���� ����
    if (ImGui::Button("Remove State Idle"))
    {
        m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_IDLE);
    }
    ImGui::SameLine();

    // Idle ���� ����
    if (ImGui::Button("Create State Idle"))
    {
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());
    }
    pImGui_Manager->SmallSeparator();

    // �÷��̾���� �Ÿ�
    ImGui::Text("Distance From Player : %.1f", m_pTransformCom->Get_Distance(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0)->Get_Transform()));
    pImGui_Manager->SmallSeparator();

    // �÷��̾���� �Ÿ�
    ImGui::Text("HP : %.1f", m_pStatusCom->Get_HP());
    pImGui_Manager->SmallSeparator();

    // OnlyIdle ���·� ����
    ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle);
    pImGui_Manager->SmallSeparator();

    // Dead ó��
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
    // Player Weapon�̶� �浹 �Ǿ��� �� �����Ű�� �Լ�
        // �ױ� ���� ���¶�� �浹�˻� �н�
    if (m_bReadyToDie == true)
        return false;

    // ������ ����
    auto fRate = m_pStatusCom->Get_MaxHP() / m_pStatusCom->Get_HP();

    if (false == m_pStatusCom->Damaged(fDamaged * fRate))
        return false;

    if (m_eWolfSkill != WOLF_DASH)
        ++m_iHitCnt;

    // pAtkCollider �� �θ�(�ÿ��̾� ���� ��ü)�� ���ͼ� Ư�� ����� �����ų �� ����
    CGameObject* pDestObject = pAtkCollider->Get_Parent();

    // ��� ���⿡�� ������ �޾Ҵ��� ���
    CTransform* pDestTransfrom = pDestObject->Get_Transform();

    // ���� �ʾҰ�, m_OnDamagedAnimTimeAcc �� 5.0 �̻��̸� OnDamaged ���·� ������ ������
    if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 5.0)
    {
        // ���� ���̰ų� �˹�����̰ų� ���� ���̰ų� �̹� OnDamaged �����̸� �н�
        if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk")       ||
            m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_KnockBack") ||
            m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_OnDamaged"))
            return false;

        // ���� ��Ÿ�� 0.5�ʷ� �ʱ�ȭ
        m_pMonsterCoolerCom->Set_Cool(CMonsterCooler::COOLTYPE_ATK, 0.5);

        // �ǰ� ���� ��� (�յ�)
        CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
        _uint iAnimIndex = 0;

        // �ǰ� ���⿡ �´� �ִϸ��̼� ����
        if (DamagedDir == CTransform::DAMAGED_FRONT)
            iAnimIndex = 13;
        else if (DamagedDir == CTransform::DAMAGED_BACK)
            iAnimIndex = 14;

        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

        CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("DarkWolf_Damaged.wav"), 0.5f, true, m_pTransformCom->Get_Position());

        
        // ���� �ð� ���� OnDamaged ���� ���� �ʵ��� ��Ÿ�� �ʱ�ȭ
        m_OnDamagedAnimTimeAcc = 0.0;


    }

    else if (m_bReadyToDie == true)
    {

        // Ÿ�ٿ��� ����
        static_cast<CPlayer*>(m_pPlayer)->Eliminate_DeadTarget(this);

        // �ǰ� ���� ��� (�յ�)
        CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
        _uint iAnimIndex = 0;

        // �ǰ� ���⿡ �´� �ִϸ��̼� ����
        if (DamagedDir == CTransform::DAMAGED_FRONT)
            iAnimIndex = 15;
        else if (DamagedDir == CTransform::DAMAGED_BACK)
            iAnimIndex = 18;

        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
    }

    // �״� ����Ʈ
 //   CEffect_Manager::GetInstance()->Create_DeadEffect(m_pTransformCom, 4.5f, 0.f);

    // �浹�ϸ� m_bReadyToDie �� true ��
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
    // �ǰ� �ݶ��̴�
    const char* HitBones[3] = {"shita2", "L_yubiB2", "R_yubiB2"};

    for (_uint i = 0; i < 3; ++i)
    {
        m_HitCollider_BoneMatrixPtr[i] = m_pModelCom->Get_BoneMatrixPtr(HitBones[i]);
        m_HitCollider_SocketPivotMatrix[i] = m_pModelCom->Get_TransformMatrix();
    }

    // ���� �ݶ��̴�
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
    // ���⼭ �� �ൿ���� ��Ÿ���� ����
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

        // �ǰ� �ݶ��̴�
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



        // ���� �ݶ��̴�
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
    //�ִϸ��̼��� �̵��ϸ鼭 �������� ����� ��ü�� ���� ��ġ�� �׸�ŭ �̵�������� �ϴµ� �Ź� ������ ����ֱ� ���ŷӱ� ������
    //��Ʈ��带 �����صΰ�, ��Ʈ����� �̵�����ŭ ��ü�� �̵��ϰ� ����� ����

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

    // ��� �ִϸ��̼ǵ��� �����ǰ� �� (�̰� �� �ϸ� ���� �� ��)
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
    // ������Ʈ
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
        // ������ �ؽ�ó ���ε�
        if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 99)))
            return E_FAIL;

        // ������ Alpha ���ε�
        if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAlpha", &m_fDissolveAmount, sizeof(_float))))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CFrozenWolf::OnCollision(_double TimeDelta)
{
    // �÷��̾� �о��
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

                // ������
                _float fSourColliderRadius = m_pHitColliderCom[i]->Get_Sphere()->Radius;
                _float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;

                // ����
                XMFLOAT3 sourcenterXM3 = m_pHitColliderCom[i]->Get_Sphere()->Center;
                XMFLOAT3 destcenterXM3 = pDestCollider->Get_Sphere()->Center;
                _float4 sourcenterFloat4 = _float4(sourcenterXM3.x, sourcenterXM3.y, sourcenterXM3.z, 1.f);
                _float4 destcenterFloat4 = _float4(destcenterXM3.x, destcenterXM3.y, destcenterXM3.z, 1.f);
                _vector vSourCenter = XMLoadFloat4(&sourcenterFloat4);
                _vector vDestCenter = XMLoadFloat4(&destcenterFloat4);


                // ��ģ�κ� ũ��
                _float fCollisionSize = fDestColliderRadius + fSourColliderRadius - DISTANCE(vDestCenter - vSourCenter);

                // ������
                _vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

                // ��ġ�κи�ŭ �з�����
                _vector vPos = pDestTransform->Get_Position();
                pDestTransform->Set_Position(vPos + vDir * fCollisionSize);
            }
        }
    }

    return S_OK;
}

HRESULT CFrozenWolf::ChangeState(_double TimeDelta)
{
    // ���� ������ �ִ� Player �� Player �� Transform �� ������
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

    // 0. ���� 
    if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
    {
        // Spawn ���� ����
        m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
        m_bSpawn = true;

        m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

        return S_OK;
    }


    // 1. ����
		// ��Ÿ���� �Ǵ� ��쿡�� ���� �����ϰ� ��Ÿ�� �ʱ�ȭ 
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

    // 2 Phase ���� �߰� �������� ���
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

    // �ʻ�� �ι����� ���� �������༭ �ߵ���Ű��
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
