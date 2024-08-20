#include "stdafx.h"
#include "Nightmare.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "MotionTrail.h"

// State
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h"
#include "State_MonsterAtk.h"
#include "State_MonsterChase.h" 
#include "State_MonsterRunAway.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"

CNightmare::CNightmare(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
}

CNightmare::CNightmare(const CNightmare& rhs)
	: CMonster(rhs)
{
}

HRESULT CNightmare::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CNightmare::NativeConstruct(void* pArg)
{
	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 17.5f;
	TransformDesc.fRotationPerSec = 3.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ���� Ÿ�� ����
	m_MonsterType = MONSTERTYPE_NIGHTMARE;

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
	m_iPass = 10;

	// ����ü ���� ����
	m_fFrustumRange = 15.f;

	// ���� ��ġ ����
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	m_bIsBoss = true;

	return S_OK;
}

HRESULT CNightmare::NativeConstruct_Pool(void * pArg)
{
	//	���� �Ŵ��� ���� ����Ʈ�� �߰�
	GM->Add_Monster(this);

	return S_OK;
}

_int CNightmare::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
	{
		m_pGameInstance->Release_LayerObject(LEVEL_TWILIGHT, TEXT("Layer_UI_BossTarget"));
		m_pGameInstance->Release_LayerObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Boss"));
		m_pGameInstance->Release_LayerObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Boss_Cnt"));
		m_pGameInstance->Release_LayerObject(LEVEL_TWILIGHT, TEXT("Layer_UI_Boss_CntBack"));

		return RESULT_DEAD;
	}

	// �ǰ� �ִϸ��̼� ��� ��Ÿ�� ó��
	m_OnDamagedAnimTimeAcc += TimeDelta;

	// ���� ���� ó��
	ChangeState(TimeDelta);

	// ������ ������ ����
	m_pTransformCom->Get_PositionFloat4_Pointer();

	// ���¸ӽ� Action ����
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// MonsterCooler ������Ʈ ������ ��Ÿ�� �ʱ�ȭ
	m_pMonsterCoolerCom->Update(TimeDelta);

	// �� ������Ʈ
	if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_TEST || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
	else
		m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);

	// �˹� ������Ʈ
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom);

	// ���Ʈ���� �ð�
	m_MotionTrailInterval += TimeDelta;

	// �ݶ��̴��� ��ġ ���
	Collider_Transform_Update();

	// �ݶ��̴� ��ġ ������Ʈ
	// �ǰ� �ݶ��̴�
	m_pHitColliderCom->Update_Transform(m_HitCollider_TransformMatrix);
	// ���� �ݶ��̴�
	for (_uint i = 0; i < 4; ++i)
		m_pAtkColliderCom[i]->Update_Transform(m_AtkCollider_TransformMatrix[i]);

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	// ���� �ݶ��̴��� ���� ������ ���� ���
	if (m_bReadyToDie == false)
		m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 4; ++i)
			m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom[i]);
	}

	return RESULT_NOPROBLEM;
}

_int CNightmare::LateTick(_double TimeDelta)
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
	m_pRendererCom->Add_Debug(m_pHitColliderCom);
	// ���� �ݶ��̴�
	if (m_bIsAttacking == true)
	{
		for (_uint i = 0; i < 4; ++i)
			m_pRendererCom->Add_Debug(m_pAtkColliderCom[i]);
	}
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CNightmare::Render()
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
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices", &MeshDescVector);
		else
			m_pModelCom->Render(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	// ��� Ʈ���� ��ü ����
	if (true == m_bUseMotionTrail)
	{
		if (m_MotionTrailInterval >= 0.03)
		{
			CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
			tMotionTrailDesc.iSize = 280;	// ����Ʈ�������� (����)
			tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// ����Ʈ���� ������
			tMotionTrailDesc.vColor = _float4(1.f, 1.f, 0.7f, 0.5f);	// ���ϴ� ��� Ʈ���� ����
			tMotionTrailDesc.fRimPower = 1.f;	// ������Ʈ �Ŀ� (�������� �β���)
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

	return S_OK;
}

HRESULT CNightmare::Render_Shadow()
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

void CNightmare::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// �ִϸ��̼� �׽�Ʈ
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// ������� �׽�Ʈ
	pImGui_Manager->SetGameObjectPosition(string("NM Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("NM Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("NM Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// ���̴� �׽�Ʈ
	pImGui_Manager->SetGameObjectShaderPass(string("NM Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// ���� üũ
	pImGui_Manager->CheckMonsterState(string("NM State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	// Dead ó��
	if (ImGui::Button("DEAD"))
		m_bDead = true;

	ImGui::End();
}

HRESULT CNightmare::Save_ObjectData(HANDLE hFile)
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

HRESULT CNightmare::Load_ObjectData(HANDLE hFile)
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

_bool CNightmare::Damaged(CCollider * pAtkCollider, _float fDamaged)
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

	// ���� �ʾҰ�, m_OnDamagedAnimTimeAcc �� 5.0 �̻��̸� OnDamaged ���·� ������ ������
	if (m_bReadyToDie == false && m_OnDamagedAnimTimeAcc >= 5.0)
	{
		// ���� ���̰ų� ���� ���̸� �н�
		if (m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_Atk") ||
			m_pStateMachineCom->Get_StateName(CStateMachine::STATETYPE_MONSTERSTATE) == string("State_RunAway"))
			return false;

		// ���� ��Ÿ�� �ʱ�ȭ
		m_pMonsterCoolerCom->Reset_Cool(CMonsterCooler::COOLTYPE_ATK);

		// �ǰ� ���� ��� (�յ�)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_2Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 12;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 13;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);

		// ���� �ð� ���� OnDamaged ���� ���� �ʵ��� ��Ÿ�� �ʱ�ȭ
		m_OnDamagedAnimTimeAcc = 0.0;
	}
	// �׾��ٸ� Dead ���·� ������ ������
	else if(m_bReadyToDie == true)
	{
		_uint iAnimIndex = 14;

		m_pTransformCom->LookAtXZ(pDestTransfrom);

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

void CNightmare::Ready_Collider_Info() // �ݶ��̴��� ���� �� ������, �ǹ���Ʈ���� ���� �ʱ�ȭ
{
	// �ǰ� �ݶ��̴�
	m_HitCollider_BoneMatrixPtr      = m_pModelCom->Get_BoneMatrixPtr("center");
	m_HitCollider_SocketPivotmMatrix = m_pModelCom->Get_TransformMatrix();

	// ���� �ݶ��̴�
	const char* AtkBones[4] = { "L_ashi2", "R_ashi2", "L_te", "R_te" };
	for (_uint i = 0; i < 4; ++i)
	{
		m_AtkCollider_BoneMatrixPtr[i] = m_pModelCom->Get_BoneMatrixPtr(AtkBones[i]);
		m_AtkCollider_SocketPivotmMatrix[i] = m_pModelCom->Get_TransformMatrix();
	}
}

void CNightmare::Collider_Transform_Update() // �ݶ��̴� ��ġ ���
{
	// �ǰ� �ݶ��̴�
	m_HitCollider_TransformMatrix =
		XMLoadFloat4x4(m_HitCollider_BoneMatrixPtr.pCombinedMatrix)
		* XMLoadFloat4x4(&m_HitCollider_SocketPivotmMatrix)
		* m_pTransformCom->Get_WorldMatrix();

	// ���� �ݶ��̴�
	for (_uint i = 0; i < 4; ++i)
	{
		m_AtkCollider_TransformMatrix[i] =
			XMLoadFloat4x4(m_AtkCollider_BoneMatrixPtr[i].pCombinedMatrix)
			* XMLoadFloat4x4(&m_AtkCollider_SocketPivotmMatrix[i])
			* m_pTransformCom->Get_WorldMatrix();
	}
}

HRESULT CNightmare::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = 200.f;
	StatusDesc.fHP      = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel   = 1;
	StatusDesc.fEXP     = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_MonsterCooler */
	CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
	MonsterCoolerDesc.MaxAttackCool  = 4.5;
	MonsterCoolerDesc.MaxChaseCool   = 7.0;
	MonsterCoolerDesc.ChaseCool      = 7.0;
	MonsterCoolerDesc.MaxRunAwayCool = 9.0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	{
		CCollider::COLLIDERDESC ColliderDesc;
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

		// �ǰ� �ݶ��̴�
		ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
		ColliderDesc.vScale       = _float3(1.5f, 1.5f, 1.5f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, -0.5f, 0.f, 1.f);
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Hit"), (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
			return E_FAIL;

		// ���� �ݶ��̴�
		ColliderDesc.vScale       = _float3(2.f, 2.f, 2.f);
		ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
		ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
		_tchar* pAtkComponentTag[4] = { TEXT("Com_Collider_L_ashi2"), TEXT("Com_Collider_R_ashi2"), TEXT("Com_Collider_L_te"), TEXT("Com_Collider_R_te") };
		for (_int i = 0; i < 2; ++i)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), pAtkComponentTag[i], (CComponent**)&m_pAtkColliderCom[i], &ColliderDesc)))
				return E_FAIL;
		}

		ColliderDesc.vScale = _float3(1.f, 1.f, 1.f);
		for (_int i = 2; i < 4; ++i)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), pAtkComponentTag[i], (CComponent**)&m_pAtkColliderCom[i], &ColliderDesc)))
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
		if (FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Navigation_Twilight_Boss"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_TWILIGHT, TEXT("Prototype_Component_Model_Nightmare"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNightmare::SetUp_RootNodeIndex()
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

HRESULT CNightmare::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Rmv_OnceList(1);
	m_pModelCom->Rmv_OnceList(2);
	m_pModelCom->Rmv_OnceList(5);

	m_pModelCom->Add_LerpListAll();

	return S_OK;
}

HRESULT CNightmare::SetUp_ConstantTable()
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

void CNightmare::OnCollision(_double TimeDelta)
{
	// �ױ� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true)
		return;

	// �÷��̾� �о��
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		if (m_pGameInstance->Collision_Sphere(m_pHitColliderCom, pDestCollider))
		{
			CGameObject* pDestObject = pDestCollider->Get_Parent();
			CTransform* pDestTransform = pDestObject->Get_Transform();
			CCollider* pDestCollider = pDestObject->Get_HitCollider();

			// ������
			_float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;
			_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;

			// ����
			XMFLOAT3 sourcenterXM3 = m_pHitColliderCom->Get_Sphere()->Center;
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

void CNightmare::ChangeState(_double TimeDelta)
{
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


	// 1. ����
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) <= 5.f)
	{
		if (m_pStateMachineCom->FindMonsterState() == false)
		{
			// ��Ÿ���� �Ǵ� ��쿡�� RunAway ���� �����ϰ� ��Ÿ�� �ʱ�ȭ
			if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_RUNAWAY) == true)
			{
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_RunAway"), CState_MonsterRunAway::Create());
				m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_RUNAWAY);
				return;
			}
		}
	}


	// 2. ����
	if (m_pTransformCom->Get_Distance(m_pPlayerTransform) > 30.f)
	{
		if (m_pStateMachineCom->FindMonsterState() == false)
		{
			// ��Ÿ���� �Ǵ� ��쿡�� Chase ���� �����ϰ� ��Ÿ�� �ʱ�ȭ
			if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_CHASE) == true)
			{
				m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Chase"), CState_MonsterChase::Create());
				m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_CHASE);
				return;
			}
		}
	}


	// 3. ���� 
	if (m_pStateMachineCom->FindMonsterState() == false)
	{
		// ��Ÿ���� �Ǵ� ��쿡�� Atk ���� �����ϰ� ��Ÿ�� �ʱ�ȭ
		if (m_pMonsterCoolerCom->Get_IsStatePossible(CMonsterCooler::COOLTYPE_ATK) == true)
		{
			m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Atk"), CState_MonsterAtk::Create());
			m_pMonsterCoolerCom->Cool_State(CMonsterCooler::COOLTYPE_ATK);
			return;
		}
	}
}

CNightmare* CNightmare::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CNightmare* pInstance = new CNightmare(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CNightmare");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CNightmare::Clone(void* pArg)
{
	CNightmare* pInstance = new CNightmare(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CNightmare");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CNightmare::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pMonsterCoolerCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pHitColliderCom);
	for (_uint i = 0; i < 4; ++i)
		Safe_Release(m_pAtkColliderCom[i]);
}