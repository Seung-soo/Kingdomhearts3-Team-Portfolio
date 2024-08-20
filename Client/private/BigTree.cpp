#include "stdafx.h"
#include "BigTree.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "Camera_Perspective.h"
#include "MotionTrail.h"

// State
#include "State_BigTreeAtk.h"
 
// State
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h"
#include "State_MonsterDynamicMove.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"

#define	 ANIMSPEED 1.2f

CBigTree::CBigTree(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CBigTree::CBigTree(const CBigTree& rhs)
	: CMonster(rhs)
{
}

HRESULT CBigTree::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBigTree::NativeConstruct(void* pArg)
{
	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 12.5f;
	TransformDesc.fRotationPerSec = 1.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ���� Ÿ�� ����
	m_MonsterType = MONSTERTYPE_BIGTREE;

	// ������Ʈ �ʱ�ȭ
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ��Ʈ����ε��� �ʱ�ȭ
	if (FAILED(SetUp_RootNodeIndex()))
		return E_FAIL;

	// �ִϸ��̼� ����
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// �ݶ��̴� ���� �ʱ�ȭ
	Ready_Collider_Info();

	// ���� ���� ���� (Idle)
	m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());

	// ���̴��н� ����
	m_iPass = 0;
	
	// ����ü ���� ����
	m_fFrustumRange = 20.f;

	// ���� ��ġ ����
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	m_bIsBoss = true;

	m_pGameInstance->StopSound(CSoundMgr::BGM);
	//m_pGameInstance->PlayBGM(TEXT("BigTreeBGM.mp3"), 1.f);

	return S_OK;
}

HRESULT CBigTree::NativeConstruct_Pool(void * pArg)
{
	//	���� �Ŵ��� ���� ����Ʈ�� �߰�
	GM->Add_Monster(this);

	GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_BIGTREE_APPEAR, m_pTransformCom);

	CEffect_Manager::GetInstance()->Create_BigTreeParticle(&m_pEffectBone_Pos[0], &m_bDead);

	return S_OK;
}

_int CBigTree::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
	{
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_BossTarget"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_Cnt"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_CntBack"));

		return RESULT_DEAD;
	}

	// �ǰ� �ִϸ��̼� ��� ��Ÿ�� ó��
	m_OnDamagedAnimTimeAcc += TimeDelta;

	// ���� ���� ó��
	ChangeState(TimeDelta);

	// ���¸ӽ� Action ����
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// ����Ʈ�� ��ġ ����
	m_pTransformCom->Get_PositionFloat4_Pointer();

	// �� ������Ʈ
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_TEST || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * ANIMSPEED, m_pTransformCom);
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * ANIMSPEED, m_pTransformCom, m_pNavigationCom);

	// �ݶ��̴��� ��ġ ���
	Collider_Transform_Update();

	// �ݶ��̴� ��ġ ������Ʈ
	for (_uint i = 0; i < 4; ++i)
		m_pHitColliderCom[i]->Update_Transform(m_HitCollider_TransformMatrix[i]);
	// ���� �ݶ��̴�
	for (_uint i = 0; i < 5; ++i)
		m_pAtkColliderCom[i]->Update_Transform(m_AtkCollider_TransformMatrix[i]);

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	// ���� �ݶ��̴��� ���� ������ ���� ���
	if (m_bReadyToDie == false)
	{
		for (_uint i = 0; i < 4; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom[i]);
	}	
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 5; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom[i]);
	}

	return RESULT_NOPROBLEM;
}

_int CBigTree::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// �浹 �˻� (LateTick ���� ������ ��)
	OnCollision(TimeDelta);

	// ����ü �ȿ� �ִٸ� + m_bDead ���°� �ƴϸ� �����׷쿡 �߰�
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) &&
		FALSE == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// �ݶ��̴� ����� ������
	// �ǰ� �ݶ��̴�
	for (_uint i = 0; i < 4; ++i)
		m_pRendererCom->Add_Debug(m_pHitColliderCom[i]);
	// ���� �ݶ��̴�
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 5; ++i)
			m_pRendererCom->Add_Debug(m_pAtkColliderCom[i]);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CBigTree::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// ���̴� ���� ����� �� ����
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// ���Ʈ���Ͽ� ���̴� ����Ʈ���� ���� ���� �����̳�
	vector<CModel::MESHDESC*> MeshDescVector;

	// ���� �����ִ� ���׸��� �� �����ͼ� ���̴��� �ؽ�ó ���� ����� �� �����ϰ� ������
	_uint iNumMaterials = m_pModelCom->Get_NumberOfMaterials();

	_uint iPass = m_iPass;
	_float fUsingLUT = 1.f;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture");

		if (i == 8)
		{
			iPass = 10;
			m_pModelCom->Bind_OnShader(m_pShaderCom, i, aiTextureType_EMISSIVE, "g_ThirdTexture");

			
			if (FAILED(m_pShaderCom->Set_RawValue("g_fLUTMask", &fUsingLUT, sizeof(_float))))
				return E_FAIL;
		}

		if (true == m_bUseMotionTrail)
			m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices");

	}

	// ��� Ʈ���� ��ü ����
	if (true == m_bUseMotionTrail)
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 450;	// ����Ʈ�������� (����)
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// ����Ʈ���� ������
		tMotionTrailDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);	// ���ϴ� ��� Ʈ���� ����
		tMotionTrailDesc.fRimPower = 1.f;	// ������Ʈ �Ŀ� (�������� �β���)
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

HRESULT CBigTree::Render_Shadow()
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
		m_pModelCom->Render_Heavy(m_pShaderCom, iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CBigTree::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	if (ImGui::Button("Climb"))
	{
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_IDLE);
		
		m_pTransformCom->Set_Position(__vector(128.f, 42.f, 128.f, 1.f));
		m_pTransformCom->Rotation(__vector(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		Set_AnimAndReset(m_pModelCom, 14);

		if (m_pModelCom->Check_CurAnim_And_Finished(14))
			Set_AnimAndReset(m_pModelCom, 15);

		if (m_pModelCom->Check_CurAnim_And_Finished(15))
			Set_AnimAndReset(m_pModelCom, 16);

		if (m_pModelCom->Check_CurAnim_And_Finished(16))
			Set_AnimAndReset(m_pModelCom, 17);
	}

	// �ִϸ��̼� �׽�Ʈ
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// ������� �׽�Ʈ
	pImGui_Manager->SetGameObjectPosition(string("BT Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("BT Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("BT Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// ���̴� �׽�Ʈ
	pImGui_Manager->SetGameObjectShaderPass(string("BT Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// ���� üũ
	pImGui_Manager->CheckMonsterState(string("BT State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	// ���� ��ų
	const char* Skills[BTSKILL_END] = { "Summon_Crow", "Hug", "Ground_HeadButt", "Power_HeadButt", "ScrewDash", "Building_Climb", "Summon_Fruits" };

	ImGui::Text("Next Skill : ");
	ImGui::SameLine();
	ImGui::Text(Skills[m_eSkill]);

	// ��ų ����
	int Select = -1;
	
	ImGui::Combo("SELECT", &Select, Skills, IM_ARRAYSIZE(Skills));
	if (Select != -1 && Select != m_eSkill)
	{
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
		m_eSkill = BIGTREE_SKILL(Select);
	}
	pImGui_Manager->SmallSeparator();

	// ���� �ö󰡱� ����
	if (ImGui::Button("ClimbAtk"))
	{
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
		m_pStatusCom->Set_HP(m_pStatusCom->Get_MaxHP() * 0.35f);
	}

	// Dead ó��
	if (ImGui::Button("DEAD"))
		m_bDead = true;

	ImGui::End();
}

HRESULT CBigTree::Save_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ������� ����
	_float4x4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, m_pTransformCom->Get_WorldMatrix());

	DWORD dwBytes = 0;
	WriteFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CBigTree::Load_ObjectData(HANDLE hFile)
{
	if (hFile == INVALID_HANDLE_VALUE)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���̺굥���Ϳ��� ��������� �ε�
	DWORD		dwBytes = 0;
	_float4x4	worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// ������� ����
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

_bool CBigTree::Damaged(CCollider * pAtkCollider, _float fDamaged)
{
	// �ױ� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true)
		return false;

	// pAtkCollider �� �θ� ���ͼ� Ư�� ����� �����ų �� ����
	CGameObject* pDestObject = pAtkCollider->Get_Parent();

	// ������ ����
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// ��� ���⿡�� ������ �޾Ҵ��� ���
	CTransform* pDestTransfrom = pDestObject->Get_Transform();

	// ���� �ʾҰ�, m_OnDamagedAnimTimeAcc �� 15.0 �̻��̸� OnDamaged ���·� ������ ������
	if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 15.0)
	{
		// ���� ���̸� �н�
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk"))
			return false;

		_uint iAnimIndex = 22;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

		// ���� �ð� ���� OnDamaged ���� ���� �ʵ��� ��Ÿ�� �ʱ�ȭ
		m_OnDamagedAnimTimeAcc = 0.0;
	}
	// �׾��ٸ� Dead ���·� ������ ������
	else if(m_bReadyToDie == true)
	{
		// ������

		// Ÿ�ٿ��� ����
		static_cast<CPlayer*>(m_pPlayer)->Eliminate_DeadTarget(this);

		_uint iAnimIndex = 23;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CBigTree::Ready_Collider_Info()
{
	// �ǰ� �ݶ��̴�
	const char* HitBones[4] = { "headleaf_axis", "tail", "sebone6", "sebone10" };
	for (_uint i = 0; i < 4; ++i)
	{
		m_HitCollider_BoneMatrixPtr[i]      = m_pModelCom->Get_BoneMatrixPtr(HitBones[i]);
		m_HitCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}

	// ���� �ݶ��̴�
	const char* AtkBones[5] = { "headleaf_axis", "R_DyubiD2", "L_DyubiD2", "R_UyubiB3", "L_UyubiB3" };
	for (_uint i = 0; i < 5; ++i)
	{
		m_AtkCollider_BoneMatrixPtr[i]      = m_pModelCom->Get_BoneMatrixPtr(AtkBones[i]);
		m_AtkCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}
}

void CBigTree::Collider_Transform_Update()
{
	// �ǰ� �ݶ��̴�
	for (_uint i = 0; i < 4; ++i)
	{
		m_HitCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_HitCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();

		// ����Ʈ ���� �� ��ġ
		memcpy(&m_pEffectBone_Pos[i], &m_HitCollider_TransformMatrix[i].r[3], sizeof(_vector));		

		// Remove Rotation
		_matrix Matrix = m_HitCollider_TransformMatrix[i];
		Matrix.r[0]    = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(m_HitCollider_TransformMatrix[i].r[0]));
		Matrix.r[1]    = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(m_HitCollider_TransformMatrix[i].r[1]));
		Matrix.r[2]    = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(m_HitCollider_TransformMatrix[i].r[2]));
		XMStoreFloat4x4(&m_pEffectBone_WorldMatrix[i], Matrix);
	}

	// ���� �ݶ��̴�
	for (_uint i = 0; i < 5; ++i)
	{
		m_AtkCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_AtkCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();
	}
}

HRESULT CBigTree::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 400.f;
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
	{
		CCollider::COLLIDERDESC ColliderDesc;

		// �ǰ� �ݶ��̴�
		const _tchar* HitColliderTag[4] = { L"Com_Collider_Hit_headleaf_axis", L"Com_Collider_Hit_tail", L"Com_Collider_Hit_sebone6", L"Com_Collider_Hit_sebone10" };

		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(10.f, 10.f, 10.f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), HitColliderTag[0], (CComponent**)&m_pHitColliderCom[0], &ColliderDesc)))
			return E_FAIL;

		for (_int i = 1; i < 4; ++i)
		{
			ColliderDesc.vScale       = _float3(3.f, 3.f, 3.f);
			ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
			ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), HitColliderTag[i], (CComponent**)&m_pHitColliderCom[i], &ColliderDesc)))
				return E_FAIL;
		}

		// ���� �ݶ��̴�
		const _tchar* AtkColliderTag[5] = { L"Com_Collider_Atk_headleaf_axis", L"Com_Collider_Atk_R_DyubiD2", L"Com_Collider_Atk_L_DyubiD2", L"Com_Collider_Atk_R_UyubiB3", L"Com_Collider_Atk_L_UyubiB3" };

		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(11.5f, 11.5f, 11.5f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), AtkColliderTag[0], (CComponent**)&m_pAtkColliderCom[0], &ColliderDesc)))
			return E_FAIL;

		for (_int i = 1; i < 5; ++i)
		{
			ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
			ColliderDesc.vScale       = _float3(4.f, 4.f, 4.f);
			ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
			ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), AtkColliderTag[i], (CComponent**)&m_pAtkColliderCom[i], &ColliderDesc)))
				return E_FAIL;
		}
	}

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
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
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Navigation_Rapunzel_Boss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL_BOSS, TEXT("Prototype_Component_Model_BigTree"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CBigTree::SetUp_RootNodeIndex()
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

HRESULT CBigTree::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Rmv_OnceList(0);
	m_pModelCom->Rmv_OnceList(1);
	m_pModelCom->Rmv_OnceList(2);
	m_pModelCom->Rmv_OnceList(17);

	m_pModelCom->Add_LerpListAll();

	return S_OK;
}

HRESULT CBigTree::SetUp_ConstantTable()
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

	// �������
	if (FAILED(m_pTransformCom->Bind_OnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	// �����
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	// �������
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_float fUsingLUT = 0.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fLUTMask", &fUsingLUT, sizeof(_float))))
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

void CBigTree::OnCollision(_double TimeDelta)
{
	// �÷��̾� �о��
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
}

void CBigTree::ChangeState(_double TimeDelta)
{
	// ���� ������ �ִ� Player �� Player �� Transform �� ������
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

	// 0. ����
	if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
	{
		// Spawn ���� ����
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
		m_bSpawn = true;

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		return;
	}

	// ==========================================================================================

	// ���� �� ������
	if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) != string("State_Atk"))
	{
		if (m_bCanAtk == false)
		{
			m_AtkCool += TimeDelta;

			_double MaxCool = 5.0;

			if (m_pStatusCom->Get_HP_Ratio() <= 0.5f)
				MaxCool = 3.5;

			if (m_AtkCool >= MaxCool)
			{
				m_bCanAtk = true;
				m_AtkCool = 0.0;
			}
		}
	}

	// ���� ������ �����϶��� ����
	if (m_bCanAtk == true)
	{
		if (m_pStateMachineCom->FindMonsterState() == false)
		{
			// � ���� ���� ���ϱ�
			while (m_eSkill == m_ePrevSkill)
			{
				if (m_pStatusCom->Get_HP_Ratio() > 0.8f)
				{
					// SummonCrow(0), Hug(1) ���� �� �ϳ�
					m_eSkill = (BIGTREE_SKILL)(rand() % 2);
				}
				else if (m_pStatusCom->Get_HP_Ratio() > 0.4f)
				{
					// Ground(2), Power(3), Screw(4) ���� �� �ϳ�
					m_eSkill = (BIGTREE_SKILL)(rand() % 3 + 2);
				}
				else if (m_pStatusCom->Get_HP_Ratio() > 0.3f)
				{
					// ���������� -> ���ϼ�ȯ ���� ��������
					if (m_ePrevSkill == BTSKILL_BUILDINGCLIMB)
						m_eSkill = BTSKILL_SUMMONFRUITS;
					else
						m_eSkill = BTSKILL_BUILDINGCLIMB;
				}
				else
				{
					// SummonCrow(0), Hug(1), Ground(2), Power(3), Screw(4) ���� �� �ϳ�
					m_eSkill = (BIGTREE_SKILL)(rand() % 5);
				}
			}

			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_BigTreeAtk::Create(&m_eSkill));

			m_ePrevSkill = m_eSkill;

			m_bCanAtk = false;
			m_AtkCool = 0.0;

			return;
		}
	}
	// �ƴϸ� ���̳��� ����
	else
	{
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_DynamicMove"), CState_MonsterDynamicMove::Create());
		return;
	}
}

CBigTree* CBigTree::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CBigTree* pInstance = new CBigTree(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CBigTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CBigTree::Clone(void* pArg)
{
	CBigTree* pInstance = new CBigTree(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CBigTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBigTree::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	for (_uint i = 0; i < 4; ++i)
		Safe_Release(m_pHitColliderCom[i]);
	for (_uint i = 0; i < 5; ++i)
		Safe_Release(m_pAtkColliderCom[i]);
}