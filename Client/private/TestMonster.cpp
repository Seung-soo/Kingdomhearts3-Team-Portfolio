#include "stdafx.h"
#include "..\Public\TestMonster.h"
#include "GameInstance.h"
#include "ImGui_Manager.h"

// State
#include "State_MonsterIdle.h"

CTestMonster::CTestMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTestMonster::CTestMonster(const CTestMonster & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTestMonster::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestMonster::NativeConstruct(void * pArg)
{
	// �θ� �ʱ�ȭ�� �� Ʈ������ �ʱ�ȭ
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 10.f;
	TransformDesc.fRotationPerSec = 15.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	// ������Ʈ �ʱ�ȭ
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ��Ʈ����ε��� �ʱ�ȭ
	if (FAILED(SetUp_RootNodeIndex()))
		return E_FAIL;

	// �ִϸ��̼� ����
	if (FAILED(SetUp_Animations()))
		return E_FAIL;

	// ���̴��н� ����
	m_iPass = 0;

	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	// ImGui ��Ʈ��
	m_bImGuiController = true;

	return S_OK;
}

_int CTestMonster::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ������ RESULT_DEAD ��ȯ
	if (true == m_bDead)
		return RESULT_DEAD;

	// Űüũ
	if (m_pGameInstance->Get_LevelIndex() == LEVEL_TEST)
		KeyInput(TimeDelta);

	// ���� ���� ó��
	ChangeState(TimeDelta);

	// ���¸ӽ� Action ����
	if (FAILED(m_pStateMachineCom->Action(TimeDelta)))
	{
		BREAKPOINT;
		return RESULT_ERROR;
	}

	// �� ������Ʈ
	m_pModelCom->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

	if (m_bPositionLock)
		m_pTransformCom->Set_Position(__vector(0.f, 0.f, 0.f, 1.f));

	// �ݶ��̴� ��ġ ������Ʈ
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	m_pGameInstance->Add_Collider(string("Monster_Hit"), m_pColliderCom);

	return RESULT_NOPROBLEM;
}

_int CTestMonster::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	// �浹 �˻� (LateTick ���� ������ ��)
	OnCollision(TimeDelta);

	// ����ü �ȿ� �ִٸ� �����׷쿡 �߰�
	if (TRUE == m_pGameInstance->isIn_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRange))
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
	// �ݶ��̴� ����� ������
	m_pRendererCom->Add_Debug(m_pColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CTestMonster::Render()
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
		m_pModelCom->Render_Heavy(m_pShaderCom, m_iPass, i, "g_BoneMatrices");
	}

	return S_OK;
}

void CTestMonster::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Object Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	// �ִϸ��̼� �׽�Ʈ
	pImGui_Manager->SetGameObjectAnimation(m_pModelCom);
	pImGui_Manager->SmallSeparator();

	// ������ ��
	ImGui::Checkbox("POS LOCK", &m_bPositionLock);

	// ������� �׽�Ʈ
	pImGui_Manager->SetGameObjectPosition(string("TM Pos"), m_pTransformCom);
	pImGui_Manager->SetGameObjectRotation(string("TM Rot"), m_pTransformCom);
	pImGui_Manager->SetGameObjectScale(string("TM Scale"), m_pTransformCom);
	pImGui_Manager->SmallSeparator();

	// �ݶ��̴� �׽�Ʈ
	/*pImGui_Manager->SetGameObjectCollider(string("TM Collider"), m_pColliderCom);
	pImGui_Manager->SmallSeparator();*/

	// ���̴� �׽�Ʈ
	pImGui_Manager->SetGameObjectShaderPass(string("TM Shader"), m_pShaderCom, (_int*)(&m_iPass));
	pImGui_Manager->SmallSeparator();

	// �� �ٲٱ�
	ImGui::Combo("Select Model", &m_SelectMonster, m_Monsters, IM_ARRAYSIZE(m_Monsters));
	if (ImGui::Button("Change"))
	{
		if (m_SelectMonster != -1)
		{
			char ModelTag[128]  = "Prototype_Component_Model_";
			const char* Monster = m_Monsters[m_SelectMonster];
			strcat_s(ModelTag, Monster);

			_bstr_t wModelTag(ModelTag);
			Set_Model(wModelTag);
		}
	}

	ImGui::End();
}

HRESULT CTestMonster::Save_ObjectData(HANDLE hFile)
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

HRESULT CTestMonster::Load_ObjectData(HANDLE hFile)
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

HRESULT CTestMonster::SetUp_Components()
{
	/* For.Com_StateMachine */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"), TEXT("Com_StateMachine"), (CComponent**)&m_pStateMachineCom)))
		return E_FAIL;

	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP         = 100.f;
	StatusDesc.fHP            = StatusDesc.fMaxHP;
	StatusDesc.fDamaged       = 5.f;
	StatusDesc.iLevel         = 1;
	StatusDesc.fEXP           = 0.f;

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
	ColliderDesc.vTranslation = _float4(0.f, 0.5f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimHeavyModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_TestMonster"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestMonster::SetUp_RootNodeIndex()
{
	_int iRootNodeIndex = m_pModelCom->Get_NodeIndexByName("kh_sk");;
	if (UINT_MAX == iRootNodeIndex)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	m_pModelCom->Set_RootNodeIndex(iRootNodeIndex);

	return S_OK;
}

HRESULT CTestMonster::SetUp_Animations()
{
	/*m_pModelCom->Add_OnceList(0);
	m_pModelCom->Add_OnceList(1);
	m_pModelCom->Add_OnceList(2);*/

	m_pModelCom->Add_OnceListAll();
	m_pModelCom->Add_LerpListAll();

	return S_OK;
}

HRESULT CTestMonster::SetUp_ConstantTable()
{
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

void CTestMonster::KeyInput(_double TimeDelta)
{
	if (m_pGameInstance->Mouse_Pressing(CInput_Device::MBS_RBUTTON) == false)
	{
		CTransform* pCamTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_TEST, TEXT("Layer_Camera"), TEXT("Com_Transform"));

		_vector vCamLook  = pCamTransform->Get_Look();
		_vector vCamBack  = -pCamTransform->Get_Look();
		_vector vCamLeft  = -pCamTransform->Get_Right();
		_vector vCamRight = pCamTransform->Get_Right();

		if (m_pGameInstance->Get_KeyPressing(DIK_W) && m_pGameInstance->Get_KeyPressing(DIK_A))
		{
			m_pTransformCom->Turn_To_Direction(vCamLook + vCamLeft, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamLook + vCamLeft, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_W) && m_pGameInstance->Get_KeyPressing(DIK_D))
		{
			m_pTransformCom->Turn_To_Direction(vCamLook + vCamRight, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamLook + vCamRight, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_A) && m_pGameInstance->Get_KeyPressing(DIK_S))
		{
			m_pTransformCom->Turn_To_Direction(vCamLeft + vCamBack, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamLeft + vCamBack, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_S) && m_pGameInstance->Get_KeyPressing(DIK_D))
		{
			m_pTransformCom->Turn_To_Direction(vCamBack + vCamRight, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamBack + vCamRight, TimeDelta, nullptr);
			return;
		}


		if (m_pGameInstance->Get_KeyPressing(DIK_W))
		{
			m_pTransformCom->Turn_To_Direction(vCamLook, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamLook, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_S))
		{
			m_pTransformCom->Turn_To_Direction(vCamBack, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamBack, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_A))
		{
			m_pTransformCom->Turn_To_Direction(vCamLeft, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamLeft, TimeDelta, nullptr);
			return;
		}
		if (m_pGameInstance->Get_KeyPressing(DIK_D))
		{
			m_pTransformCom->Turn_To_Direction(vCamRight, TimeDelta);
			m_pTransformCom->Go_VectorXZ(vCamRight, TimeDelta, nullptr);
			return;
		}
	}
}

void CTestMonster::OnCollision(_double TimeDelta)
{
	// �浹 �˻�
	//auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster"));
	//for (auto& pDestCollider : *pColliderList)
	//{
	//	if (m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pDestCollider)) // �Ϲ��浹, Enter, Stay, Exit �浹���� ���� �ٸ� �Լ� �����ų �� ����
	//	{
	//		// �����Ű�� ���� �Լ� ����
	//		CGameObject* pDestObject = pDestCollider->Get_Parent(); // DestCollider �� �θ� ���ͼ� Ư�� ����� �����ų �� ����
	//		MSGBOX("�浹");
	//	}
	//}
}

void CTestMonster::ChangeState(_double TimeDelta)
{
	// ���º��� �ڵ� �ۼ�
}

HRESULT CTestMonster::Set_Model(_tchar* pPrototypeTag)
{
	if (nullptr != m_pModelCom)
	{
		m_Components.erase(TEXT("Com_Model"));
		Safe_Release(m_pModelCom);
		Safe_Release(m_pModelCom);
	}

	LEVEL eLevel = LEVEL_END;

	if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Sora")) == 0)
		eLevel = LEVEL_STATIC;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_SkullSoldier")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_CrowSoldier")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_ThinSoldier")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Nightmare")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_BowMan")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Cannon")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_TreasureBox_TT")) == 0)
		eLevel = LEVEL_TWILIGHT;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Monkey")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Mower")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_ShiningEgg")) == 0)
		eLevel = LEVEL_RAPUNZEL;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Nobody")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_BigTree")) == 0)
		eLevel = LEVEL_RAPUNZEL_BOSS;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_IceMan")) == 0)
		eLevel = LEVEL_FROZEN;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_FrozenWolf")) == 0 ||
		lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_LittleWolf")) == 0)
		eLevel = LEVEL_KEYGRAVE;
	else if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Goat")) == 0)
		eLevel = LEVEL_FINALBOSS;

	if (FAILED(__super::Add_Component(eLevel, pPrototypeTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	{
		return E_FAIL;
	}

	SetUp_RootNodeIndex();
	m_pModelCom->Add_OnceList(0);
	m_pModelCom->Set_TempIndex(0);

	if (lstrcmp(pPrototypeTag, TEXT("Prototype_Component_Model_Sora")) == 0)
	{
		vector<CAnimation*>* animations = m_pModelCom->Get_Animations();
		for (auto animation : (*animations))
		{
			animation->Set_PlaySpeed_Rate(1.5);
			animation->Set_MovementRatioXZ(0.f);
			animation->Set_MovementRatioY(0.f);
		}
	}

	return S_OK;
}

CTestMonster* CTestMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTestMonster* pInstance = new CTestMonster(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTestMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestMonster::Clone(void * pArg)
{
	CTestMonster* pInstance = new CTestMonster(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTestMonster");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestMonster::Free()
{
	__super::Free();

	Safe_Release(m_pStatusCom);
	Safe_Release(m_pStateMachineCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pStatusCom);
}