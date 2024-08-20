#include "stdafx.h"
#include "Trigger.h"
#include "GameInstance.h"
#include "GameManager.h"
#include "Progress_Manager.h"
#include "ImGui_Manager.h"
#include "FileLoad_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Perspective.h"
#include "Player.h"

CTrigger::CTrigger(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrigger::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger::NativeConstruct(void* pArg)
{
	CTransform::TRANSFORMDESC TransformDesc;
	TransformDesc.fSpeedPerSec    = 5.f;
	TransformDesc.fRotationPerSec = 5.f;
	TransformDesc.fScalingPerSec  = 5.f;
	if (FAILED(__super::NativeConstruct(&TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ���� ��ġ ����
	if (pArg != nullptr)
	{
		_vector vPosition = __vector(0.f, 0.f, 0.f, 0.f);
		memcpy(&vPosition, pArg, sizeof(_vector));
		m_pTransformCom->Set_Position(vPosition);
	}

	return S_OK;
}

_int CTrigger::Tick(_double TimeDelta)
{
	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	if (true == m_bDead)
		return RESULT_DEAD;

#ifdef _DEBUG
	if (m_bDead == false && m_bReadyToDie == false)
		PickingTrigger(TimeDelta);
#endif

	// �ݶ��̴� ��ġ ������Ʈ
	m_pColliderCom->Update_Transform(m_pTransformCom->Get_WorldMatrix());

	// �ݸ����Ŵ����� �ݶ��̴� ��� (Tick �� ���������� ����ؾ���)
	m_pGameInstance->Add_Collider(string("Trigger"), m_pColliderCom);

	return _int();
}

_int CTrigger::LateTick(_double TimeDelta)
{
	if (0 > __super::LateTick(TimeDelta))
		return	RESULT_ERROR;

	// �浹�˻�
	OnCollision(TimeDelta);

	if (m_bDead == false && m_bReadyToDie == false)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

#ifdef _DEBUG
	if (m_bDead == false && m_bReadyToDie == false)
		m_pRendererCom->Add_Debug(m_pColliderCom);
#endif

	return RESULT_NOPROBLEM;
}

HRESULT CTrigger::Render()
{
	return S_OK;
}

void CTrigger::PostRender(_double TimeDelta)
{
	ImGui::SetNextWindowBgAlpha(0.7f);
	ImGui::Begin("Trigger Control");

	CImGui_Manager* pImGui_Manager = CImGui_Manager::GetInstance();

	pImGui_Manager->SetGameObjectScale(string("SS Scale"), m_pTransformCom, 0.1f, 50.f);
	pImGui_Manager->SmallSeparator();

	const char* CurrentTrigger = Triggers[m_eTrigger];
	ImGui::BulletText("Current Type:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), Triggers[m_eTrigger]);

	int Select = -1;
	ImGui::Combo("SELECT", &Select, Triggers, IM_ARRAYSIZE(Triggers) - 1);
	if (Select != -1 && Select != m_eTrigger)
		m_eTrigger = (TRIGGER)Select;

	ImGui::End();
}

HRESULT CTrigger::Save_ObjectData(HANDLE hFile)
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

	// Ÿ�� ����
	WriteFile(hFile, &m_eTrigger, sizeof(TRIGGER), &dwBytes, nullptr);

	return S_OK;
}

HRESULT CTrigger::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));
	ColliderDesc.vScale       = _float3(1.f, 1.f, 1.f);
	ColliderDesc.vRotation    = _float4(0.f, 0.f, 0.f, 1.f);
	ColliderDesc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

void CTrigger::OnCollision(_double TimeDelta)
{
	// �ױ� ���� ���¶�� �浹�˻� �н�
	if (m_bReadyToDie == true || m_bDead == true)
		return;

	// �÷��̾�� �浹 �˻�
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Player_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		if (true == m_pGameInstance->Collision_Enter_Sphere(m_pColliderCom, pDestCollider))
		{
			// �浹ó��
			ActiveTrigger(TimeDelta);

			// ����ó��
			m_bDead = true;
		}
	}
}

void CTrigger::ActiveTrigger(_double TimeDelta)
{
	switch (m_eTrigger)
	{
	case TRIGGER_TT_FIRST:
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_FIRST);
		GM->Set_OnTrigger(0, true);
		{
			// UI ����
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 3;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}
		break;
	case TRIGGER_TT_SECOND:
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_SECOND);
		GM->Set_OnTrigger(1, true);
		{
			// UI ����
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 3;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_TWILIGHT, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}

		break;
	case TRIGGER_TT_BOSS:
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_TT_BOSS);
		GM->Set_OnTrigger(2, true);
		GM->Get_Camera()->Set_HorizonShake(3.f, 2.f, 1.5f);
		GM->Get_Player()->Set_ControlMode(false);
		// ���� ī�޶� ����
		GM->Get_Camera()->Set_BossScene(true);
		GM->Get_Camera()->Set_Focusing(false);
		GM->Set_bCameraMove(true);
		GM->Get_Player()->MakeState_Idle();
		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);

		m_pGameInstance->StopSound(CSoundMgr::BGM);
		//m_pGameInstance->PlayBGM(TEXT("TwilightTownBossBGM.mp3"), 0.7f);
	
		break;
	// ==================================================================================================================================================================================================
	case TRIGER_RA_FIRST:
		// �Ա� ī�޶� ����
		break;
	case TRIGGER_RA_SECOND:
		// �߾��� �ѷ��� ���� ����
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_SECOND);
		{
			// UI ����
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 3;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}
		break;
	case TRIGGER_RA_THIRD:
		// ���� ������µ� �ڿ� ���� ����
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_THIRD);
		CCamera_Manager::GetInstance()->Load_Camera("RA_SecondMonster.dat");
		CCamera_Manager::GetInstance()->Play_Start();
		{
			// UI ����
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 3;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}
		GM->Get_Player()->MakeState_Idle();
		GM->Get_Player()->Change_PlayerAnimation(CPlayer::SORA_ANIM_IDLE);
		break;
	case TRIGGER_RA_FOURTH:
		// ��� ������ �� ������ ���ٰ� ���� ����
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_FOURTH);
		{
			// UI ����
			_int iComTex = 0;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}

			iComTex = 3;
			if (FAILED(m_pGameInstance->Add_GameObject(LEVEL_RAPUNZEL, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_UI_Tip_Down"), &iComTex)))
			{
				BREAKPOINT;
				return;
			}
		}
		break;
	case TRIGGER_RA_FIFTH:
		// ��ٵ� ���� ����
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_FIFTH);
		GM->Get_Camera()->Start_CameraWork(CCamera_Perspective::WORK_RAPUNZEL_NOBODY_APPEAR);
		PM->Set_ReadyWhiteFadeOut(true);
		break;
	// ==================================================================================================================================================================================================
	case TRIGGER_RA_BOSS_BIGTREE:
		// ��Ǭ�� ž ���ٰ� ���� ����
		CFileLoad_Manager::GetInstance()->Load_SaveFile(CFileLoad_Manager::SAVEFILE_RA_BOSS_BIGTREE);
		PM->Set_ReadyWhiteFadeOut(true);
		break;
	}
}

void CTrigger::PickingTrigger(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	// ��Ʈ�� + ��Ŭ������ ũ��, Ʈ����Ÿ�� ����
	if (pGameInstance->Get_KeyPressing(DIK_LCONTROL) && pGameInstance->Get_Mouse_Enter(CInput_Device::MBS_LBUTTON))
	{
		auto TriggerList = *(pGameInstance->Get_ObjectList(pGameInstance->Get_LevelIndex(), TEXT("Layer_Trigger")));

		_float fDist = 0.f;
		if (pGameInstance->Ray_Sphere_Collision(*m_pColliderCom->Get_Sphere(), fDist) == true)
		{
			for (auto& pTrigger : TriggerList)
			{
				if (pTrigger == this)
					continue;
				pTrigger->Set_ImGuiController(false);
			}

			m_bImGuiController = !m_bImGuiController;
		}
	}
}

CTrigger* CTrigger::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CTrigger* pInstance = new CTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CTrigger::Clone(void* pArg)
{
	CTrigger* pInstance = new CTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Created CTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTrigger::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pRendererCom);
}