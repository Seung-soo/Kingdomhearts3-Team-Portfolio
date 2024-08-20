#pragma once

#include "Client_Defines.h"
#include "MonsterDefines.h"
#include "State.h"
#include "IceMan.h"

BEGIN(Engine)
class CTransform;
class CMonster;
class CNavigation;
END

BEGIN(Client)

class CState_IceManAtk final : public CState
{
private:
	explicit CState_IceManAtk();
	virtual ~CState_IceManAtk() = default;

private: // 초기화 함수 (이 상태를 생성할 때 실행되는 함수)
	virtual HRESULT Initialize(void* pArg) override;

public:
	virtual HRESULT EnterState(); // 이 상태가 상태머신에 바인딩될 때 상태머신의 BindState 함수에서 한 번 실행시켜주는 함수
	virtual _bool	Action(_double TimeDelta); // 객체의 매 Tick 마다 돌릴 함수
	virtual HRESULT ClearState(); // 다른 상태의 생성으로 강제로 삭제될 때 호출되는 함수


private: // IceMan 의 각 공격에 대한 행동 설정
	_bool Atk_WalkSlash(_double TimeDelta);
	_bool Atk_Clap(_double TimeDelta);
	_bool Atk_LongSlash(_double TimeDelta);
	_bool Atk_JumpDownwardBlow(_double TimeDelta);

private:
	_bool ChasePlayer(_double TimeDelta);

private:
	void Create_IceBall(_double TimeDelta, _fvector vFirstDir);

private:
	// 부모클래스인 State 에 GameObejct, Model, Transform 은 미리 정의되어있음
	// 나머지 필요한 것들만 변수로 정해주면 됨
	CIceMan*		m_pIceMan            = nullptr;
	MONSTERTYPE		m_eMonsterType       = MONSTERTYPE_ICEMAN;
	CTransform*		m_pPlayerTransform   = nullptr;
	CNavigation*	m_pMonsterNavigation = nullptr;

	// IceMan 이 사용할 공격(함수를 바인딩할 때 매개변수로 받아서 설정해줄 것임)
	CIceMan::ICEMAN_SKILL m_eIceManSkill = CIceMan::ICEMAN_SKILL_END;

	// 시간누적값, 이펙트 실행변수, 사운드 실행변수 (나중에 필요할 때를 대비해서 미리 추가해둠)
	_double m_TimeAcc       = 0.0;
	_bool	m_bEffectEnd[3] = { false, };
	_bool	m_bSoundEnd[3]  = { false, };

	// 점프공격 관련 변수들
	_vector			m_vJumpDir;
	_double			m_GoPower;
	CGameObject*	m_pSkill = nullptr;

	// 박수공격 관련 변수
	_bool m_bPushEnd = false;

private:
	// 공격 애니메이션 인덱스
	// 이 게임의 대부분의 애니메이션들은 하나의 애니메이션을 여러개로 잘게 쪼개놓음
	// 그래서 점프공격도 그냥 애니메이션 하나가 아니라 35번~39번 애니메이션들을 순차적으로 실행시켜야함
	// Mosnter 클래스에 애니메이션을 순차적으로 실행시키기 위한 PlayAnimations 함수를 만들어뒀는데, 순차적으로 실행시킬 애니메이션을 vector 매개변수로 받기 때문에 미리 설정해둠
	// m_LongSlashIndices 변수같은 경우엔 한 애니메이션(32번)을 여러 번 실행시킬 것이라서 동일한 숫자를 반복해서 넣어뒀음
	vector<_uint> m_WalkSlash               = { 29 };
	vector<_uint> m_Clap                    = { 30, 30 };
	vector<_uint> m_LongSlashIndices        = { 31, 32, 32, 32, 32, 32, 32, 34 };
	vector<_uint> m_JumpDownwardBlowIndices = { 35, 36, 37, 38, 39 };

public:
	static CState_IceManAtk* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END