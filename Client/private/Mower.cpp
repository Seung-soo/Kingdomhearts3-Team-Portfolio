#include "stdafx.h"
#include "Mower.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "ImGui_Manager.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "UI_MiniMap_Monster.h"

// State
#include "State_MonsterSpawn.h"
#include "State_MonsterIdle.h"
#include "State_MonsterAtk.h"
#include "State_MonsterOnDamaged.h"
#include "State_MonsterDead.h"

CMower::CMower(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CMonster(pDevice, pDeviceContext)
{
} 

CMower::CMower(const CMower& rhs)
	: CMonster(rhs)
{
}

HRESULT CMower::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMower::NativeConstruct(void* pArg)
{
	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 3.5f;
	TransformDesc.fRotationPerSec = 3.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ���� Ÿ�� ����
	m_MonsterType = MONSTERTYPE_MOWER;

	// ������Ʈ �ʱ�ȭ
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ��Ʈ����ε��� �ʱ�ȭ
	if (FAILED(SetUp_RootNodeIndex()))
		return E_FAIL;

	// �ִϸ��̼� ����
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// ���� ���� ���� (Idle)
	m_pStateMachineCom->BindState(CStateMachine::STATETYPE_IDLE, string("State_Idle"), CState_MonsterIdle::Create());

	// ���̴��н� ����
	m_iPass = 10;

	// ���� ��ġ ����
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	return S_OK;
}

HRESULT CMower::NativeConstruct_Pool(void * pArg)
{
	//	���� �Ŵ��� ���� ����Ʈ�� �߰�
	GM->Add_Monster(this);

	return S_OK;
}

_int CMower::Tick(_double TimeDelta)
{
	TimeDelta = TimeDelta * m_pStatusCom->Get_SlowSpeed();

	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
	{
		if (nullptr != m_pMinimapIcon)
		{
			m_pMinimapIcon->Set_Dead(true);
			m_pMinimapIcon = nullptr;
		}

		return RESULT_DEAD;
	}

	// ���� ���� ó��
	ChangeState(TimeDelta);

	// �������ͽ� ������Ʈ
	m_pStatusCom->Update(TimeDelta);

	// ���¸ӽ� Action ����
	if (m_pStatusCom->Get_Stun() == false)
	{
		if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
		{
			BREAKPOINT;
			return RESULT_ERROR;
		}
	}

	// MonsterCooler ������Ʈ ������ ��Ÿ�� �ʱ�ȭ
	if (m_pStatusCom->Get_Stun() == false)
	{
		m_pMonsterCoolerCom->Update(TimeDelta);
	}

	// �� ������Ʈ
	if (m_pStatusCom->Get_Stun() == false)
	{
		if (m_pNavigationCom->Get_Ignore() == true || m_pGameInstance->Get_LevelIndex() == LEVEL_TEST || m_pGameInstance->Get_LevelIndex() == LEVEL_MAPTOOL)
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);
		else
			m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom, m_pNavigationCom);
	}

	// �˹� ������Ʈ
	if (0.0 < m_pTransformCom->Get_KnockBackTime())
		m_pTransformCom->KnockBackTick(TimeDelta, m_pNavigationCom);

	// �ݶ��̴� ��ġ ������Ʈ
	m_pHitColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());
	m_pAtkColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	if (m_bReadyToDie == false)
		m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pHitColliderCom);
	// ���� �ݶ��̴��� ���� ������ ���� ���
	if (m_bIsAttacking == true)
		m_pGameInstance->Add_Collider(string("Monster_Atk"), m_pAtkColliderCom);

	/* MiniMap */
	CUI_MiniMap_Monster::MONSTERPOINTER				tMonsterPointer;
	tMonsterPointer.fMonsterPos = m_pTransformCom->Get_PositionFloat4_Pointer();
	tMonsterPointer.bMonsterDead = &m_bDead;

	if (false == bCreate)
	{
		m_pMinimapIcon = m_pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_UI"), TEXT("Prototype_GameObject_MiniMap_Monster"), &tMonsterPointer);
		bCreate = true;
	}

	return RESULT_NOPROBLEM;
}

_int CMower::LateTick(_double TimeDelta)
{
	TimeDelta = TimeDelta * m_pStatusCom->Get_SlowSpeed();

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
	m_pRendererCom->Add_Debug(m_pHitColliderCom);
	if (m_bIsAttacking == true)
		m_pRendererCom->Add_Debug(m_pAtkColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CMower::Render()
{
	if (nullptr == m_pModelCom)
		return E_FAIL;

	// ���̴� ���� ����� �� ����
	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	// ���� �����ִ� ���׸��� �� �����ͼ� ���̴��� �ؽ�ó ���� ����� �� �����ϰ� ������
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

HRESULT CMower::Render_Shadow()
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

void CMower::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// �ִϸ��̼� �׽�Ʈ
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// ������� �׽�Ʈ
	pImGui_Manager->SetGameObjectPosition(string("MW Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("MW Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("MW Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// ���̴� �׽�Ʈ
	pImGui_Manager->SetGameObjectShaderPass(string("MW Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// ���� üũ
	pImGui_Manager->CheckMonsterState(string("MW State"), m_pStateMachineCom);
	pImGui_Manager->SmallSeparator();

	ImGui::End();
}

HRESULT CMower::Save_ObjectData(HANDLE hFile)
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

HRESULT CMower::Load_ObjectData(HANDLE hFile)
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

_bool CMower::Damaged(CCollider * pAtkCollider, _float fDamaged)
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

	// ���� �ʾҴٸ� OnDamaged ���·� ������ ������
	if (m_bReadyToDie == false)
	{
		// ���� ��Ÿ�� �ʱ�ȭ
		m_pMonsterCoolerCom->Reset_Cool(CMonsterCooler::COOLTYPE_ATK);

		// �ǰ� ���� ��� (�յ��¿�)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_4Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 4;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 5;
		else if (DamagedDir == CTransform::DAMAGED_LEFT)
			iAnimIndex = 6;
		else
			iAnimIndex = 7;

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_OnDamaged"), CState_MonsterOnDamaged::Create(&iAnimIndex), true);
	}
	// �׾��ٸ� Dead ���·� ������ ������
	else
	{
		// �ǰ� ���� ��� (�յ�)
		CTransform::DAMAGEDDIR DamagedDir = m_pTransformCom->Get_Damaged_4Dir(pDestTransfrom);
		_uint iAnimIndex = 0;

		if (DamagedDir == CTransform::DAMAGED_FRONT)
			iAnimIndex = 10;
		else if (DamagedDir == CTransform::DAMAGED_BACK)
			iAnimIndex = 11;
		else if (DamagedDir == CTransform::DAMAGED_LEFT)
		{
			m_pTransformCom->LookAtXZ(pDestTransfrom);
			iAnimIndex = 10;
		}
		else
		{
			m_pTransformCom->LookAtXZ(pDestTransfrom);
			iAnimIndex = 10;
		}

		m_pStateMachineCom->BindState(CStateMachine::STATETYPE_MONSTERSTATE, string("State_Dead"), CState_MonsterDead::Create(&iAnimIndex), true);
	}

	return true;
}

HRESULT CMower::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP = 25.f;
	StatusDesc.fHP = StatusDesc.fMaxHP;
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel = 1;
	StatusDesc.fEXP = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_MonsterCooler */
	CMonsterCooler::MONSTERCOOLERDESC MonsterCoolerDesc;
	MonsterCoolerDesc.MaxAttackCool = 1.0;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_MonsterCooler"), TEXT("Com_MonsterCooler"), (CComponent**)&m_pMonsterCoolerCom, &MonsterCoolerDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(2.f, 2.f, 2.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Hit"), (CComponent**)&m_pHitColliderCom, &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vScale = _float3(4.f, 4.f, 4.f);
	ColliderDesc.vTranslation = _float4(0.f, 1.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider_Atk"), (CComponent**)&m_pAtkColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
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
		if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Navigation_Rapunzel"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &tNaviDesc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	m_pNavigationCom->Find_Cell(m_pTransformCom->Get_Position());

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_RAPUNZEL, TEXT("Prototype_Component_Model_Mower"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMower::SetUp_RootNodeIndex()
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

HRESULT CMower::SetUp_Animations()
{
	m_pModelCom->Add_OnceListAll();

	m_pModelCom->Rmv_OnceList(1);

	for (_uint i = 0; i < 4; ++i)
		m_pModelCom->Add_LerpList(i);

	return S_OK;
}

HRESULT CMower::SetUp_ConstantTable()
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

	return S_OK;
}

void CMower::OnCollision(_double TimeDelta)
{
	// �ױ� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true)
		return;

	// ���ͳ��� �о��
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

			_float fDestColliderRadius = pDestCollider->Get_Sphere()->Radius;
			_float fSourColliderRadius = m_pHitColliderCom->Get_Sphere()->Radius;

			// ��ģ�κ� ũ��
			_float fCollisionSize = fDestColliderRadius + fSourColliderRadius - m_pTransformCom->Get_Distance(pDestTransform);

			// ������
			_vector vDir = m_pTransformCom->Get_Dir(pDestTransform);

			// ��ġ�κи�ŭ �з�����
			_vector vPos = m_pTransformCom->Get_Position();
			m_pTransformCom->Set_Position(vPos - vDir * fCollisionSize);
		}
	}
}

void CMower::ChangeState(_double TimeDelta)
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


	// 1. ����
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

CMower* CMower::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMower* pInstance = new CMower(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CMower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMower::Clone(void* pArg)
{
	CMower* pInstance = new CMower(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CMower");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMower::Free()
{
	__super::Free();

	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pMonsterCoolerCom);
	Safe_Release(m_pStatusCom);
	Safe_Release(m_pHitColliderCom);
	Safe_Release(m_pAtkColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}