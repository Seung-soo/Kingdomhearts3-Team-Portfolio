#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"
#include "Goat.h"
#include "GoatWeapon.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_GoatAtk final : public CState
{
private:
	explicit CState_GoatAtk();
	virtual ~CState_GoatAtk() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState();
	virtual HRESULT ClearState();
	virtual _bool	Action(_double TimeDelta);

private: // �Ϲ� ����
	_bool Atk_Spear(_double TimeDelta);					// ���������� ���
	_bool Atk_FlySwing(_double TimeDelta);				// ���� ��¦ ���Ҵٰ� �밢�� �Ʒ��� ����
	_bool Atk_DashSwing(_double TimeDelta);				// ���������� ���Ҵٰ� ����
	_bool Atk_SpinDashSwing(_double TimeDelta);			// Į �����鼭 ���ƿ��ٰ� �������� ũ�� ����
	_bool Atk_JumpDashSwing(_double TimeDelta);			// �ڷ� ��¦ ���ٰ� ������ ���ƿ��鼭 ���� Į ������
	_bool Atk_UnderSwing(_double TimeDelta);			// ���� ���� ������ ���Ҵٰ� �Ʒ��� ���
	_bool Atk_LongUnderAtk(_double TimeDelta);			// ũ�� ���� ���Ƽ� �Ʒ��� ��� �������
	_bool Atk_ConsecutiveSwing(_double TimeDelta);		// ���ƿ��鼭 ���� ����
	_bool Atk_MagicShot(_double TimeDelta);				// ��������
	_bool Atk_ShortPillarOfFire(_double TimeDelta);		// ª�� �ұ�� ����
	_bool Atk_GravityPillarOfFire(_double TimeDelta);	// ����ü�����ų� ����� ���� ����
	_bool Atk_PillarOfFire(_double TimeDelta);			// �ұ�� ����

private: // �ڷ���Ʈ ����
	_bool Atk_TeleportSpear(_double TimeDelta);			// ���������� ���
	_bool Atk_ConsecutiveSpear(_double TimeDelta);		// ��� �����̵� �ϸ鼭 ���� ���
	_bool Atk_TeleportFlySwing(_double TimeDelta);		// ���� ��¦ ���Ҵٰ� �밢�� �Ʒ��� ����
	_bool Atk_TeleportDashSwing(_double TimeDelta);		// ���������� ���Ҵٰ� ����
	_bool Atk_TeleportSpinDashSwing(_double TimeDelta);	// Į �����鼭 ���ƿ��ٰ� �������� ũ�� ����
	_bool Atk_TeleportUnderSwing(_double TimeDelta);	// ���� ���� ������ ���Ҵٰ� �Ʒ��� ���
	_bool Atk_TeleportLongUnderAtk(_double TimeDelta);	// ũ�� ���� ���Ƽ� �Ʒ��� ��� �������
	
private:
	_bool	Turn_ExceptAnim(_uint iExceptAnim, _double TimeDelta); // Ư�� �ִϸ��̼��� �ƴ� �� Turn
	_bool	GoStraight_Limit(_float fDistanceLimit, _double TimeLimit, _double TimeDelta); // Ư�� �Ÿ�, Ư�� �ð��� �Ǹ� ����

private:
	_tchar* GetRandomSwingSound() { return m_SwingSound[rand() % 4]; }
	_tchar* GetRandomSpearSound() { return m_SpearSound[rand() % 5]; }

private:
	MONSTERTYPE		m_eMonsterType         = MONSTERTYPE_GOAT;
	CGoat*			m_pGoat                = nullptr;
	CGoatWeapon*	m_pGoatWeapon          = nullptr;
	CTransform*		m_pGoatWeaponTransform = nullptr;

	CTransform*		m_pPlayerTransform   = nullptr;
	CTransform*		m_pCameraTransform   = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	_vector m_vCamLook;
	_vector m_vCamBack;
	_vector m_vCamLeft;
	_vector m_vCamRight;

	_uint m_iLoopAcc = 0;

	// ���Ӱ����� �� ����� ���⺤��
	vector<_vector> m_vConsecutiveDir = {
		__vector(+0.f, +0.f, +1.f, +0.f),	// ����
		__vector(+0.f, +1.f, +1.f, +0.f),	// ���� ��
		__vector(+1.f, +1.f, +0.f, +0.f),	// ���� ��
		__vector(-1.f, +1.f, +0.f, +0.f),	// ���� ��
		__vector(+1.f, +1.f, +1.f, +0.f),	// ���� ���� ��
		__vector(-1.f, +1.f, +1.f, +0.f),	// ���� ���� ��
		__vector(+0.f, +0.f, -1.f, +0.f),	// �ĸ�
		__vector(+0.f, +1.f, -1.f, +0.f),	// �ĸ� ��
		__vector(+1.f, +1.f, -1.f, +0.f),	// �ĸ� ���� ��
		__vector(-1.f, +1.f, -1.f, +0.f),	// �ĸ� ���� ��
		__vector(+0.f, +1.f, +0.f, +0.f)	// �׳� ��
	};

	CGoat::GOAT_ATK m_eGoatSkill = CGoat::GOAT_ATK_END;

	_double m_TimeAcc           = 0.0;
	_double m_EffectIntervalAcc = 0.0;

	_uint m_iAtkLoopAcc = 0;

	_bool m_bSkillEnd[3]  = { false, };
	_bool m_bSoundEnd[3]  = { false, };
	_bool m_bEffectEnd[5] = { false, };

	// ����Ʈ ��ü ���� ����
	vector<CGameObject*> m_EffectObjects;
	_uint iIndex = 0;

	// ������� ���� ä�ΰ� ��������
	_uint m_iCurrentSoundChannel  = 0;
	_tchar* m_SwingSound[4] = { TEXT("Goat_39_Swing.wav"), TEXT("Goat_40_Swing.wav"), TEXT("Goat_41_Swing.wav"), TEXT("Goat_42_Swing.wav") };
	_tchar* m_SpearSound[5] = { TEXT("Goat_7_Spear.wav"), TEXT("Goat_8_Spear.wav"), TEXT("Goat_9_Spear.wav"), TEXT("Goat_10_Spear.wav"), TEXT("Goat_11_Spear.wav") };

private:
	vector<_uint> m_Spear               = { 22, 23, 24 };
	vector<_uint> m_ConsecutiveSpear    = { 22, 23, 24 };
	vector<_uint> m_FlySwing            = { 5, 9, 25 };
	vector<_uint> m_DashSwing           = { 26, 27, 28 };
	vector<_uint> m_SpinDashSwing       = { 29, 30, 31 };
	vector<_uint> m_JumpDashSwing       = { 32, 33, 34 };
	vector<_uint> m_UnderSwing          = { 5, 9, 35 };
	vector<_uint> m_LongUnderAtk        = { 37 };
	vector<_uint> m_ConsecutiveSwing    = { 31, 28, 30, 31, 24, 35 };
	vector<_uint> m_MagicShot           = { 36 };
	vector<_uint> m_ShortPillarOfFire   = { 40 };
	vector<_uint> m_GravityPillarOfFire = { 37, 40, 40, 40, 40 };
	vector<_uint> m_PillarOfFire        = { 37 };

public:
	static CState_GoatAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END