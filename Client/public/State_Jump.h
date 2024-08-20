#pragma once

#include "Client_Defines.h"
#include "State.h"

#include "Player.h"

BEGIN(Engine)
class CNavigation;
END

BEGIN(Client)

class CState_Jump final : public CState
{
public:
	typedef struct tagJumpStateStruct
	{
		CGameObject::OBJECT_TYPE eObjectType = CGameObject::OBJECT_TYPE::OBJECT_END;
		
		_uint iNumberOfJumpAnim	= 0;		// 점프 관련 애니메이션의 개수(ex : jump_up, jump_loop, jump_end일 경우 3개)
		_uint iJumpAnimIndex	= 0;		// 세팅된 애니메이션 체인에 접근할 인덱스 변수

		_bool bChangeAnimation = true;

	} JUMP_STATE_DESC;

public:
	explicit CState_Jump() = default;
	virtual ~CState_Jump() = default;

private:
	virtual HRESULT Initialize(void* pArg) override;
	virtual _bool KeyInput(_double TimeDelta) override;

public:
	virtual _bool Action(_double TimeDelta);

	virtual HRESULT EnterState() override;
	virtual HRESULT ExitState() override;

public:
	void Reset_State();					// 점프 상태 리셋
	void Change_Animation();			// 점프 애니메이션 체인을 통한 애니메이션 변경
	void Change_Animation_Falling();	// 낙하 애니메이션으로 변경

	// 점프 애니메이션 체인 세팅
	void SetUp_AnimationChain(_bool bSecond = false); 

	// 인자로 들어온 애니메이션이 점프 애니메이션 체인 안에 있는지 확인
	_bool Check_JumpAnimation(CPlayer::SORA_ANIM eAnimation); 

public:
	static _float g_Falling_Speed_Limit;
	static _float g_Falling_Speed_Limit_Hovering;
	static _double g_SecondJump_CoolDown;

private:
	CTransform::JUMP_DESC* m_JumpDesc = nullptr;

	class CPlayer*	m_pPlayer = nullptr;
	class CCombat_Assistant* m_pCombat_Assistant = nullptr;
	class CAction_Assistant* m_pAction_Assistant = nullptr;
	class CPlayer_Weapon* m_pPlayer_Weapon = nullptr;

	CNavigation*	m_pNavigation = nullptr;
	
	_bool m_bSecondJump = false;
	_double m_dSecondJumpTimer = 0.0;

	JUMP_STATE_DESC m_Desc;
	vector<_uint> m_JumpAnimChain;

public:
	static CState_Jump* Create(void* pArg = nullptr);
	virtual void Free() override;
};

END