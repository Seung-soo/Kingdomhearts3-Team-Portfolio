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
	virtual HRESULT EnterState(); // 이 상태가 상태머신에 바인딩될 때 상태머신의 BindState 함수에서 한 번 실행시켜주는 함수
	virtual _bool	Action(_double TimeDelta); // 객체의 매 Tick 마다 돌릴 함수

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
	// 부모클래스인 State 에 GameObejct, Model, Transform 은 미리 정의되어있음
	CFrozenWolf*	m_pFrozenWolf= nullptr;
	MONSTERTYPE		m_eMonsterType = MONSTERTYPE_BIGTREE;
	CTransform*		m_pPlayerTransform = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	CFrozenWolf::WOLF_SKILL m_eFrozenWolfSkill = CFrozenWolf::WOLF_SKILL_END;

	// 시간누적값, 이펙트 실행변수, 사운드 실행변수 (나중에 필요할 때를 대비해서 미리 추가해둠)
	_double m_TimeAcc = 0.0;
	//_bool	m_bEffectEnd[3] = { false, };
	//_bool	m_bSoundEnd[3] = { false, };

	// 점프해서 앞으로 가는 파워 (점프공격일 때 사용할 예정)
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