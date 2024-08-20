#include "stdafx.h"
#include "Player_Weapon.h"

#include "GameInstance.h"
#include "Player.h"
#include "Effect_Manager.h"
#include "ImGui_Manager.h"
#include "GameManager.h"
#include "Monster.h"
#include "Progress_Manager.h"

#include "UI_Num_Manager.h"
#include "State_PlayerForm.h"

#include "Terrain.h"
#include "MotionTrail.h"

CPlayer_Weapon::CPlayer_Weapon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer_Weapon::CPlayer_Weapon(const CPlayer_Weapon & rhs)
	: CGameObject(rhs)
{
}

void CPlayer_Weapon::Plus_WeaponType()
{
	_uint iweapon_type = (_uint)m_eWeaponType;
	iweapon_type++;

	m_eWeaponType = (WEAPON_TYPE)iweapon_type;

	if (WEAPON_TYPE_END == m_eWeaponType)
		m_eWeaponType = WEAPON_TYPE_KINGDOM_KEY;
}

HRESULT CPlayer_Weapon::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Weapon::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC tTransformDesc;

	tTransformDesc.fSpeedPerSec = 0.f;
	tTransformDesc.fRotationPerSec = XMConvertToRadians(0.f);
	tTransformDesc.fScalingPerSec = 0.f;

	if (FAILED(__super::NativeConstruct(&tTransformDesc)))
		return E_FAIL;

	m_eObjectType = OBJECT_TYPE::OBJECT_WEAPON;
	m_iPass = 0;

	CGameInstance* pGI = CGameInstance::GetInstance();
	LEVEL current_level = GM->Get_CurrentLevel();

	// �÷��̾� ����
	m_pPlayer = static_cast<CPlayer*>(pGI->Get_Front(current_level, TEXT("Layer_Player")));

	if (nullptr == m_pPlayer)
		_breakpoint;

	// �÷��̾� ������Ʈ ����
	m_pPlayerStateMachine	= m_pPlayer->Get_StateMachine();
	m_pPlayerTransform		= m_pPlayer->Get_Transform();
	m_pPlayerModel			= m_pPlayer->Get_Model();

	// �÷��̾� �� �ǹ�
	m_PlayerModelTransformMatrix = m_pPlayerModel->Get_TransformMatrix();

	// ���� ����
	m_eWeaponType = WEAPON_TYPE::WEAPON_TYPE_KINGDOM_KEY;
	m_eWeaponState = WEAPON_STATE::WEAPON_STATE_SHEATHE;



	// NORMAL FORM
	m_fNormalCollider_Scale = 1.f;
	m_fNormalCollider_Position = _float4(-0.5f, 0.f, 0.f, 1.f);

	m_pNormalCollider_ComponentTag = TEXT("Com_Collider_NormalAttack");



	// HAMMER FORM
	m_fHammerCollider_LocalScale[HAMMER_COLLIDER_HEAD] = 2.f;
	m_fHammerCollider_LocalScale[HAMMER_COLLIDER_WAVE] = 4.f;
	m_fHammerCollider_LocalScale[HAMMER_COLLIDER_LAST] = 8.f;
	m_fHammerCollider_LocalScale[HAMMER_COLLIDER_FINISH] = 8.f;

	m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_HEAD] = _float4(-1.f, -0.25f, 0.f, 1.f);
	m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_WAVE] = _float4(-1.f, -0.25f, 0.f, 1.f);
	m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_LAST] = _float4(-1.f, -0.25f, 0.f, 1.f);
	m_vHammerCollider_LocalPosition[HAMMER_COLLIDER_FINISH] = _float4(0.f, 1.f, 3.f, 1.f);

	m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_HEAD] = TEXT("Com_HammerCollider_Head");
	m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_WAVE] = TEXT("Com_HammerCollider_Wave");
	m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_LAST] = TEXT("Com_HammerCollider_Last");
	m_pHammerCollider_ComponentTag[HAMMER_COLLIDER_FINISH] = TEXT("Com_HammerCollider_Finish");


	// HAMMER HEAD POINT
	m_fHammer_HeadPoint_LocalScale = 0.5f;
	for (_uint i = 0; i < HAMMER_SPOT_END; ++i)
	{
		if (HAMMER_SPOT_BOOSTER == i)
		{
			m_vHammer_HeadPoint_LocalPosition[i] = _float4(0.1f, 0.f, 0.f, 1.f);
			continue;
		}

		m_vHammer_HeadPoint_LocalPosition[i] = _zeroPoint4;
	}

	m_pHammer_HeadPoint_ComponentTag[HAMMER_SPOT_HEAD] = TEXT("Com_Hammer_HeadPosition");
	m_pHammer_HeadPoint_ComponentTag[HAMMER_SPOT_BODY] = TEXT("Com_Hammer_BodyPosition");
	m_pHammer_HeadPoint_ComponentTag[HAMMER_SPOT_BOOSTER] = TEXT("Com_Hammer_BoosterPosition");

	for (_uint i = 0; i < HAMMER_SPOT_END; ++i)
		XMStoreFloat4x4(&m_Hammer_HeadPoint_Matrix[i], XMMatrixIdentity());

	// RAGE FORM
	m_fRageCollider_LocalScale[RAGE_COLLIDER_BLADE] = 1.75f;
	m_fRageCollider_LocalScale[RAGE_COLLIDER_NAIL] = 1.75f;
	m_fRageCollider_LocalScale[RAGE_COLLIDER_FRONT] = 5.f;
	m_fRageCollider_LocalScale[RAGE_COLLIDER_AROUND] = 6.f;
	m_fRageCollider_LocalScale[RAGE_COLLIDER_FINISH] = 15.f;
	m_fRageCollider_LocalScale[RAGE_COLLIDER_BLADEAREA] = 3.f;

	m_pRageCollider_ComponentTag[RAGE_COLLIDER_BLADE] = TEXT("Com_RageCollider_Blade");
	m_pRageCollider_ComponentTag[RAGE_COLLIDER_NAIL] = TEXT("Com_RageCollider_Nail");
	m_pRageCollider_ComponentTag[RAGE_COLLIDER_FRONT] = TEXT("Com_RageCollider_Front");
	m_pRageCollider_ComponentTag[RAGE_COLLIDER_AROUND] = TEXT("Com_RageCollider_Around");
	m_pRageCollider_ComponentTag[RAGE_COLLIDER_FINISH] = TEXT("Com_RageCollider_Finish");
	m_pRageCollider_ComponentTag[RAGE_COLLIDER_BLADEAREA] = TEXT("Com_RageCollider_Counter");



	// ��ġ, ũ�� ���� �� ������Ʈ �±� ���� ���� ( Setup_Components �ϱ� ���� �����ؾ��Ѵ� )
	m_pModelComponentTag[WEAPON_TYPE_KINGDOM_KEY] = TEXT("Com_Model_Kingdom_Key");
	m_pModelComponentTag[WEAPON_TYPE_FAVORITE_DEPUTY] = TEXT("Com_Model_Favorite_Deputy");
	m_pModelComponentTag[WEAPON_TYPE_HYPER_HAMMER] = TEXT("Com_Model_Hyper_Hammer");
	m_pModelComponentTag[WEAPON_TYPE_HYPER_HAMMER_CHANGING] = TEXT("Com_Model_Hyper_Hammer_Changing");

	// ������Ʈ ����
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	// ================================================================================================

	// NORMAL FORM
	m_BoneMatrixPtr_Normal = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");
	m_bRegisterCollider = false;

	// HAMMER FORM
	m_BoneMatrixPtr_Hammer[HAMMER_COLLIDER_HEAD] = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");
	m_BoneMatrixPtr_Hammer[HAMMER_COLLIDER_WAVE] = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");
	m_BoneMatrixPtr_Hammer[HAMMER_COLLIDER_LAST] = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");
	m_BoneMatrixPtr_Hammer[HAMMER_COLLIDER_FINISH] = m_pPlayerModel->Get_BoneMatrixPtr("kh_sk");

	for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		m_bRegisterCollider_Hammer[i] = false;

	// HAMMER HEAD POINT
	m_BoneMatrixPtr_Hammer_HeadPoint[HAMMER_SPOT_HEAD] = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Get_BoneMatrixPtr("Hammer_Head");
	m_BoneMatrixPtr_Hammer_HeadPoint[HAMMER_SPOT_BODY] = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Get_BoneMatrixPtr("Rocket_base");
	m_BoneMatrixPtr_Hammer_HeadPoint[HAMMER_SPOT_BOOSTER] = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Get_BoneMatrixPtr("BH_Jet");

	// RAGE FORM
	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_BLADE] = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");
	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_NAIL] = m_pPlayerModel->Get_BoneMatrixPtr("L_buki");

	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_FRONT] = m_pPlayerModel->Get_BoneMatrixPtr("center");
	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_AROUND] = m_pPlayerModel->Get_BoneMatrixPtr("kh_sk");
	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_FINISH] = m_pPlayerModel->Get_BoneMatrixPtr("center");
	m_BoneMatrixPtr_Rage[RAGE_COLLIDER_BLADEAREA] = m_pPlayerModel->Get_BoneMatrixPtr("R_buki");

	for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		m_bRegisterCollider_Rage[i] = false;

	/*
		������ ������Ʈ ���� �Ŀ� BoneMatrix���� �����ϴ� ������
		�÷��̾� ���� �ƴ϶� ������ �𵨿��� �����;� �ϴ� ��쵵 �ֱ� ����
	*/

	// ===========================================================================================

	for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		m_vRageCollider_Position[i] = _zeroPoint4;

	XMStoreFloat4x4(&m_RageCollider_Front_Matrix, XMMatrixIdentity());

	return S_OK;
}

_int CPlayer_Weapon::Tick(_double TimeDelta)
{
	if (true == PM->Get_MinigameWaiting())
		return RESULT_NOPROBLEM;

	if (0 > __super::Tick(TimeDelta))
		return RESULT_ERROR;

	// ���� ImGuI �˾�
	CGameInstance* pGI = CGameInstance::GetInstance();

	if (pGI->Get_KeyPressing(DIK_LCONTROL))
	{
		if (pGI->Get_KeyEnter(DIK_W))
			m_bImGuiController = !m_bImGuiController;
	}

	if (true == m_pGameInstance->Get_KeyEnter(DIK_Z))
		m_pStatusCom->Set_Damage(9999.f);
	if (true == m_pGameInstance->Get_KeyEnter(DIK_X))
		m_pStatusCom->Set_Damage(5.f);													

	// ------------------------------------------------

	// ���� �� ����, ���� ���¸� �˻��ؼ� ������ ������ ��� Add_Collider
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
		// ��� �� ������ ��
	case Client::CPlayer::SF_NORMAL:

		// ���� ������ ���
		if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
		{
			// �Ϲ� ���� �ݶ��̴� �浹 �˻�
			if (true == m_bRegisterCollider)
				m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_NormalAttack);
		}

		// �Ϲ� �ݶ��̴� ����
		if (true == m_bRegisterCollider)
			m_pCollider_NormalAttack->Set_Color(_cyan4);
		else
			m_pCollider_NormalAttack->Set_Color(_white4);

		break;

		// �ظ� �� ������ ��
	case Client::CPlayer::SF_HAMMER:

		// ���� ������ ���
		if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
		{
			// ���� �ִϸ��̼��� �������� �޾Ƶ�(� ���� �ִϸ��̼��� ���� ���ΰ�)
			CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModel->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

			// 1. �ظ� �� �Ϲ� ���� HAMMER_COLLIDER_HEAD �浹 �˻�
			if(true == m_bRegisterCollider_Hammer[HAMMER_COLLIDER_HEAD])
				m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_HammerForm[HAMMER_COLLIDER_HEAD]);

			// 2. �ظ� �� ������ ���� �ִϸ��̼����� Ȯ��
			if (CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK == curr_anim)
			{
				// HAMMER_COLLIDER_LAST �浹 �˻�
				if (true == m_bRegisterCollider_Hammer[HAMMER_COLLIDER_LAST])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_HammerForm[HAMMER_COLLIDER_LAST]);

				// ���Ʈ����
				if (CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == curr_anim)
				{
					if (19.0 <= m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY) && 46.0 > m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY))
						m_bUseMotionTrail = true;
					else
						m_bUseMotionTrail = false;
				}
				else
				{
					if (0.0 < m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY) && 28.0 > m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY))
						m_bUseMotionTrail = true;
					else
						m_bUseMotionTrail = false;
				}
			}

			// 3. �ظ� �� ����İ� �߻��ϴ� �ִϸ��̼����� Ȯ��
			if (CPlayer::SORA_ANIM_HAMMER_ATTACK_01 == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_ATTACK_05 == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_ATTACK_06 == curr_anim ||
				CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK == curr_anim)
			{
				// HAMMER_COLLIDER_WAVE �浹 �˻�
				if(true == m_bRegisterCollider_Hammer[HAMMER_COLLIDER_WAVE])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_HammerForm[HAMMER_COLLIDER_WAVE]);
			}
			
			// 4. �ظ� �� �ǴϽ� �������� Ȯ��
			if (CPlayer::FORM_FUNC_HAMMER_FINISH == curr_anim)
			{
				if (true == m_bRegisterCollider_Hammer[HAMMER_COLLIDER_FINISH])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_HammerForm[HAMMER_COLLIDER_FINISH]);
			}
		}

		// �ظ� �� �ݶ��̴� ����
		for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		{
			if (true == m_bRegisterCollider_Hammer[i])
				m_pCollider_HammerForm[i]->Set_Color(_lightblue4);
			else
				m_pCollider_HammerForm[i]->Set_Color(_mildblue4);
		}

		if (true == m_bAppear)
		{
			_double appear_anim_play_speed = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Get_TargetPlaySpeed(0);
			m_dTimer_Control_RimBrightness += TimeDelta * appear_anim_play_speed;

			// �߰��� �ѹ� 0.0���� Ŭ����
			if (100.0 <= m_dTimer_Control_RimBrightness)
				m_dTimer_Control_RimBrightness = 100.0;

			// �� ����Ʈ from �Ŀ�, to �Ŀ�, �ð�
			m_fRimPower = LinearInterpolation(4.f, 1.5f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);
			m_vRimColor.x = LinearInterpolation(1.f, 0.f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);
			m_vRimColor.y = LinearInterpolation(1.f, 0.5f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);

			if (100.0 <= m_dTimer_Control_RimBrightness)
			{
				m_dTimer_Control_RimBrightness = 0.0;
				m_dTimer_Control_RimBrightness_Max = 5.0;
				m_bAppear = false;
			}
		}
		else
		{
			if (WEAPON_TYPE::WEAPON_TYPE_HYPER_HAMMER == m_eWeaponType)
			{
				m_dTimer_Control_RimBrightness += TimeDelta;

				if (5.0 <= m_dTimer_Control_RimBrightness)
				{
					m_dTimer_Control_RimBrightness = 5.0;
					m_isUsingRim = false;
				}

				// �� ����Ʈ from �Ŀ�, to �Ŀ�, �ð�
				m_fRimPower = LinearInterpolation(1.5f, 10.f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);
				m_vRimColor.y = LinearInterpolation(0.5f, 0.f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);
				m_vRimColor.z = LinearInterpolation(1.f, 0.f, (_float)m_dTimer_Control_RimBrightness / (_float)m_dTimer_Control_RimBrightness_Max);
			}
		}

		break;
	case Client::CPlayer::SF_RAGE:

		// ���� ������ �� �ش� ���ݿ� �´� �ݶ��̴� �浹 �˻�
		if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
		{
			// ���� �ִϸ��̼��� �������� �޾Ƶ�(� ���� �ִϸ��̼��� ���� ���ΰ�)
			CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModel->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

			switch (curr_anim)
			{
			case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
			case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

				if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_BLADE])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_BLADE]);

				if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_NAIL])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_NAIL]);

				break;
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

				if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_AROUND])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_AROUND]);

		

				break;
			case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

				if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_FRONT])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_FRONT]);

				break;
			case Client::CPlayer::SORA_ANIM_RAGE_FINISH:

				if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_FINISH])
					m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_FINISH]);

				break;
			}
		}

		// ���� ������ �� ī���� ���� �浹 �˻�
		if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_GUARD))
		{
			if (true == m_bRegisterCollider_Rage[RAGE_COLLIDER_BLADEAREA])
				m_pGameInstance->Add_Collider(string("Player_Atk"), m_pCollider_RageForm[RAGE_COLLIDER_BLADEAREA]);
		}

		// ������ �� �ݶ��̴� ����
		for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		{
			if (true == m_bRegisterCollider_Rage[i])
				m_pCollider_RageForm[i]->Set_Color(_lightpink4);
			else
				m_pCollider_RageForm[i]->Set_Color(_mildred4);
		}

		break;
	}

	return RESULT_NOPROBLEM;
}

_int CPlayer_Weapon::LateTick(_double TimeDelta)
{
	if (true == PM->Get_MinigameWaiting())
		return RESULT_NOPROBLEM;

	// ���⸦ ���� ���°� �ƴ϶�� return
	if (WEAPON_STATE::WEAPON_STATE_DRAW != m_eWeaponState)
		return RESULT_NOPROBLEM;

	if (0 > __super::LateTick(TimeDelta))
		return RESULT_ERROR;

	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	_matrix	TransformMatrix = XMMatrixIdentity();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		switch (m_eWeaponType)
		{
		case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

			// ��� ����
			TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) *
				XMLoadFloat4x4(m_BoneMatrixPtr_Normal.pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

			break;
		case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:

			// ��� ����
			TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f)) *
				XMLoadFloat4x4(m_BoneMatrixPtr_Normal.pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

			break;
		}

		// ���� ���� & �� ������Ʈ
		m_pTransformCom->Set_WorldMatrix(TransformMatrix);
		m_pModelCom[m_eWeaponType]->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

		// �ݶ��̴� ��ġ ����
		m_pCollider_NormalAttack->Update_Transform(TransformMatrix);

		// �Ϲ� �ݶ��̴� ��������
		if (true == m_bRegisterCollider)
		{
			// ���� ������ ��
			if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
				OnCollision(m_pCollider_NormalAttack);
		}
		else
		{
			// �ݶ��̴��� ����ϴ� Ÿ�̹��� �ƴ� ������ Ŭ�����صд�
			m_pCollider_NormalAttack->Clear_Colliding_All();
			m_CollisionParents.clear();
		}

		break;
	case Client::CPlayer::SF_HAMMER:

		switch (m_eWeaponType)
		{
		case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:

			// ��� ����
			TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(m_fRotationValue)) *
				XMLoadFloat4x4(m_BoneMatrixPtr_Normal.pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

			// ���� ���� & �� ������Ʈ
			m_pTransformCom->Set_WorldMatrix(TransformMatrix);
			m_pModelCom[m_eWeaponType]->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

			break;
		case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER_CHANGING:

			// ��� ����
			TransformMatrix = m_pPlayerTransform->Get_WorldMatrix();

			// ���� ���� & �� ������Ʈ
			m_pTransformCom->Set_WorldMatrix(TransformMatrix);
			m_pModelCom[m_eWeaponType]->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

			if (37.0 <= m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY) && 83.0 > m_pPlayerModel->Get_CurrTimeAcc_EachBody(CPlayer::SB_BODY))
				m_bUseMotionTrail = true;
			else
				m_bUseMotionTrail = false;

			break;
		}
		
		/*if (WEAPON_TYPE_HYPER_HAMMER_CHANGING == m_eWeaponType)
			break;*/

		for (_uint i = 0; i < HAMMER_SPOT_END; ++i)
		{
			// ������ ������ �ݶ��̴� ������Ʈ ��� ����
			TransformMatrix = XMLoadFloat4x4(m_BoneMatrixPtr_Hammer_HeadPoint[i].pCombinedMatrix) * XMLoadFloat4x4(&m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Get_TransformMatrix()) * m_pTransformCom->Get_WorldMatrix();

			// �ݶ��̴� ��ġ ����
			m_pCollider_HammerHeadPoint[i]->Update_Transform(TransformMatrix);

			// ������ �ݶ��̴� ��ġ ����
			XMStoreFloat4x4(&m_Hammer_HeadPoint_Matrix[i], TransformMatrix);
			XMStoreFloat4(&m_vHammer_HeadPoint_Position[i], m_pCollider_HammerHeadPoint[i]->Get_Center());
		}

		for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		{
			// �ݶ��̴� ������Ʈ�� ��� ����
			TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) * XMMatrixRotationAxis(__vector(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(180.f)) *
				XMLoadFloat4x4(m_BoneMatrixPtr_Hammer[i].pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

			// �ݶ��̴� ��ġ ����
			m_pCollider_HammerForm[i]->Update_Transform(TransformMatrix);
		}

		// �������ظ� �� �ݶ��̴� ��������
		for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		{
			if (true == m_bRegisterCollider_Hammer[i])
			{
				// ���� ������ ��
				if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
					OnCollision(m_pCollider_HammerForm[(HAMMER_COLLIDER)i]);
			}
			else
			{
				m_pCollider_HammerForm[i]->Clear_Colliding_All();
				m_CollisionParents_Hammer[i].clear();
			}
		}


		break;
	case Client::CPlayer::SF_RAGE:

		// ��� ����
		TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) *
			XMLoadFloat4x4(m_BoneMatrixPtr_Normal.pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

		// ���� ���� & �� ������Ʈ
		m_pTransformCom->Set_WorldMatrix(TransformMatrix);
		m_pModelCom[m_eWeaponType]->Update_CombinedTransformationMatrices(TimeDelta, m_pTransformCom);

		for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		{
			// �ݶ��̴� ������Ʈ�� ��� ����
			TransformMatrix = XMMatrixRotationAxis(__vector(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f)) *
				XMLoadFloat4x4(m_BoneMatrixPtr_Rage[i].pCombinedMatrix) * XMLoadFloat4x4(&m_PlayerModelTransformMatrix) * m_pPlayerTransform->Get_WorldMatrix();

			// �ݶ��̴� ��ġ ����
			m_pCollider_RageForm[i]->Update_Transform(TransformMatrix);

			// ����Ʈ ������ ����ϱ����� ���� ��ġ ����
			XMStoreFloat4(&m_vRageCollider_Position[i], m_pCollider_RageForm[i]->Get_Center());

			// ����Ʈ ������ ����ϱ����� ��Ʈ���� ��ġ ����
			if (RAGE_COLLIDER_FRONT == (RAGE_COLLIDER)i)
				XMStoreFloat4x4(&m_RageCollider_Front_Matrix, TransformMatrix);
		}

		// ������ �� �ݶ��̴� ��������
		for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		{
			// ������ �� �ݶ��̴� ���������� ��
			if (true == m_bRegisterCollider_Rage[i])
			{
				// ���� ������ ��
				if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_ATK))
					OnCollision(m_pCollider_RageForm[(RAGE_COLLIDER)i]);

				// ���� ������ ��
				if (true == m_pPlayerStateMachine->FindState(CStateMachine::STATETYPE_GUARD))
					OnCollision(m_pCollider_RageForm[RAGE_COLLIDER_BLADEAREA]);
			}

			// �ݶ��̴��� ����ϴ� Ÿ�̹��� �ƴ� �� Ŭ����
			else
			{
				m_pCollider_RageForm[i]->Clear_Colliding_All();
				m_CollisionParents_Rage[i].clear();
			}
		}

		break;
	case Client::CPlayer::SF_END:
		_breakpoint;
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

//#ifdef _DEBUG

	// ���⸦ ���� ������ �� �ݶ��̴� ����
	if (WEAPON_STATE::WEAPON_STATE_DRAW == m_eWeaponState)
	{
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		switch (curr_form)
		{
			// ��� ���� ��
		case Client::CPlayer::SF_NORMAL:

			// �Ϲ� ���� �ݶ��̴� ����
			m_pRendererCom->Add_Debug(m_pCollider_NormalAttack);
			break;

			// �ظ� ���� ��
		case Client::CPlayer::SF_HAMMER:

			if (true == m_bRender_All_Colliders)
			{
				for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
					m_pRendererCom->Add_Debug(m_pCollider_HammerForm[i]);

				for (_uint i = 0; i < HAMMER_SPOT_END; ++i)
					m_pRendererCom->Add_Debug(m_pCollider_HammerHeadPoint[i]);

			}
			else
			{
				CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModel->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
				vector<HAMMER_COLLIDER> render_target_types;
				switch (curr_anim)
				{
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_01:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					render_target_types.push_back(HAMMER_COLLIDER_WAVE);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_02:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_03:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_04:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_05:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					render_target_types.push_back(HAMMER_COLLIDER_WAVE);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_ATTACK_06:
					render_target_types.push_back(HAMMER_COLLIDER_HEAD);
					render_target_types.push_back(HAMMER_COLLIDER_WAVE);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_GROUND_LAST_ATTACK:
					render_target_types.push_back(HAMMER_COLLIDER_LAST);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_AIR_LAST_ATTACK:
					render_target_types.push_back(HAMMER_COLLIDER_LAST);
					break;
				case Client::CPlayer::SORA_ANIM_HAMMER_FINISH:
					render_target_types.push_back(HAMMER_COLLIDER_FINISH);
					break;
				}

				for(auto hammer_collider_type : render_target_types)
					m_pRendererCom->Add_Debug(m_pCollider_HammerForm[hammer_collider_type]);
			}

			break;

			// ������ ���� ��
		case Client::CPlayer::SF_RAGE:

			if (true == m_bRender_All_Colliders)
			{
				for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
					m_pRendererCom->Add_Debug(m_pCollider_RageForm[i]);
			}
			else
			{
				CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModel->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);
				switch (curr_anim)
				{
				case Client::CPlayer::SORA_ANIM_RAGE_DOUBLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_TRIPLE_TAP:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK1:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK2:

					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_BLADE]);
					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_NAIL]);

					break;
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK3:
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK5:

					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_AROUND]);

					break;
				case Client::CPlayer::SORA_ANIM_RAGE_ATTACK4:

					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_FRONT]);

					break;
				case Client::CPlayer::SORA_ANIM_RAGE_FINISH:

					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_FINISH]);

					break;
				case Client::CPlayer::SORA_ANIM_RAGE_COUNTER_ATTACK:

					m_pRendererCom->Add_Debug(m_pCollider_RageForm[RAGE_COLLIDER_BLADEAREA]);

					break;
				}
			}

			break;
		}
	}

//#endif // _DEBUG

	return RESULT_NOPROBLEM;
}

HRESULT CPlayer_Weapon::Render()
{
	if (nullptr == m_pModelCom[m_eWeaponType])
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���⸦ ���� ���°� �ƴ� ��� return
	if (WEAPON_STATE::WEAPON_STATE_DRAW != m_eWeaponState)
		return S_OK;

	_bool anim_shader = false;
	if (FAILED(SetUp_ConstantTable(&anim_shader)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	// ���Ʈ���Ͽ� ���̴� ����Ʈ���� ���� ���� �����̳�
	vector<CModel::MESHDESC*> MeshDescVector;

	CShader* target_shader = nullptr;
	if (true == anim_shader)
		target_shader = m_pShaderCom_Anim;
	else
		target_shader = m_pShaderCom_NonAnim;

	_uint iNumMaterials = m_pModelCom[m_eWeaponType]->Get_NumberOfMaterials();
	_uint iPass = m_iPass;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		m_pModelCom[m_eWeaponType]->Bind_OnShader(target_shader, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		m_pModelCom[m_eWeaponType]->Bind_OnShader(target_shader, i, aiTextureType_NORMALS, "g_NormalTexture");

		// RageForm�϶� �̰ɷ� ���ε� ���ٰ� ===================================================
		if (CPlayer::SF_RAGE == m_pPlayer->Get_Form() && WEAPON_TYPE_KINGDOM_KEY == m_eWeaponType)
			m_pRageFormTex->SetUp_OnShader(target_shader, "g_DiffuseTexture", 0);

		if (true == m_isUsingBlur)
			iPass = 6;

		if (true == m_isUsingRim)
			iPass = 12;

		if (true == m_pPlayer->Get_TP())
			iPass = 7;

		if (true == anim_shader)
		{
			if (true == m_bUseMotionTrail)
				m_pModelCom[m_eWeaponType]->Render(target_shader, iPass, i, "g_BoneMatrices", &MeshDescVector);
			else
				m_pModelCom[m_eWeaponType]->Render(target_shader, iPass, i, "g_BoneMatrices");
		}
		else
			m_pModelCom[m_eWeaponType]->Render(target_shader, 44, i);
	}

	// ��� Ʈ���� ��ü ����
	if (true == m_bUseMotionTrail)
	{
		CMotionTrail::MOTIONTRAILDESC tMotionTrailDesc;
		tMotionTrailDesc.iSize = 280;	// ����Ʈ�������� (����)
		tMotionTrailDesc.pMeshDescVector = &MeshDescVector;	// ����Ʈ���� ������
		tMotionTrailDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);	// ���ϴ� ��� Ʈ���� ����
		if (CPlayer::SF_RAGE == m_pPlayer->Get_Form())
			tMotionTrailDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);	// ���ϴ� ��� Ʈ���� ����
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

void CPlayer_Weapon::PostRender(_double TimeDelta)
{
	ImGui::Begin("Player Weapon Panel");

	ImGui::Checkbox("Player Blur", &m_isUsingBlur);
	ImGui::Checkbox("Player Rim Light", &m_isUsingRim);

	ImGui::Separator();
	ImGui::Text("Rim Light Control");
	ImGui::Separator();

	ImGui::DragFloat("Power", &m_fRimPower, 0.05f);
	ImGui::DragFloat("Range", &m_fRimRange, 0.05f);

	_float rim_color[4] = { m_vRimColor.x, m_vRimColor.y, m_vRimColor.z, m_vRimColor.w };
	ImGui::DragFloat4("Color", rim_color, 0.05f);
	memcpy(&m_vRimColor, rim_color, sizeof(_float) * 4);

	// ���� ���� Ÿ�� ǥ��
	switch (m_eWeaponType)
	{
	case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:
		ImGui::Text("[ Weapon Type : KINGDOM_KEY ]");
		break;
	case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:
		ImGui::Text("[ Weapon Type : FAVORITE_DEPUTY ]");
		break;
	case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:
		ImGui::Text("[ Weapon Type : HYPER_HAMMER ]");
		break;
	case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER_CHANGING:
		ImGui::Text("[ Weapon Type : HYPER_HAMMER_CHANGING ]");
		break;
	}

	ImGui::Separator();
	ImGui::Separator();

	_float knockback_timer = (_float)m_dKnockBackTimer;
	ImGui::InputFloat("KnockBack Power", &m_fKnockBackPower);
	ImGui::InputFloat("KnockBack Timer", &knockback_timer);
	m_dKnockBackTimer = (_double)knockback_timer;

	ImGui::Separator();
	ImGui::Separator();

	// ���� ���� �ش��ϴ� ��� �ݶ��̴��� �ѹ��� �������� ����
	ImGui::Checkbox("Render All Colliders", &m_bRender_All_Colliders);

	static _bool normal_attack_col = false;
	static _bool rage_attack_col = false;
	static _bool hammer_attack_col = false;
	static _bool spot_col = false;

	ImGui::Checkbox("Normal Attack Collider", &normal_attack_col);
	ImGui::Checkbox("Rage Attack Collider", &rage_attack_col);
	ImGui::Checkbox("Hammer Attack Collider", &hammer_attack_col);
	ImGui::Checkbox("Hammer Spot Collider", &spot_col);

	if (true == ImGui::CollapsingHeader("Collider Geometry Control"))
	{
		// �Ϲ� ���� �ݶ��̴�
		if (true == normal_attack_col)
		{
			ImGui::Separator();
			_float3 scale = m_pCollider_NormalAttack->Get_Scale();

			ImGui::DragFloat("Collider Scale", &scale.x, 0.05f);

			m_pCollider_NormalAttack->Set_Scale(_float3(scale.x, scale.x, scale.x));

			_float4 position = m_pCollider_NormalAttack->Get_Translation();
			_float position_f[3] = { position.x, position.y, position.z };

			ImGui::DragFloat3("Collider Position", position_f, 0.05f);

			m_pCollider_NormalAttack->Set_Translation(_float4(position_f[0], position_f[1], position_f[2], 1.f));
			ImGui::Separator();
		}
		
		// ������ �� �ݶ��̴�
		if (true == rage_attack_col)
		{
			static _int rage_col_number = 0;
			ImGui::SliderInt("Number", &rage_col_number, 0, (_int)RAGE_COLLIDER_END - 1);

			ImGui::Separator();

			RAGE_COLLIDER rage_collider_type = (RAGE_COLLIDER)rage_col_number;
			switch (rage_collider_type)
			{
			case Client::CPlayer_Weapon::RAGE_COLLIDER_BLADE:
				ImGui::Text("Selected Rage Collider : [ BLADE ]");
				break;
			case Client::CPlayer_Weapon::RAGE_COLLIDER_NAIL:
				ImGui::Text("Selected Rage Collider : [ NAIL ]");
				break;
			case Client::CPlayer_Weapon::RAGE_COLLIDER_FRONT:
				ImGui::Text("Selected Rage Collider : [ FRONT ]");
				break;
			case Client::CPlayer_Weapon::RAGE_COLLIDER_AROUND:
				ImGui::Text("Selected Rage Collider : [ AROUND ]");
				break;
			case Client::CPlayer_Weapon::RAGE_COLLIDER_FINISH:
				ImGui::Text("Selected Rage Collider : [ FINISH ]");
				break;
			case Client::CPlayer_Weapon::RAGE_COLLIDER_BLADEAREA:
				ImGui::Text("Selected Rage Collider : [ BLADE_AREA ]");
				break;
			}

			ImGui::Separator();

			_float3 scale = m_pCollider_RageForm[rage_collider_type]->Get_Scale();

			ImGui::DragFloat("Collider Scale", &scale.x, 0.05f);

			m_pCollider_RageForm[rage_collider_type]->Set_Scale(_float3(scale.x, scale.x, scale.x));

			_float4 position = m_pCollider_RageForm[rage_collider_type]->Get_Translation();
			_float position_f[3] = { position.x, position.y, position.z };

			ImGui::DragFloat3("Collider Position", position_f, 0.05f);

			m_pCollider_RageForm[rage_collider_type]->Set_Translation(_float4(position_f[0], position_f[1], position_f[2], 1.f));
			ImGui::Separator();
		}

		// �ظ� �� �ݶ��̴�
		if (true == hammer_attack_col)
		{
			static _int hammer_col_number = 0;
			ImGui::SliderInt("Number", &hammer_col_number, 0, (_int)HAMMER_COLLIDER_END - 1);

			ImGui::Separator();

			HAMMER_COLLIDER hammer_collider_type = (HAMMER_COLLIDER)hammer_col_number;
			switch (hammer_collider_type)
			{
			case Client::CPlayer_Weapon::HAMMER_COLLIDER_HEAD:
				ImGui::Text("Selected Hammer Collider : [ HEAD ]");
				break;
			case Client::CPlayer_Weapon::HAMMER_COLLIDER_WAVE:
				ImGui::Text("Selected Hammer Collider : [ WAVE ]");
				break;
			case Client::CPlayer_Weapon::HAMMER_COLLIDER_LAST:
				ImGui::Text("Selected Hammer Collider : [ LAST ]");
				break;
			case Client::CPlayer_Weapon::HAMMER_COLLIDER_FINISH:
				ImGui::Text("Selected Hammer Collider : [ FINISH ]");
				break;
			}

			ImGui::Separator();

			_float3 scale = m_pCollider_HammerForm[hammer_collider_type]->Get_Scale();

			ImGui::DragFloat("Collider Scale", &scale.x, 0.05f);

			m_pCollider_HammerForm[hammer_collider_type]->Set_Scale(_float3(scale.x, scale.x, scale.x));

			_float4 position = m_pCollider_HammerForm[hammer_collider_type]->Get_Translation();
			_float position_f[3] = { position.x, position.y, position.z };

			ImGui::DragFloat3("Collider Position", position_f, 0.05f);

			m_pCollider_HammerForm[hammer_collider_type]->Set_Translation(_float4(position_f[0], position_f[1], position_f[2], 1.f));
			ImGui::Separator();
		}

		// �ظ� ���� �ݶ��̴�
		if (true == spot_col)
		{
			static _int spot_col_number = 0;
			ImGui::SliderInt("Number", &spot_col_number, 0, (_int)HAMMER_SPOT_END - 1);

			ImGui::Separator();

			HAMMER_SPOT spot_collider_type = (HAMMER_SPOT)spot_col_number;
			switch (spot_collider_type)
			{
			case Client::CPlayer_Weapon::HAMMER_SPOT_HEAD:
				ImGui::Text("Selected Spot Collider : [ HEAD ]");
				break;
			case Client::CPlayer_Weapon::HAMMER_SPOT_BODY:
				ImGui::Text("Selected Spot Collider : [ BODY ]");
				break;
			case Client::CPlayer_Weapon::HAMMER_SPOT_BOOSTER:
				ImGui::Text("Selected Spot Collider : [ BOOSTER ]");
				break;
			}
			
			_float3 scale = m_pCollider_HammerHeadPoint[spot_collider_type]->Get_Scale();

			ImGui::DragFloat("Collider Scale", &scale.x, 0.05f);

			m_pCollider_HammerHeadPoint[spot_collider_type]->Set_Scale(_float3(scale.x, scale.x, scale.x));

			_float4 position = m_pCollider_HammerHeadPoint[spot_collider_type]->Get_Translation();
			_float position_f[3] = { position.x, position.y, position.z };

			ImGui::DragFloat3("Collider Position", position_f, 0.05f);

			m_pCollider_HammerHeadPoint[spot_collider_type]->Set_Translation(_float4(position_f[0], position_f[1], position_f[2], 1.f));
			ImGui::Separator();
		}
	}

	//// ������ ���� ��Ʈ��
	//LEVEL current_level = (LEVEL)m_pGameInstance->Get_LevelIndex();
	//if (LEVEL_TEST == current_level || LEVEL_TWILIGHT == current_level)
	//{
	//	ImGui::Separator();
	//	ImGui::Separator();

	//	if (WEAPON_TYPE_HYPER_HAMMER_CHANGING == m_eWeaponType)
	//	{
	//		static _int selected_meshcontainer = 0;
	//		_int number_of_meshcontainer = (_int)m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Get_NumberOfMeshContainers() - 1;

	//		ImGui::SliderInt("Container Number", &selected_meshcontainer, 0, number_of_meshcontainer);
	//		ImGui::Text("Selected MeshContainer : %d / %d", selected_meshcontainer, number_of_meshcontainer);
	//		ImGui::Text("Size of Skipping List : %d", (_int)m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Size_SkipList());

	//		if (true == ImGui::Button("Apply Skipping Number"))
	//		{
	//			m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Add_SkipList((_uint)selected_meshcontainer);
	//		}

	//		if (true == ImGui::Button("Clear Skipping Number"))
	//		{
	//			m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Clear_SkipList();
	//		}
	//	}
	//}

	ImGui::End();
}

HRESULT CPlayer_Weapon::Render_Shadow()
{
	if (nullptr == m_pModelCom)
	{
		BREAKPOINT;
		return E_FAIL;
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_bool anim_shader = Check_Anim_NonAnim();
	CShader* target_shader = nullptr;
	if (true == anim_shader)
		target_shader = m_pShaderCom_Anim;
	else
		target_shader = m_pShaderCom_NonAnim;

	if (nullptr == target_shader)
	{
		_breakpoint;
		return E_FAIL;
	}

	if (FAILED(m_pTransformCom->Bind_OnShader(target_shader, "g_WorldMatrix")))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(target_shader->Set_RawValue("g_LightViewMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightViewMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	if (FAILED(target_shader->Set_RawValue("g_LightProjMatrix", &pGameInstance->Get_LightMatrix(CLight_Manager::SHADOW_DYNAMIC).LightProjMatrix, sizeof(_float4x4))))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	_uint iNumMaterials = m_pModelCom[m_eWeaponType]->Get_NumberOfMaterials();
	_uint iPass = 14;

	for (_uint i = 0; i < iNumMaterials; ++i)
	{
		if (true == anim_shader)
		{
			iPass = 14;

			m_pModelCom[m_eWeaponType]->Render(target_shader, iPass, i, "g_BoneMatrices");
		}
		else
		{
			iPass = 45;
			m_pModelCom[m_eWeaponType]->Render(target_shader, iPass, i);
		}
	}

	_float fLUTMask = 0.f;
	if (true == anim_shader)
	{
		if (FAILED(target_shader->Set_RawValue("g_fLUTMask", &fLUTMask, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}

	return S_OK;
}

void CPlayer_Weapon::Change_Animation(_uint iIndex)
{
	if (WEAPON_TYPE_END == m_eWeaponType)
		_breakpoint;

	m_pModelCom[m_eWeaponType]->Set_TempIndex(iIndex);
}

void CPlayer_Weapon::Reset_Animation()
{
	_uint curr_anim = m_pModelCom[m_eWeaponType]->Get_CurrAnimIndex();
	m_pModelCom[m_eWeaponType]->ResetAnimation(curr_anim);
}

HRESULT CPlayer_Weapon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	/* For.Com_Model */
	{
		// Kingdom Key
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Kingdom_Key"),
			m_pModelComponentTag[WEAPON_TYPE_KINGDOM_KEY], (CComponent**)&m_pModelCom[WEAPON_TYPE_KINGDOM_KEY])))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pModelCom[WEAPON_TYPE_KINGDOM_KEY]->Add_OnceListAll();

		vector<CAnimation*>* kingdom_key_animations = m_pModelCom[WEAPON_TYPE_KINGDOM_KEY]->Get_Animations();
		if (nullptr == kingdom_key_animations)
			_breakpoint;

		for (auto animation : (*kingdom_key_animations))
			animation->Set_PlaySpeed_Rate(1.25);

		// Favority Deputy
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Favorite_Deputy"),
			m_pModelComponentTag[WEAPON_TYPE_FAVORITE_DEPUTY], (CComponent**)&m_pModelCom[WEAPON_TYPE_FAVORITE_DEPUTY])))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		// Hyper Hammer
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hyper_Hammer"),
			m_pModelComponentTag[WEAPON_TYPE_HYPER_HAMMER], (CComponent**)&m_pModelCom[WEAPON_TYPE_HYPER_HAMMER])))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Add_OnceListAll();

		vector<CAnimation*>* hyper_hammer_animations = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER]->Get_Animations();
		if (nullptr == hyper_hammer_animations)
			_breakpoint;

		for (auto animation : (*hyper_hammer_animations))
			animation->Set_PlaySpeed_Rate(1.25);

		// Hyper Hammer Changing
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Model_Hyper_Hammer_Changing"),
			m_pModelComponentTag[WEAPON_TYPE_HYPER_HAMMER_CHANGING], (CComponent**)&m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING])))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Add_OnceListAll();

		vector<CAnimation*>* hyper_hammer_changing_animations = m_pModelCom[WEAPON_TYPE_HYPER_HAMMER_CHANGING]->Get_Animations();
		if (nullptr == hyper_hammer_changing_animations)
			_breakpoint;

		for (auto animation : (*hyper_hammer_changing_animations))
			animation->Set_PlaySpeed_Rate(1.25);
	}
	
	/* For.Com_Status */
	CStatus::STATUSDESC StatusDesc;
	StatusDesc.fMaxHP   = FLT_MAX;
	StatusDesc.fHP      = FLT_MAX;
	StatusDesc.fDamaged = 5.f;
	StatusDesc.iLevel   = 1;
	StatusDesc.fEXP     = 0.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Status"), TEXT("Com_Status"), (CComponent**)&m_pStatusCom, &StatusDesc)))
		return E_FAIL;

	/* For.Com_Shader */
	{
		// Anim
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
			TEXT("Com_Shader_Anim"), (CComponent**)&m_pShaderCom_Anim)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		// Non Anim
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_NonAnimModel"), 
			TEXT("Com_Shader_NonAnim"), (CComponent**)&m_pShaderCom_NonAnim)))
		{
			BREAKPOINT;
			return E_FAIL;
		}
	}
	
	CCollider::COLLIDERDESC collider_desc;
	ZeroMemory(&collider_desc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_Collider */
	{
		// ��� �� ���� �ݶ��̴�
		collider_desc.vScale = _float3(m_fNormalCollider_Scale);
		collider_desc.vTranslation = m_fNormalCollider_Position;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), 
			m_pNormalCollider_ComponentTag, (CComponent**)&m_pCollider_NormalAttack, &collider_desc)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		// �ظ� �� ���� �ݶ��̴�
		for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		{
			collider_desc.vScale = m_fHammerCollider_LocalScale[i];
			collider_desc.vTranslation = m_vHammerCollider_LocalPosition[i];

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
				m_pHammerCollider_ComponentTag[i], (CComponent**)&m_pCollider_HammerForm[i], &collider_desc)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}

		// �ظ� �� ���� �ݶ��̴�
		for (_uint i = 0; i < HAMMER_SPOT_END; ++i)
		{
			collider_desc.vScale = m_fHammer_HeadPoint_LocalScale;
			collider_desc.vTranslation = m_vHammer_HeadPoint_LocalPosition[i];

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
				m_pHammer_HeadPoint_ComponentTag[i], (CComponent**)&m_pCollider_HammerHeadPoint[i], &collider_desc)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}

		// ������ �� ���� �ݶ��̴�
		for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		{
			if (RAGE_COLLIDER_FRONT == i)
				collider_desc.vTranslation = _float4(0.f, 0.f, -1.f, 1.f);
			else
				collider_desc.vTranslation = _float4(0.f, 0.f, 0.f, 1.f);

			collider_desc.vScale = _float3(m_fRageCollider_LocalScale[i]);

			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"), 
				m_pRageCollider_ComponentTag[i], (CComponent**)&m_pCollider_RageForm[i], &collider_desc)))
			{
				BREAKPOINT;
				return E_FAIL;
			}
		}
	}
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	m_pTrail = static_cast<CEffect_VISwordTrail*>(pGameInstance->Add_GameObject_And_Return(GM->Get_CurrentLevel(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_VISwordTrail"), &m_pTransformCom));

	// RageForm Texture
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Copmonent_Texture_Weapon_RageForm"), TEXT("Com_Texture"), (CComponent**)&m_pRageFormTex)))
	{
		BREAKPOINT;
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_Weapon::SetUp_ConstantTable(_bool* pAnim)
{
	CShader* target_shader = nullptr;
	if (true == Check_Anim_NonAnim())
	{
		target_shader = m_pShaderCom_Anim;
		*pAnim = true;
	}
	else
		target_shader = m_pShaderCom_NonAnim;

	if (FAILED(m_pTransformCom->Bind_OnShader(target_shader, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(target_shader->Set_RawValue("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(target_shader->Set_RawValue("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	// Anim Shader
	if (true == (*pAnim))
	{
		// ������ �ؽ�ó ���ε�
		if (FAILED(m_pRendererCom->Get_NoiseTexture()->SetUp_OnShader(target_shader, "g_DissolveTexture", 4)))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float4 vColor = _float4(0.f, 0.f, 0.f, 1.f);
		if (FAILED(target_shader->Set_RawValue("g_vDissolveColor1", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(target_shader->Set_RawValue("g_vDissolveColor2", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}
		if (FAILED(target_shader->Set_RawValue("g_vDissolveColor3", &vColor, sizeof(_float4))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float fDissolve = m_pPlayer->Get_Dissolve();
		if (FAILED(target_shader->Set_RawValue("g_fDissolveAlpha", &fDissolve, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		_float fLUTMask = 1.f;
		if (FAILED(target_shader->Set_RawValue("g_fLUTMask", &fLUTMask, sizeof(_float))))
		{
			BREAKPOINT;
			return E_FAIL;
		}

		target_shader->Set_RawValue("g_isBlur", &m_isUsingBlur, sizeof(_bool));
	}

	// Non-Anim Shader
	else
	{
		target_shader->Set_RawValue("g_iBlur", &m_isUsingBlur, sizeof(_bool));
	}

	target_shader->Set_RawValue("g_isRim", &m_isUsingRim, sizeof(_bool));
	target_shader->Set_RawValue("g_fRimPower", &m_fRimPower, sizeof(_float));
	target_shader->Set_RawValue("g_fRimRange", &m_fRimRange, sizeof(_float));
	target_shader->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4));

	return S_OK;
}

void CPlayer_Weapon::OnCollision(CCollider * pCollider)
{
	auto pColliderList = m_pGameInstance->Get_ColliderList(string("Monster_Hit"));
	for (auto& pDestCollider : *pColliderList)
	{
		CGameObject* pDestObject = pDestCollider->Get_Parent();

		if (pDestObject->Get_ReadyToDie() == true)
			continue;

		// �浹 enter�� ������ �ʾҴٸ� continue
		if (false == m_pGameInstance->Collision_Enter_Sphere(pCollider, pDestCollider))
			continue;

		// �ߺ��浹 �˻縦 ���� �غ�
		CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
		_bool duplicated = false;
		_uint collider_index = UINT_MAX;

		switch (curr_form)
		{
			// ��� �� ������ ��
		case Client::CPlayer::SF_NORMAL:

			for (auto pParent : m_CollisionParents)
			{
				if (pParent == pDestObject)
				{
					duplicated = true;
					break;
				}
			}

			if (false == duplicated)
				m_CollisionParents.push_back(pDestObject);
			else
				continue;

			break;

			// �ظ� �� ������ ��
		case Client::CPlayer::SF_HAMMER:

			// � �ظ� �� �ݶ��̴��� �ε��������� ã�� �ε����� �޾Ƶд�
			for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
			{
				if (pCollider == m_pCollider_HammerForm[i])
				{
					collider_index = i;
					break;
				}
			}

			// �ش� �ε����� parent���Ϳ� �浹�� ����� �˻��ؼ� �ߺ� �浹 ������� Ȯ��
			for (auto pParent : m_CollisionParents_Hammer[collider_index])
			{
				if (pParent == pDestObject)
				{
					duplicated = true;
					break;
				}
			}

			// �ߺ����� �ʾҴٸ� ����� parent���Ϳ� ����
			if (false == duplicated)
				m_CollisionParents_Hammer[collider_index].push_back(pDestObject);
			else
				continue;

			break;
		case Client::CPlayer::SF_RAGE:

			// � ������ �� �ݶ��̴��� �ε��������� ã�� �ε����� �޾Ƶд�
			for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
			{
				if (pCollider == m_pCollider_RageForm[i])
				{
					collider_index = i;
					break;
				}
			}

			// �ش� �ε����� parent���Ϳ� �浹�� ����� �˻��ؼ� �ߺ� �浹 ������� Ȯ��
			for (auto pParent : m_CollisionParents_Rage[collider_index])
			{
				if (pParent == pDestObject)
				{
					duplicated = true;
					break;
				}
			}

			// �ߺ����� �ʾҴٸ� ����� parent���Ϳ� ����
			if (false == duplicated)
				m_CollisionParents_Rage[collider_index].push_back(pDestObject);
			else
				continue;

			break;
		}
		
		// ����Ʈ �Ͷ߸� ��ġ�� ���
		_vector direction_from_target_to_attack_collider = _vec3norm(pCollider->Get_Center() - pDestCollider->Get_Center());
		_float radius = pDestCollider->Get_Sphere()->Radius;

		_vector effect_point = pDestCollider->Get_Center() + (direction_from_target_to_attack_collider * radius);
		_float4 effect_point_f4;
		XMStoreFloat4(&effect_point_f4, XMVectorSetW(effect_point, 1.f));

		// ����Ʈ ����
		switch (m_eWeaponType)
		{
		case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

			CEffect_Manager::GetInstance()->Create_HitStar(&effect_point_f4, 0.5f);

			break;
		case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:
		case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:

			CEffect_Manager::GetInstance()->Create_HitStar(&effect_point_f4, 0.5f);
			CEffect_Manager::GetInstance()->Create_HammerForm_AttackEffect_Beads(effect_point);
			CEffect_Manager::GetInstance()->Create_HammerForm_AttackEffect_Rings(m_pTransformCom, effect_point);

			break;
		}

		CPlayer::SORA_ANIM curr_anim = (CPlayer::SORA_ANIM)m_pPlayerModel->Get_CurrAnimIndex_EachBody(CPlayer::SB_BODY);

		// Ÿ�� ���� ���
		_bool last_combo = false;
		last_combo = m_pPlayer->Anim_Check_LastCombo(curr_anim);

		m_pPlayer->Play_AttackHitSound(last_combo);

		// ������ ����
		static_cast<CMonster*>(pDestObject)->Damaged(pCollider, m_pStatusCom->Get_Damage());

		// Ÿ�ݰ��� ���� �浹 ���� �ӵ� ��������
		if (false == m_pPlayer->Check_OnFinish() && CPlayer::SORA_FORM::SF_RAGE != curr_form)
		{
			switch (m_eWeaponType)
			{
			case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:

				// Ÿ�ݰ��� ���� �浹 ���� �ӵ� ��������
				GM->HitSpeedControl(0.1f, 0.05f, 0.5f);

				break;
			case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:

				// Ÿ�ݰ��� ���� �浹 ���� �ӵ� ��������
				GM->HitSpeedControl(0.1f, 0.05f, 0.5f);

				break;
			case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:

				// Ÿ�ݰ��� ���� �浹 ���� �ӵ� ��������
				GM->HitSpeedControl(0.1f, 0.05f, 1.f);

				break;
			}
		}
		

		// ù �޺��� ��� �޺� �� UI�� ����
		if (0 >= m_pPlayer->Get_Combo())
			NM->Create_ComboBar();

		// �޺� Ƚ�� ����
		m_pPlayer->Add_Combo();

		// ���Ͱ� ���� �ʾ��� ��
		if (pDestObject->Get_ReadyToDie() == false)
		{
			// �÷��̾��� Attack Ÿ���� �浹�� �ݶ��̴��� ����
			m_pPlayer->Set_AttackTarget(pDestCollider);

			MONSTERTYPE target_monster_type = (MONSTERTYPE)static_cast<CMonster*>(pDestObject)->Get_MonsterType();
			Decide_KnockBack(pDestObject, target_monster_type);
		}

		// ���Ͱ� �׾��� ��
		else
		{
			// Ÿ�ٿ��� ����
			m_pPlayer->Eliminate_DeadTarget(pDestObject);
			pCollider->Erase_Colliding_All(pDestCollider);
		}
	}
}

void CPlayer_Weapon::Decide_KnockBack(CGameObject* pMonster, MONSTERTYPE eMonsterType)
{
	switch (eMonsterType)
	{
		// ���� ����, ũ�ο� ����, �� ����, ���̴� ����, ��Ű : �Ϲ� ����. ������� �˹� ������ ó��
	case Client::MONSTERTYPE_SKULLSOLDIER:
	case Client::MONSTERTYPE_CROWSOLDIER:
	case Client::MONSTERTYPE_THINSOLDIER:
	case Client::MONSTERTYPE_SHININGEGG:
	case Client::MONSTERTYPE_MONKEY:

		m_fKnockBackPower *= 1.f;
		m_dKnockBackTimer *= 1.0;

		break;

		// ��Ʈ : ���� ���� ����. �������� �˹� ���·� ó��
	case Client::MONSTERTYPE_GOAT:

		m_fKnockBackPower *= 1.f;
		m_dKnockBackTimer *= 0.5;

		break;

		// ��ٵ�, ����Ʈ�޾� : ���� ����. �������� �˹� ���·� ó��
	case Client::MONSTERTYPE_NOBODY:
	case Client::MONSTERTYPE_NIGHTMARE:

		m_fKnockBackPower *= 0.5f;
		m_dKnockBackTimer *= 0.75;

		break;

		// ---------------------------------------------------------------------

		// ���� : ������ �Ϲ� ����. ó������ ����
	case Client::MONSTERTYPE_MOWER:

		m_fKnockBackPower *= 0.f;
		m_dKnockBackTimer *= 0.0;

		break;

		// �� Ʈ��, ���̽� ��, ������ ���� : ������ ���� ����. ó������ ����
	case Client::MONSTERTYPE_BIGTREE:
	case Client::MONSTERTYPE_ICEMAN:
	case Client::MONSTERTYPE_FROZENWOLF:

		m_fKnockBackPower *= 0.f;
		m_dKnockBackTimer *= 0.0;

		break;

		// ���� ���� : �Ҹ� ����. ó������ ����
	case Client::MONSTERTYPE_SUMMONWOLF:

		m_fKnockBackPower *= 0.f;
		m_dKnockBackTimer *= 0.0;

		break;
	}

	_vector player_position = m_pPlayer->Get_Transform()->Get_Position();
	_vector target_position = pMonster->Get_Transform()->Get_Position();

	vecy(player_position) = 0.f;
	vecy(target_position) = 0.f;

	_vector knockback_direction = _vec3norm(target_position - player_position);

	pMonster->Get_Transform()->KnockBackInput_v(knockback_direction, m_fKnockBackPower, m_dKnockBackTimer);
}

_bool CPlayer_Weapon::Check_Anim_NonAnim()
{
	_bool return_value = false;
	switch (m_eWeaponType)
	{
	case Client::CPlayer_Weapon::WEAPON_TYPE_KINGDOM_KEY:
	case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER_CHANGING:
	case Client::CPlayer_Weapon::WEAPON_TYPE_HYPER_HAMMER:

		return_value = true;

		break;
	case Client::CPlayer_Weapon::WEAPON_TYPE_FAVORITE_DEPUTY:

		return_value = false;

		break;
	}

	return return_value;
}

_vector CPlayer_Weapon::Calculate_Point_On_Ground(_uint iColliderKind, _uint iNumber)
{
	LEVEL curr_level = GM->Get_CurrentLevel();

	if (iColliderKind > 3)
		return XMVectorZero();

	CCollider* target_collider = nullptr;

	switch (iColliderKind)
	{
	case 0: // normal

		target_collider = m_pCollider_NormalAttack;

		break;
	case 1: // rage

		if (iNumber >= RAGE_COLLIDER_END)
			return XMVectorZero();

		target_collider = m_pCollider_RageForm[(RAGE_COLLIDER)iNumber];

		break;
	case 2: // hammer

		if (iNumber >= HAMMER_COLLIDER_END)
			return XMVectorZero();

		target_collider = m_pCollider_HammerForm[(HAMMER_COLLIDER)iNumber];

		break;
	case 3: // hammer spot

		if(iNumber >= HAMMER_SPOT_END)
			return XMVectorZero();

		target_collider = m_pCollider_HammerHeadPoint[(HAMMER_SPOT)iNumber];

		break;
	}

	if (nullptr == target_collider)
	{
		_breakpoint;
		return XMVectorZero();
	}

	_vector center_position = target_collider->Get_Center();

	switch (curr_level)
	{
	case LEVEL_TWILIGHT:
	case LEVEL_RAPUNZEL:
	case LEVEL_FROZEN:
	case LEVEL_KEYGRAVE:
	case LEVEL_FINALBOSS:
		
	{
		// Navi
		CCell* curr_cell = m_pPlayer->Get_Navigation()->Get_CurrentCell();

		if (nullptr == curr_cell)
			_breakpoint;

		
		_float height_on_cell = curr_cell->Compute_Y(center_position);
		vecy(center_position) = height_on_cell;
	}

	break;
	case LEVEL_RAPUNZEL_BOSS:

	{
		// Terrain
		CTerrain* curr_terrain = static_cast<CTerrain*>(m_pGameInstance->Get_Back(curr_level, TEXT("Layer_Terrain")));

		if (nullptr == curr_terrain)
			_breakpoint;

		center_position = curr_terrain->Compute_PositionOnTerrain(center_position);
	}
		
		break;
	}

	return center_position;
}

CGameObject* CPlayer_Weapon::Create_Effect_HammerEmbers()
{
	return CEffect_Manager::GetInstance()->Create_HammerForm_Effect_Swing_HeadEmbers(m_pTransformCom, &m_vHammer_HeadPoint_Position[HAMMER_SPOT_BOOSTER]);
}

HRESULT CPlayer_Weapon::Create_Effect_HammerSwing()
{
	HRESULT result = S_OK;

	result = CEffect_Manager::GetInstance()->Create_HammerForm_Effect_Swing_HeadStream(m_pTransformCom, &m_Hammer_HeadPoint_Matrix[HAMMER_SPOT_HEAD], 1);
	result = CEffect_Manager::GetInstance()->Create_HammerForm_Effect_Swing_HeadAirCut(m_pTransformCom, &m_Hammer_HeadPoint_Matrix[HAMMER_SPOT_HEAD]);

	return result;
}

HRESULT CPlayer_Weapon::Create_Effect_AirHit()
{
	HRESULT result = S_OK;

	_vector player_look_norm = _vec3norm(m_pPlayerTransform->Get_Look());

	result = CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_Spike(m_pTransformCom, m_pCollider_HammerHeadPoint[HAMMER_SPOT_HEAD]->Get_Center(), player_look_norm);
	result = CEffect_Manager::GetInstance()->Create_HammerForm_LastAttack_Circle(m_pTransformCom, m_pCollider_HammerHeadPoint[HAMMER_SPOT_HEAD]->Get_Center(), player_look_norm);

	return result;
}

HRESULT CPlayer_Weapon::Create_Effect_AirCut()
{
	HRESULT result = S_OK;

	_vector effect_look_norm = _vec3norm(m_pCollider_HammerHeadPoint[HAMMER_SPOT_HEAD]->Get_Center() - m_pCollider_HammerHeadPoint[HAMMER_SPOT_BOOSTER]->Get_Center());

	result = CEffect_Manager::GetInstance()->Create_HammerForm_Effect_Swing_AirCut(m_pTransformCom, m_pCollider_HammerHeadPoint[HAMMER_SPOT_BODY]->Get_Center(), effect_look_norm);

	return result;
}

void CPlayer_Weapon::Clear_All_Colliding_Data()
{
	CPlayer::SORA_FORM curr_form = m_pPlayer->Get_Form();
	switch (curr_form)
	{
	case Client::CPlayer::SF_NORMAL:

		m_bRegisterCollider = false;
		m_pCollider_NormalAttack->Clear_Colliding_All();

		break;
	case Client::CPlayer::SF_HAMMER:

		for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		{
			m_bRegisterCollider_Hammer[i] = false;
			m_pCollider_HammerForm[i]->Clear_Colliding_All();
		}

		break;
	case Client::CPlayer::SF_RAGE:

		for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		{
			m_bRegisterCollider_Rage[i] = false;
			m_pCollider_RageForm[i]->Clear_Colliding_All();
		}

		break;
	}
	
	m_CollisionParents.clear();
}

void CPlayer_Weapon::Appear_Hammer()
{
	m_bAppear = true;
	m_dTimer_Control_RimBrightness = 0.0;
	m_dTimer_Control_RimBrightness_Max = 100.0;

	m_fRimPower = 0.f;
	m_vRimColor = _float4(1.f);

	m_isUsingRim = true;
}

CPlayer_Weapon * CPlayer_Weapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer_Weapon*	pInstance = new CPlayer_Weapon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSGBOX("Failed to Created CPlayer_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CPlayer_Weapon::Clone(void * pArg)
{
	CPlayer_Weapon*	pInstance = new CPlayer_Weapon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSGBOX("Failed to Cloned CPlayer_Weapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CPlayer_Weapon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom_Anim);
	Safe_Release(m_pShaderCom_NonAnim);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pStatusCom);

	for (_uint i = 0; i < WEAPON_TYPE_END; ++i)
		Safe_Release(m_pModelCom[i]);

	Safe_Release(m_pCollider_NormalAttack);

	Safe_Release(m_pRageFormTex);

	for (_uint i = 0; i < RAGE_COLLIDER_END; ++i)
		Safe_Release(m_pCollider_RageForm[i]);

	for (_uint i = 0; i < HAMMER_COLLIDER_END; ++i)
		Safe_Release(m_pCollider_HammerForm[i]);

	for(_uint i = 0; i < HAMMER_SPOT_END; ++i)
		Safe_Release(m_pCollider_HammerHeadPoint[i]);
}
