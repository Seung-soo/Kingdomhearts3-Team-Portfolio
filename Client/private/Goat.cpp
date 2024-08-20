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
	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 17.5f;
	TransformDesc.fRotationPerSec = 5.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ���� Ÿ�� ����
	m_MonsterType = MONSTERTYPE_GOAT;

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

	// ���� ��ġ ����
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

	// ������ RESULT_DEAD ��ȯ (������Ʈ �Ŵ������� ����)
	if (true == m_bDead)
	{
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_BossTarget"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_Cnt"));
		m_pGameInstance->Release_LayerObject(GM->Get_CurrentLevel(), TEXT("Layer_UI_Boss_CntBack"));

		return RESULT_DEAD;
	}

	// ���� ���� ó��
	ChangeState(TimeDelta);

	// ���¸ӽ� Action ����
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// �˹� ó��
	m_pTransformCom->Set_KnockBackInputPower(2.f);
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta * 0.1, m_pNavigationCom);

	// �ڷ���Ʈ ������Ʈ
	TeleportDissolveUpdate(TimeDelta);

	// �ǰ�ī��Ʈ �ʱ�ȭ (10��)
	if (m_iDamagedCountAcc != 0)
	{
		m_DamagedTimeAcc += TimeDelta;

		if (m_DamagedTimeAcc >= 10.0)
		{
			m_iDamagedCountAcc = 0;
			m_DamagedTimeAcc   = 0.0;
		}
	}

	// ���� ��Ÿ�� �ʱ�ȭ
	if (m_bCanAttack == false)
	{
		m_AttackCool += TimeDelta;

		if (m_AttackCool >= m_MaxAttackCool)
		{
			m_bCanAttack = true;
			m_AttackCool = 0.0;
		}
	}

	// ���Ʈ���� ���͹� �ð� ����
	m_MotionTrailInterval += TimeDelta;

	// �׺���̼Ǻ��� �Ʒ��� �������� �ʰ� �ϱ�
	_vector vPos = m_pTransformCom->Get_Position();
	_float fMinY = m_pNavigationCom->Compute_Y(vPos);
	if (vecy(vPos) < fMinY)
		m_pTransformCom->Set_Position(XMVectorSetY(vPos, fMinY));

	// �� ������Ʈ
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * 1.2, m_pTransformCom);
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta * 1.2, m_pTransformCom, m_pNavigationCom);

	// �˹� ������Ʈ
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom);

	// �ݶ��̴��� ��ġ ���
	Collider_Transform_Update();

	// �ǰ� �ݶ��̴� ��ġ ������Ʈ
	m_pHitColliderCom->Update_Transform(m_HitCollider_TransformMatrix);

	// �ݸ����Ŵ����� �ݶ��̴� ���
	m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	
	return RESULT_NOPROBLEM;
}

_int CGoat::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// �÷��̾ ������ �ٸ� ��ü���� �浹 �˻�
	OnCollision(TimeDelta);

	// ����ü �ȿ� �ִٸ� + m_bDead ���°� �ƴϸ� �����׷쿡 �߰�
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange) && false == m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// �ݶ��̴� ����� ������
	// �ǰ� �ݶ��̴�
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

	// ���̴� ���� ����� �� ����
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	/* ��� Ʈ���� ����� �� m_bUseMotionTrail ������ true�� �� �� false��*/
	// ���Ʈ���Ͽ� ���̴� ����Ʈ���� ���� ���� �����̳�
	vector<CModel::MESHDESC*> MeshDescVector;

	// ���� �����ִ� ���׸��� �� �����ͼ� ���̴��� �ؽ�ó ���� ����� �� �����ϰ� ������
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

	// ��� Ʈ���� ��ü ����
	if (true == m_bUseMotionTrail)
	{
		if (m_MotionTrailInterval >= 0.015)
		{
			CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
			tMotionTrailDesc.iSize = 450;	// ����Ʈ�������� (����)
			tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// ����Ʈ���� ������
			tMotionTrailDesc.vColor = _float4(0.5f, 0.4f, 0.4f, 0.3f);	// ���ϴ� ��� Ʈ���� ����
			tMotionTrailDesc.fRimPower = 0.1f;	// ������Ʈ �Ŀ� (�������� �β���)
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

	// �ִϸ��̼� �׽�Ʈ
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// ������� �׽�Ʈ
	pImGui_Manager->SetGameObjectPosition(string("GO Pos"), m_pTransformCom);
	static _bool bPosZero = false;
	ImGui::Checkbox("Position Zero", &bPosZero);
	if (bPosZero)
		m_pTransformCom->Set_Position(__vector(0.f, 0.f, 0.f, 1.f));

	pImGui_Manager->SmallSeparator();

	// ���̴� �׽�Ʈ
	pImGui_Manager->SetGameObjectShaderPass(string("GO Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// ���� üũ
	ImGui::Text("AnimIndex : %d", m_pModelCom->Get_CurrAnimIndex());
	pImGui_Manager->CheckMonsterState(string("GO State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	// ���� �׽�Ʈ
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

	// ��Ÿ ���� �׽�Ʈ
	if (ImGui::Button("Spawn"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create(), true);
	ImGui::SameLine();
	if (ImGui::Button("RunAway"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create(), true);
	ImGui::SameLine();
	if (ImGui::Button("DynamicMove"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_DynamicMove"), CState_MonsterDynamicMove::Create(), true);
	pImGui_Manager->SmallSeparator();

	// ��������
	if (ImGui::Button("Remove State"))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);
	ImGui::SameLine();

	// Idle ���� ����
	if (ImGui::Button("Remove Idle"))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_IDLE);
	ImGui::SameLine();
	// Idle ���� ����
	if (ImGui::Button("Create Idle"))
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());
	pImGui_Manager->SmallSeparator();

	// �÷��̾���� �Ÿ�
	ImGui::Text("Distance From Player : %.1f", m_pTransformCom->Get_Distance(m_pGameInstance->Get_GameObject(m_pGameInstance->Get_LevelIndex(), TEXT("Layer_Player"), 0)->Get_Transform()));

	// OnlyIdle ���·� ����
	if (ImGui::Checkbox("ONLYIDLE", &m_bOnlyIdle))
		m_pStateMachineCom->ClearState(CStateMachine::STATETYPE_MONSTERSTATE);

	pImGui_Manager->SmallSeparator();

	// Look ����
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_Look());
	ImGui::Text("Look Direction : (%.1f, %.1f, %.1f)", vecx(vLook), vecy(vLook), vecz(vLook));

	// ���� ü��
	ImGui::Text("HP : %.0f / %.0f (%.0f %%)", m_pStatusCom->Get_HP(), m_pStatusCom->Get_MaxHP(), m_pStatusCom->Get_HP_Ratio() * 100.f);

	// �ǰ� Ƚ��
	ImGui::Text("DamagedCountAcc : %d", m_iDamagedCountAcc);
	pImGui_Manager->SmallSeparator();

	// ������ ����
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

	// Dead ó��
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

	// ������� ����
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

	// ���̺굥���Ϳ��� ��������� �ε�
	DWORD		dwBytes = 0;
	_float4x4	worldMatrix;

	ReadFile(hFile, &worldMatrix, sizeof(_float4x4), &dwBytes, nullptr);

	// ������� ����
	m_pTransformCom->Set_WorldMatrix(worldMatrix);

	return S_OK;
}

_bool CGoat::Damaged(CCollider* pAtkCollider, _float fDamaged)
{
	// �ױ� ���� �����̰ų� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true || m_bDead == true)
		return false;

	// �ǰ� ī��Ʈ ����
	++m_iDamagedCountAcc;

	// pAtkCollider �� �θ�(�ÿ��̾� ���� ��ü)�� ���ͼ� Ư�� ����� �����ų �� ����
	CGameObject* pDestObject = pAtkCollider->Get_Parent();

	// ������ ����
	if (false == m_pStatusCom->Damaged(fDamaged))
		return false;

	// ��� ���⿡�� ������ �޾Ҵ��� ���
	CTransform* pDestTransfrom = pDestObject->Get_Transform();

	// ���� ������ OnDamaged ��
	if (m_bReadyToDie == false)
	{
		// ���� ���̰ų� �˹�����̰ų� ���� ���̰ų� �̹� OnDamaged �����̸� �н�
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_RunAway"))
			return false;

		// �ǰ� ���� ��� (�յ�)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		// �ǰ� ���⿡ �´� �ִϸ��̼� ����
		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 14;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 15;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);
	}
	// �׾��ٸ� Dead ���·� ������ ������
	else if (m_bReadyToDie == true)
	{
		_uint iAnimIndex = 15;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CGoat::CreateWeapon()
{
	// �������
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
	// �ϴ� ��� �ִϸ��̼��� ���� �� ONCE Ÿ������ ����
	m_pModelCom->Add_OnceListAll();

	// LOOP Ÿ���� ���⼭ ����
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

	// �ְ� ���ε�
	if (FAILED(m_pShaderCom->Set_RawValue("g_fModelDistortion", &m_fDistortion, sizeof(_float))))
	{
		BREAKPOINT;
		return E_FAIL;
	}
	
	// ��Ǻ�
	if (FAILED(m_pTransformCom->Bind_OnOldShader(m_pShaderCom, "g_OldWorldMatrix")))
		return E_FAIL;

	if (m_iPass == 7)
	{
		// ������ �ؽ�ó ���ε�
		if (FAILED(m_pDissolveTexture->SetUp_OnShader(m_pShaderCom, "g_DissolveTexture", 4)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		
		// ������ Alpha ���ε�
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
	// �ױ� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true || m_bDead == true)
		return;
}

void CGoat::ChangeState(_double TimeDelta)
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

	// ������ (Idle ���·� ����)
	if (m_bOnlyIdle == true)
		return;

	// ����
	if (m_bSpawn == false && m_pStateMachineCom->FindMonsterState() == false)
	{
		// Spawn ���� ����
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Spawn"), CState_MonsterSpawn::Create());
		m_bSpawn = true;

		m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

		return;
	}

	// ==========================================================================================

	// ȸ�� �Ǵ� ���̳��͹��� (�����ð� ���� ���� Ƚ�� �ǰݵǸ� ȸ�� �Ǵ� ���̳��͹��� �ߵ��ϰ� �ʱ�ȭ)
	if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) != string("State_Dead") && m_iDamagedCountAcc >= 5)
	{
		if (rand() % 2 == 0)
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create(), true);
		else
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_DynamicMove"), CState_MonsterDynamicMove::Create(), true);

		m_iDamagedCountAcc = 0;
		m_DamagedTimeAcc   = 0.0;

		// ���� ���� ���·� �����
		m_bCanAttack = true;
		m_AttackCool = 0.0;
	}

	// ������ ���� ó�� (������ �Ѿ �� Ư�� ��ų ����)
	if (m_pStatusCom->Get_HP_Ratio() <= 0.75f && m_bPhaseChanged[0] == false)
	{
		// �� ��° �������
		m_iCurrentPhase = 1;
		m_bPhaseChanged[0] = true;

		// ���� ��Ÿ�� ����
		m_MaxAttackCool = 4.0;

		// �߾����� �̵�
		m_pTransformCom->Set_Position(__vector(0.f, 10.f, 0.f, 1.f));

		// ������� �ұ�� ��ų ����
		m_eGoatAtk[0]     = GOAT_ATK_GRAVITYPILLAROFFIRE;
		m_ePrevGoatAtk[0] = GOAT_ATK_GRAVITYPILLAROFFIRE;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[0]), true);

		// �÷��̾� ��ġ �̵�
		m_pPlayerTransform->Set_Position(__vector(20.f, 10.f, 20.f, 1.f));
		static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")))->Find_Cell(m_pPlayerTransform->Get_Position());

		m_pPlayerTransform->LookAtXZ(m_pTransformCom);

		// ���� ������ �ʱ�ȭ
		m_iNormalAtkAcc  = 0;
		m_iSpecialAtkAcc = 0;

		// ī�޶� �׼� �ֱ� ================================================================
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_GOAT_PILLARFIRE, m_pTransformCom);
	}
	else if (m_pStatusCom->Get_HP_Ratio() <= 0.5f && m_bPhaseChanged[1] == false)
	{
		// �� ��° �������
		m_iCurrentPhase = 2;
		m_bPhaseChanged[1] = true;

		// ���� ��Ÿ�� ����
		m_MaxAttackCool = 3.5;

		// �߾����� �̵�
		m_pTransformCom->Set_Position(__vector(0.f, 10.f, 0.f, 1.f));

		// �����ϴ� �ұ�� ��ų ����
		m_eGoatAtk[0]     = GOAT_ATK_PILLAROFFIRE;
		m_ePrevGoatAtk[0] = GOAT_ATK_PILLAROFFIRE;
		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[0]), true);

		// �÷��̾� ��ġ �̵�
		m_pPlayerTransform->Set_Position(__vector(0.01f, 10.f, 0.f, 1.f));
		static_cast<CNavigation*>(m_pPlayer->Get_Component(TEXT("Com_Navigation")))->Find_Cell(m_pPlayerTransform->Get_Position());
		m_pPlayerTransform->LookAtXZ(m_pTransformCom);

		GM->Get_Player()->Attacked_By_Teleport();

		// ���� ������ �ʱ�ȭ
		m_iNormalAtkAcc  = 0;
		m_iSpecialAtkAcc = 0;

		// ī�޶� �׼� �ֱ� ================================================================
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_GOAT_PILLARROT, m_pTransformCom);
	}

	// ���� (0������, 1������, 2������ ���� �ٸ���)
	// �̹� �ٸ� ���°� ������ �ǳʶٱ�, m_bCanAttack �� true �϶��� ���� ����
	_uint iAtkType = 0;
	if (m_pStateMachineCom->FindMonsterState() == false && m_bCanAttack == true)
	{
		if (m_iCurrentPhase == 0) // 0 ������ ========================================================
		{
			// �븻 ����
			// FlySwing, DashSwing, SpinDashSwing, JumpDashSwing, UnderSwing
			vector<GOAT_ATK> eNormalAtks { GOAT_ATK_FLYSWING, GOAT_ATK_DASHSWING, GOAT_ATK_SPINDASHSWING, GOAT_ATK_JUMPDASHSWING, GOAT_ATK_UNDERSWING };

			// ����Ȱ���
			// TeleportDashSwing, TeleportSpinDashSwing, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks { GOAT_ATK_TELEPORTDASHSWING, GOAT_ATK_TELEPORTSPINDASHSWING, GOAT_ATK_SHORTPILLAROFFIRE };

			// ���Ӱ���
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks { GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// ����Ȱ����� �ι� �ߴٸ� �������� ���Ӱ��� �ϳ��� ��
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// �븻���� ����Ƚ���� 3ȸ��� �������� ����� ������ ���� �븻���� ����Ƚ�� �ʱ�ȭ
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

				// ����Ȱ��� ����Ƚ�� ����
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// �븻���� ����Ƚ���� 3ȸ�� �ƴ϶�� �������� �븻���� �ϳ��� ��
			else if (m_iNormalAtkAcc < 3)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				if (m_eGoatAtk[0] == GOAT_ATK_JUMPDASHSWING)
				{
					// JumpDashSwing �� ��쿡 Goat �� �÷��̾�� ���� ��ġ�� �ƴ϶�� �׳� Spear ����
					if (m_pTransformCom->IsUnderTarget(m_pPlayerTransform, 3.5f) == false)
						m_eGoatAtk[0] = GOAT_ATK_SPEAR;
				}

				// �븻���� ����Ƚ�� ����
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// ���ݻ��� ���ε�
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[iAtkType]));
		}
		else if (m_iCurrentPhase == 1) // 1 ������ ===================================================
		{
			// �븻 ����
			// Spear, FlySwing, SpinDashSwing, UnderSwing
			vector<GOAT_ATK> eNormalAtks{ GOAT_ATK_SPEAR, GOAT_ATK_FLYSWING, GOAT_ATK_SPINDASHSWING, GOAT_ATK_UNDERSWING };

			// ����Ȱ���
			// TeleportSpear, TeleportUnderSwing, MagicShot, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks{ GOAT_ATK_TELEPORTSPEAR, GOAT_ATK_TELEPORTUNDERSWING, GOAT_ATK_MAGICSHOT, GOAT_ATK_SHORTPILLAROFFIRE };

			// ���Ӱ���
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks{ GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// ����Ȱ����� �ι� �ߴٸ� �������� ���Ӱ��� �ϳ��� ��
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// �븻���� ����Ƚ���� 2ȸ��� �������� ����� ������ ���� �븻���� ����Ƚ�� �ʱ�ȭ
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

				// ����Ȱ��� ����Ƚ�� ����
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// �븻���� ����Ƚ���� 2ȸ�� �ƴ϶�� �������� �븻���� �ϳ��� ��
			else if (m_iNormalAtkAcc < 2)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				// �븻���� ����Ƚ�� ����
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// ���ݻ��� ���ε�
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_GoatAtk::Create(&m_eGoatAtk[iAtkType]));
		}
		else // 2 ������ =============================================================================
		{
			// �븻 ����
			// Spear, UnderSwing
			vector<GOAT_ATK> eNormalAtks{ GOAT_ATK_SPEAR, GOAT_ATK_SPINDASHSWING };

			// ����Ȱ���
			// MagicShot, ShortPillarOfFire
			vector<GOAT_ATK> eSpecialAtks{ GOAT_ATK_MAGICSHOT, GOAT_ATK_SHORTPILLAROFFIRE };

			// ���Ӱ���
			// ConsecutiveSwing, ConsecutiveSpear
			vector<GOAT_ATK> eConsecutiveAtks{ GOAT_ATK_CONSECUTIVE_SWING, GOAT_ATK_CONSECUTIVE_SPEAR };

			// ����Ȱ����� �ι� �ߴٸ� �������� ���Ӱ��� �ϳ��� ��
			if (m_iSpecialAtkAcc >= 2)
			{
				while (m_eGoatAtk[2] == m_ePrevGoatAtk[2])
					m_eGoatAtk[2] = eConsecutiveAtks[rand() % eConsecutiveAtks.size()];

				m_iSpecialAtkAcc = 0;
				iAtkType = 2;
			}
			// �븻���� ����Ƚ���� 2ȸ��� �������� ����� ������ ���� �븻���� ����Ƚ�� �ʱ�ȭ
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

				// ����Ȱ��� ����Ƚ�� ����
				++m_iSpecialAtkAcc;
				iAtkType = 1;
			}
			// �븻���� ����Ƚ���� 2ȸ�� �ƴ϶�� �������� �븻���� �ϳ��� ��
			else if (m_iNormalAtkAcc < 2)
			{
				while (m_eGoatAtk[0] == m_ePrevGoatAtk[0])
					m_eGoatAtk[0] = eNormalAtks[rand() % eNormalAtks.size()];

				// �븻���� ����Ƚ�� ����
				++m_iNormalAtkAcc;
				iAtkType = 0;
			}

			// ���ݻ��� ���ε�
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
		if (true == m_bDissolvePlus) // ����� ��
		{
			m_fDissolveAlpha += (_float)TimeDelta * 5.f;
			if (1.f < m_fDissolveAlpha)
			{
				m_fDissolveAlpha = 1.f;
				m_bDissolvePlus  = false;
				m_bDissolve      = false;
				m_iPass          = 0;
				m_fDistortion    = 0.f;

				// ���� �� ������� ������ (������) �ٽ� �����ϰ� �ڷ���Ʈ�ϱ�
				StartTeleportDissolve();
				m_pTransformCom->TeleportActivate(m_pNavigationCom);

				// �ڷ���Ʈ����
				if (rand() % 2 == 0)
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_58_Teleport.wav"), 0.6f, true, m_pTransformCom->Get_Position());
				else
					CGameInstance::GetInstance()->LetsPlaySound_Monster(TEXT("Goat_62_Teleport.wav"), 0.6f, true, m_pTransformCom->Get_Position());

				// �÷��̾ �Ĵٺ��� �ϱ�(Ư�� �ִϸ��̼��� xz�θ� �Ĵٺ���)
				if (m_pModelCom->Get_CurrAnimIndex() == 35 || m_pModelCom->Get_CurrAnimIndex() == 1)
					m_pTransformCom->LookAtXZ(m_pPlayerTransform);
				else
					m_pTransformCom->LookAt(m_pPlayerTransform);
			}
		}
		else // �ٽ� ��Ÿ�� ��
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