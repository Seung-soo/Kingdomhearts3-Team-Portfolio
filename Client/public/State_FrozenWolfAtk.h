#pragma once
#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"
#include "FrozenWolf.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_FrozenWolfAtk final : public CState
{
private:
	explicit CState_FrozenWolfAtk();
	virtual ~CState_FrozenWolfAtk() = default;

private: 
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState(); // �� ���°� ���¸ӽſ� ���ε��� �� ���¸ӽ��� BindState �Լ����� �� �� ��������ִ� �Լ�
	virtual _bool	Action(_double TimeDelta); // ��ü�� �� Tick ���� ���� �Լ�

private: 
	_bool Atk_Dash(_double TimeDelta);
	_bool Atk_Scratch(_double TimeDelta);
	_bool Atk_SummonWolf(_double TimeDelta);
	_bool Atk_SpinAtk(_double TimeDelta);
	_bool Atk_ThrowFire(_double TimeDelta);
	_bool Atk_PurpleRain(_double TimeDelta);

private: 
	void ChasePlayer_ChangeAnim(_double TimeDelta, _uint iAnimIndex);
	_bool Summon_LittleWolves(_double TimeDelta, _double Interval);

private:
	// �θ�Ŭ������ State �� GameObejct, Model, Transform �� �̸� ���ǵǾ�����
	CFrozenWolf*	m_pFrozenWolf= nullptr;
	MONSTERTYPE		m_eMonsterType = MONSTERTYPE_BIGTREE;
	CTransform*		m_pPlayerTransform = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	CFrozenWolf::WOLF_SKILL m_eFrozenWolfSkill = CFrozenWolf::WOLF_SKILL_END;

	// �ð�������, ����Ʈ ���ຯ��, ���� ���ຯ�� (���߿� �ʿ��� ���� ����ؼ� �̸� �߰��ص�)
	_double m_TimeAcc = 0.0;
	//_bool	m_bEffectEnd[3] = { false, };
	//_bool	m_bSoundEnd[3] = { false, };

	// �����ؼ� ������ ���� �Ŀ� (���������� �� ����� ����)
	_double m_GoStraightPower;

private:
	vector<_uint> m_Dash = { 29, 30, 31 };
	vector<_uint> m_Scratch = { 27, 28 };
	vector<_uint> m_SummonWolf = { 38 };
	vector<_uint> m_SpinAttack = { 32, 33, 34, 35, 36};
	vector<_uint> m_TrhowFire = { 37 };
	vector<_uint> m_PurpleRain = { 39 };

	_uint		  m_iCurDashPoint = 0;
	_uint		  m_iPreDashPoint = 0;

	_double		  m_DashTime = 5.0;

	_uint		  m_iDashCnt = 0;

	_bool		  m_isReadyToDash = false;
	_bool		  m_isReadyToBite = false;

	_bool		  m_isTurning = false;

	_vector		  m_vDashPoint[8];
	_uint		  m_iDashIndex = 0;

	_bool		  m_isHeadingPoint = false;

	_vector		  m_vTargetingPos;

	_int		  m_iRevolutionDir = 1;
	_bool		  m_bCalRevolutionDir = false;

	_bool		  m_isSummonWolf = false;

	_double		  m_TimeAccum_Sum = 0.0;

	_int		  m_iLittleWolfCnt = 0;

	_float		  m_fSpinSpeed = 1.f;

	_vector		  m_vPlayerFocusPos;

	_bool		  m_bThrowFire[3] = {false,};

	_uint		  m_iFireIndex = 0;

	_double		  m_fRainSkillTime = 0.0; 

	_bool		  m_bRainStart = false;
	_bool		  m_bRainEnd   = false;

	_double		  m_fExitTime = 0.0;

	_bool		  m_isTurnFinish = false;

	_bool		  m_isFinishChase = false;

	_bool		  m_bEffect[4] = { false, };

	_double		  m_EffectInterval = 0.0;

	_bool		  m_bEffectDead = false;

	_bool		  m_bTurningBeforeAttack = false;

	_bool		  m_bCameraMoving = false;
public:
	static CState_FrozenWolfAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END